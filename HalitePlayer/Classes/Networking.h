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
		ar & map;
	}
};

static boost::interprocess::message_queue * mapQueue;
static boost::interprocess::message_queue * movesQueue;

static unsigned int getTag()
{
	std::string in;
	unsigned short id;
	std::cout << "What is this player's tag? Please enter a valid port number: ";
	while(true)
	{
		std::getline(std::cin, in);
		std::transform(in.begin(), in.end(), in.begin(), ::tolower);
		try
		{
			id = std::stoi(in);
			break;
		}
		catch(std::exception e)
		{
			std::cout << "That isn't a valid input. Please enter a valid port number: ";
		}
	}
	return id;
}

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

static void getInit(unsigned char playerTag, unsigned char& ageOfSentient, hlt::Map& m)
{
	// Receive initpackage
	std::string packageQueueName = "initpackage" + playerTag;
	boost::interprocess::message_queue packageQueue(boost::interprocess::open_only, packageQueueName.c_str());
	InitPackage package;
	receiveObject(&packageQueue, sizeof(InitPackage), package);
	ageOfSentient = package.ageOfSentient;
	m = package.map;
	if(playerTag != package.playerTag) {
		std::cout << "There was a problem with player tag assignment.\n";
		throw 1;
	}

	// Send confirmation string
	std::string confirmation = "Done";
	std::string stringQueueName = "initstring" + playerTag;
	boost::interprocess::message_queue stringQueue(boost::interprocess::create_only, stringQueueName.c_str(), 1, sizeof(confirmation));
	stringQueue.send(confirmation.data(), confirmation.size(), 0);

}

static void getInit(sf::TcpSocket * s, unsigned char& playerTag, unsigned char& ageOfSentient, hlt::Map& m)
{
	sf::Packet r;
	s->receive(r);
	std::cout << "Received init message.\n";
	r >> playerTag >> ageOfSentient >> m;
}

static void sendInitResponse(sf::TcpSocket * s)
{
	std::string response = "Done"; sf::Packet p;
	p << response;
	s->send(p);
	std::cout << "Sent init response.\n";
}

static void setupFrameQueues(unsigned short playerTag) {
	std::string mapQueueName = "map" + playerTag;
	mapQueue = new boost::interprocess::message_queue(boost::interprocess::open_only, mapQueueName.c_str());

	std::string movesQueueName = "moves" + playerTag;
	movesQueue = new boost::interprocess::message_queue(boost::interprocess::open_only, movesQueueName.c_str());
}

static void getFrame(sf::TcpSocket * s, hlt::Map& m)
{
	sf::Packet r;
	s->receive(r);
	r >> m;
}

static void sendFrame(sf::TcpSocket * s, const std::set<hlt::Move>& moves)
{
	sf::Packet p;
	p << moves;
	s->send(p);
}

#endif