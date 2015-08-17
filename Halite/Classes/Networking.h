#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML\Network.hpp>
#include <time.h>
#include <set>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>

#include "GameLogic\hlt.h"

struct InitPackage {
	unsigned char playerTag;
	unsigned char ageOfSentient;
	hlt::Map m;
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & playerTag;
		ar & ageOfSentient;
		ar & m;
	}
};

size_t const kMaxInitPackageSize = sizeof(InitPackage);
size_t const kMaxInitStringSize = 0x100;

static sf::Packet& operator<<(sf::Packet& p, const hlt::Map& m)
{
	p << m.map_width << m.map_height;
	for(auto a = m.contents.begin(); a != m.contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++) p << b->owner << b->age;
	return p;
}

static double handleInitNetworking(unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map& m)
{
	InitPackage const &package = {playerTag, ageOfSentient, m};

	try 
	{
		// Send an InitPackage to player
		std::string sendQueueName = "initpackage" + playerTag;
		boost::interprocess::message_queue::remove(sendQueueName.c_str());
		boost::interprocess::message_queue sendQueue(boost::interprocess::open_or_create, sendQueueName.c_str(), 1, kMaxInitPackageSize);

		std::ostringstream archiveStream;
		boost::archive::text_oarchive archive(archiveStream);
		archive << package;
		std::string serializedString(archiveStream.str());
		sendQueue.send(serializedString.data(), serializedString.size(), 0);

		std::string str = "Init Message sent to player " + name + "\n";
		std::cout << str;
		
		// Receive a confirmation string from player
		std::string receiveQueueName = "initstring" + playerTag;
		boost::interprocess::message_queue receiveQueue(boost::interprocess::open_only, receiveQueueName.c_str());
		
		boost::interprocess::message_queue::size_type messageSize;
		unsigned int priority;
		std::string receiveString;
		receiveString.resize(kMaxInitStringSize);

		clock_t initialTime = clock();
		receiveQueue.receive(&receiveString[0], receiveString.size(), messageSize, priority);
		receiveString.resize(messageSize);
		str = "Init Message received from player " + name + "\n";
		std::cout << str;
		clock_t finalTime = clock() - initialTime;
		double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

		if(receiveString != "Done") return FLT_MAX;
		return timeElapsed;
	} 
	catch(boost::interprocess::interprocess_exception &ex) 
	{
		std::cout << ex.what() << std::endl;
		return 1;
	}
}

static boost::interprocess::message_queue* createMapQueue(unsigned char playerTag) {
	std::string sendQueueName = "map" + playerTag;
	boost::interprocess::message_queue::remove(sendQueueName.c_str());
	boost::interprocess::message_queue *mapQueue = new boost::interprocess::message_queue(boost::interprocess::open_or_create, sendQueueName.c_str(), 1000000, sizeof(hlt::Map));
	return mapQueue;
}

static boost::interprocess::message_queue* createMovesQueue(unsigned char playerTag) {
	std::string sendQueueName = "moves" + playerTag;
	std::set<hlt::Map> expampleMapSet;
	boost::interprocess::message_queue::remove(sendQueueName.c_str());
	boost::interprocess::message_queue *movesQueue = new boost::interprocess::message_queue(boost::interprocess::open_or_create, sendQueueName.c_str(), 1000000, expampleMapSet.max_size());
	return movesQueue;
}

/*static double handleInitNetworking(sf::TcpSocket * s, unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map& m)
{
	sf::Packet p;
	p << playerTag << ageOfSentient << m;
	s->send(p);
	std::string str = "Init Message sent to player " + name + "\n";
	std::cout << str;
	clock_t initialTime = clock();
	sf::Packet r;
	s->receive(r);
	str = "Init Message received from player " + name + "\n";
	std::cout << str;
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;
	str = ""; unsigned char readChar;
	while(!r.endOfPacket())
	{
		r >> readChar;
		str += readChar;
	}
	if(str != "Done") return FLT_MAX;
	return timeElapsed;
}*/

static double handleFrameNetworking(boost::interprocess::message_queue *mapQueue, boost::interprocess::message_queue *movesQueue, hlt::Map const &m, std::set<hlt::Move> * moves)
{
	// Sending Map
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive << m;
	std::string serializedString(archiveStream.str());
	mapQueue->send(serializedString.data(), serializedString.size(), 0);

	// Receiving moves
	boost::interprocess::message_queue::size_type messageSize;
	unsigned int priority;
	std::stringstream inputStringStream;
	std::set<hlt::Move> receiveMoves;

	clock_t initialTime = clock();

	serializedString = "";
	serializedString.resize(moves->max_size());
	movesQueue->receive(&serializedString[0], moves->max_size(), messageSize, priority);

	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	inputStringStream << serializedString;
	boost::archive::text_iarchive inArchive(inputStringStream);
	inArchive >> receiveMoves;

	*moves = receiveMoves;
	
	return timeElapsed;
}

/*static double handleFrameNetworking(sf::TcpSocket * s, hlt::Map& m, std::set<hlt::Move> * moves)
{
	sf::Packet p;
	p << m;
	s->send(p);
	clock_t initialTime = clock();
	sf::Packet r;
	s->receive(r);
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;
	moves->clear();
	unsigned short x, y; unsigned char d;
	while(!r.endOfPacket())
	{
		r >> x >> y >> d;
		moves->insert({ { x, y }, d });
	}
	return timeElapsed;
}*/

#endif