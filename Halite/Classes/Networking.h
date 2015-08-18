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

struct InitPackage 
{
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

const std::string confirmation = "Done";
static unsigned int mapSize = 0;
static unsigned int moveSize = 0;

template<class type>
static void sendObject(boost::interprocess::message_queue &queue, type objectToBeSent)
{
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive & objectToBeSent;
	std::string serializedString(archiveStream.str());

	queue.send(serializedString.data(), serializedString.size(), 0);
}

template<class type>
static void receiveObject(boost::interprocess::message_queue &queue, unsigned int maxSize, type &receivingObject)
{
	boost::interprocess::message_queue::size_type messageSize;
	unsigned int priority;
	std::stringstream stream;
	std::string serializedString;
	serializedString.resize(maxSize);

	queue.receive(&serializedString[0], maxSize, messageSize, priority);

	stream << serializedString;
	boost::archive::text_iarchive archive(stream);
	archive >> receivingObject;
}

template<class type>
static unsigned int getMaxSize(type object) 
{
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive & object;
	std::string serializedString(archiveStream.str());

	return serializedString.size();
}

static void sendSize(unsigned char playerTag, unsigned int size) 
{
	std::string initialQueueName = "size" + playerTag;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned int getSize(unsigned char playerTag) 
{
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
	
	hlt::Move exampleMove = { { 0, 0 }, 0 };
	moveSize = getMaxSize(exampleMove);
	mapSize = getMaxSize(m);
	InitPackage package = { playerTag, ageOfSentient, m };
	unsigned int packageSize = getMaxSize(package);
	
	sendSize(playerTag, mapSize);
	sendSize(playerTag, packageSize);

	// Send Init package
	std::string initialQueueName = "initpackage" + playerTag;
	std::cout << "max: " << packageSize << "\n";
	boost::interprocess::message_queue packageQueue = boost::interprocess::message_queue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, packageSize);
	sendObject(packageQueue, package);

	// Receive confirmation
	std::string stringQueueName = "initstring" + playerTag;
	boost::interprocess::message_queue stringQueue(boost::interprocess::open_or_create, stringQueueName.c_str(), 1, confirmation.size());

	boost::interprocess::message_queue::size_type messageSize;
	unsigned int priority;
	std::string stringQueueString;
	stringQueueString.resize(getMaxSize(confirmation));

	clock_t initialTime = clock();
	stringQueue.receive(&stringQueueString[0], stringQueueString.size(), messageSize, priority);
	stringQueueString.resize(messageSize);
	std::cout << "Init Message received from player " + name + "\n";
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	if(stringQueueString == confirmation) std::cout << "yes\n";

	if(stringQueueString != confirmation) return FLT_MAX;
	return timeElapsed;
}

static boost::interprocess::message_queue* createMapQueue(unsigned char playerTag) 
{
	std::string mapQueueName = "map" + playerTag;
	boost::interprocess::message_queue::remove(mapQueueName.c_str());
	boost::interprocess::message_queue *mapQueue = new boost::interprocess::message_queue(boost::interprocess::open_or_create, mapQueueName.c_str(), 1, sizeof(hlt::Map));
	return mapQueue;
}

static boost::interprocess::message_queue* createMovesQueue(unsigned char playerTag) 
{
	std::string movesQueueName = "moves" + playerTag;
	boost::interprocess::message_queue *movesQueue = new boost::interprocess::message_queue(boost::interprocess::open_only, movesQueueName.c_str());
	return movesQueue;
}

static double handleFrameNetworking(unsigned char playerTag, hlt::Map &m, std::set<hlt::Move> * moves)
{
	// Sending Map
	std::string mapQueueName = "map" + playerTag;
	boost::interprocess::message_queue mapQueue(boost::interprocess::open_or_create, mapQueueName.c_str(), 1, mapSize);
	sendObject(mapQueue, m);

	// Receiving moves
	std::string movesQueueName = "moves" + playerTag;
	boost::interprocess::message_queue movesQueue(boost::interprocess::open_or_create, movesQueueName.c_str(), 10, moveSize);

	clock_t initialTime = clock();
	// Read in moves
	unsigned int numberOfMoves = getSize(playerTag);
	for(int a = 0; a < numberOfMoves; a++) {
		hlt::Move move;
		receiveObject(movesQueue, moveSize, move);
		moves->insert(move);
	}
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;


	return timeElapsed;
}


#endif