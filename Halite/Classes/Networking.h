#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML\Network.hpp>
#include <time.h>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>

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

		ar & map.map_width;
		ar & map.map_height;
		for(int a = 0; a < map.contents.size(); a++) {
			for(int b = 0; b < map.contents[a].size(); b++) {
				hlt::Site s = map.contents[a][b];
				ar & s.owner;
				ar & s.age;
			}
		}
	}
};

template<class type>
static void sendObject(boost::interprocess::message_queue *queue, type objectToBeSent, unsigned char playerTag) {
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive & objectToBeSent;
	std::string serializedString(archiveStream.str());

	sendSize(playerTag, serializedString.size());

	queue->send(serializedString.data(), serializedString.size(), 0);
}

template<class type>
static void receiveObject(boost::interprocess::message_queue *queue, type &receivingObject, unsigned char playerTag) {
	boost::interprocess::message_queue::size_type messageSize;
	unsigned int priority;
	unsigned int maxSize = getSize(playerTag);
	std::stringstream stream;
	std::string serializedString;
	serializedString.resize(maxSize);

	queue->receive(&serializedString[0], maxSize, messageSize, priority);

	stream << serializedString;
	boost::archive::text_iarchive archive(stream);
	archive >> receivingObject;
}

template<class type>
static unsigned int getMaxSize(type object) {
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive & object;
	std::string serializedString(archiveStream.str());

	return serializedString.size();
}

static void sendSize(unsigned char playerTag, unsigned int size) {
	std::string initialQueueName = "size" + playerTag;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned int getSize(unsigned char playerTag) {
	std::string initialQueueName = "playersize" + playerTag;
	unsigned int priority;
	unsigned int size;
	boost::interprocess::message_queue::size_type recvd_size;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.receive(&size, sizeof(size), recvd_size, priority);
	return size;
}

static double handleInitNetworking(unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map& m)
{
	InitPackage package = {playerTag, ageOfSentient, m};
	while(true) {
		sendSize(playerTag, 4);
		std::cout << "sent\n";
		unsigned int size = getSize(playerTag);
		std::cout << "size: " << size << "\n";
	}

	// Send initial package
	/*std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive & package;
	std::string serializedString(archiveStream.str());

	std::string initialQueueName = "initpackage" + playerTag;
	std::cout << initialQueueName << "\n";
	boost::interprocess::message_queue::remove(initialQueueName.c_str());
	boost::interprocess::message_queue packageQueue = boost::interprocess::message_queue(boost::interprocess::create_only, initialQueueName.c_str(), 1, serializedString.size());

	packageQueue.send(serializedString.data(), serializedString.size(), 0);

	std::string str = "Init Message sent to player " + name + "\n";
	std::cout << str;
		
	// Receive a confirmation string from player
	std::string stringQueueName = "initstring" + playerTag;
	boost::interprocess::message_queue stringQueue(boost::interprocess::open_only, stringQueueName.c_str());
		
	boost::interprocess::message_queue::size_type messageSize;
	unsigned int priority;
	std::string stringQueueString;
	std::string confirmation = "Done";
	stringQueueString.resize(getMaxSize(confirmation));

	clock_t initialTime = clock();
	stringQueue.receive(&stringQueueString[0], stringQueueString.size(), messageSize, priority);
	stringQueueString.resize(messageSize);
	str = "Init Message received from player " + name + "\n";
	std::cout << str;
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	if(stringQueueString != "Done") return FLT_MAX;
	return timeElapsed;*/
	return 0;
}

static boost::interprocess::message_queue* createMapQueue(unsigned char playerTag) {
	std::string mapQueueName = "map" + playerTag;
	boost::interprocess::message_queue::remove(mapQueueName.c_str());
	boost::interprocess::message_queue *mapQueue = new boost::interprocess::message_queue(boost::interprocess::create_only, mapQueueName.c_str(), 1, sizeof(hlt::Map));
	return mapQueue;
}

static boost::interprocess::message_queue* createMovesQueue(unsigned char playerTag) {
	std::string movesQueueName = "moves" + playerTag;
	boost::interprocess::message_queue *movesQueue = new boost::interprocess::message_queue(boost::interprocess::open_only, movesQueueName.c_str());
	return movesQueue;
}

static double handleFrameNetworking(unsigned char playerTag, hlt::Map &m, std::set<hlt::Move> * moves)
{
	// Sending Map
	boost::interprocess::message_queue *mapQueue = createMapQueue(playerTag);
	sendObject(mapQueue, m, playerTag);
	delete mapQueue;

	// Receiving moves
	boost::interprocess::message_queue *movesQueue = createMovesQueue(playerTag);

	clock_t initialTime = clock();
	receiveObject(movesQueue, *moves, playerTag);
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	delete movesQueue;

	return timeElapsed;
}


#endif