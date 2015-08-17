#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML\Network.hpp>
#include <time.h>
#include <set>
#include <vector>
#include <iostream>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>

#include "GameLogic\hlt.h"

struct InitPackage {
	unsigned char playerTag;
	unsigned char ageOfSentient;
	hlt::Map map;
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & playerTag;
		ar & ageOfSentient;
		ar & map;
	}
};

size_t const kMaxInitPackageSize = sizeof(InitPackage);
size_t const kMaxInitStringSize = 0x100;

template<class type>
static void sendObject(boost::interprocess::message_queue *queue, type const &objectToBeSent) {
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive << objectToBeSent;
	std::string serializedString(archiveStream.str());
	queue->send(serializedString.data(), serializedString.size(), 0);
}

template<class type>
static void receiveObject(boost::interprocess::message_queue *queue, unsigned int maxSize, type &receivingObject) {
	boost::interprocess::message_queue::size_type messageSize;
	unsigned int priority;
	std::stringstream stream;
	std::string serializedString;
	serializedString.resize(maxSize);

	queue->receive(&serializedString[0], maxSize, messageSize, priority);

	stream << serializedString;
	boost::archive::text_iarchive archive(stream);
	archive >> receivingObject;
}

static double handleInitNetworking(unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map& m)
{
	InitPackage package = {playerTag, ageOfSentient, m};

	try 
	{
		// Send an InitPackage to player
		std::string packageQueueName = "initpackage" + playerTag;
		boost::interprocess::message_queue::remove(packageQueueName.c_str());
		boost::interprocess::message_queue packageQueue(boost::interprocess::create_only, packageQueueName.c_str(), 1, kMaxInitPackageSize);

		sendObject(&packageQueue, package);

		std::string str = "Init Message sent to player " + name + "\n";
		std::cout << str;
		
		// Receive a confirmation string from player
		std::string stringQueueName = "initstring" + playerTag;
		boost::interprocess::message_queue stringQueue(boost::interprocess::open_only, stringQueueName.c_str());
		
		boost::interprocess::message_queue::size_type messageSize;
		unsigned int priority;
		std::string stringQueueString;
		stringQueueString.resize(kMaxInitStringSize);

		clock_t initialTime = clock();
		stringQueue.receive(&stringQueueString[0], stringQueueString.size(), messageSize, priority);
		stringQueueString.resize(messageSize);
		str = "Init Message received from player " + name + "\n";
		std::cout << str;
		clock_t finalTime = clock() - initialTime;
		double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

		if(stringQueueString != "Done") return FLT_MAX;
		return timeElapsed;
	} 
	catch(boost::interprocess::interprocess_exception &ex) 
	{
		std::cout << ex.what() << std::endl;
		return 1;
	}
}

static boost::interprocess::message_queue* createMapQueue(unsigned char playerTag) {
	std::string mapQueueName = "map" + playerTag;
	boost::interprocess::message_queue::remove(mapQueueName.c_str());
	boost::interprocess::message_queue *mapQueue = new boost::interprocess::message_queue(boost::interprocess::open_or_create, mapQueueName.c_str(), 1, sizeof(hlt::Map));
	return mapQueue;
}

static boost::interprocess::message_queue* createMovesQueue(unsigned char playerTag) {
	std::string movesQueueName = "moves" + playerTag;
	boost::interprocess::message_queue::remove(movesQueueName.c_str());
	boost::interprocess::message_queue *movesQueue = new boost::interprocess::message_queue(boost::interprocess::open_only, movesQueueName.c_str());
	return movesQueue;
}

static double handleFrameNetworking(unsigned char playerTag, hlt::Map const &m, std::set<hlt::Move> * moves)
{
	// Sending Map
	boost::interprocess::message_queue *mapQueue = createMapQueue(playerTag);
	sendObject(mapQueue, m);
	delete mapQueue;

	// Receiving moves
	std::set<hlt::Move> receiveMoves;
	boost::interprocess::message_queue *movesQueue = createMovesQueue(playerTag);

	clock_t initialTime = clock();
	receiveObject(movesQueue, receiveMoves.max_size(), receiveMoves);
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	delete movesQueue;

	*moves = receiveMoves;

	return timeElapsed;
}


#endif