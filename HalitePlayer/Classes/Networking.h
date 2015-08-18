#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML\Network.hpp>
#include <time.h>
#include <set>
#include <iostream>
#include <Windows.h>

#include "hlt.h"
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>

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

const std::string confirmation = "Done";

static void removeQueues(unsigned char playerTag) {
	boost::interprocess::message_queue::remove("playersize" + playerTag);
	boost::interprocess::message_queue::remove("size" + playerTag);
}

static unsigned unsigned char getTag()
{
	std::string in;
	unsigned char playerTag;
	std::cout << "What is this player's tag? Please enter a valid port number: ";
	while(true)
	{
		std::getline(std::cin, in);
		std::transform(in.begin(), in.end(), in.begin(), ::tolower);
		try
		{
			playerTag = std::stoi(in);
			break;
		}
		catch(std::exception e)
		{
			std::cout << "That isn't a valid input. Please enter a valid port number: ";
		}
	}

	removeQueues(playerTag);

	return playerTag;
}

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
	std::string initialQueueName = "playersize" + playerTag;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned int getSize(unsigned char playerTag) {
	std::string initialQueueName = "size" + playerTag;
	unsigned int priority;
	unsigned int size;
	boost::interprocess::message_queue::size_type recvd_size;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.receive(&size, sizeof(size), recvd_size, priority);
	return size;
}

static boost::interprocess::message_queue * setupInitialNetworking(unsigned char playerTag) {
	std::string stringQueueName = "initstring" + playerTag;
	boost::interprocess::message_queue::remove(stringQueueName.c_str());
	boost::interprocess::message_queue *stringQueue = new boost::interprocess::message_queue(boost::interprocess::create_only, stringQueueName.c_str(), 1, confirmation.size());

	return stringQueue;
}

static void getInit(unsigned char playerTag, unsigned char& ageOfSentient, hlt::Map& m)
{
	// Receive initpackage
	InitPackage package;
	
	while(true) {
		std::cout << "b4 sent\n";
		sendSize(playerTag, 6);
		std::cout << "sent\n";
		unsigned int size = getSize(playerTag);
		std::cout << "size: " << size << "\n";
	}

	/*std::string packageQueueName = "initpackage" + playerTag;
	std::cout << "waiting to receive package\n";
	while(true) {
			boost::interprocess::message_queue packageQueue(boost::interprocess::open_only, packageQueueName.c_str());
			std::cout << packageQueueName << "\n";
			receiveObject(&packageQueue, getMaxSize(package), package);
			break;

	}
	std::cout << "received package \n";
	ageOfSentient = package.ageOfSentient;
	m = package.map;
	if(playerTag != package.playerTag) {
		std::cout << "There was a problem with player tag assignment.\n";
		throw 1;
	}*/
}

/*static void getInit(sf::TcpSocket * s, unsigned char& playerTag, unsigned char& ageOfSentient, hlt::Map& m)
{
	sf::Packet r;
	s->receive(r);
	std::cout << "Received init message.\n";
	r >> playerTag >> ageOfSentient >> m;
}*/

static void sendInitResponse(boost::interprocess::message_queue *stringQueue) {
	stringQueue->send(confirmation.data(), confirmation.size(), 0);
}

/*
static void sendInitResponse(sf::TcpSocket * s)
{
	std::string response = "Done"; sf::Packet p;
	p << response;
	s->send(p);
	std::cout << "Sent init response.\n";
}*/

static void getFrame(unsigned char playerTag, hlt::Map& m)
{
	std::string mapQueueName = "map" + playerTag;
	boost::interprocess::message_queue mapQueue(boost::interprocess::open_only, mapQueueName.c_str());

	receiveObject(&mapQueue, m, playerTag);
}

static void sendFrame(unsigned char playerTag, std::set<hlt::Move>& moves)
{
	std::string movesQueueName = "moves" + playerTag;
	boost::interprocess::message_queue::remove(movesQueueName.c_str());
	boost::interprocess::message_queue movesQueue(boost::interprocess::create_only, movesQueueName.c_str(), 1, moves.max_size());
	sendObject(&movesQueue, moves, playerTag);
}

#endif