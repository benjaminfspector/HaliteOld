#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML/Network.hpp>
#include <time.h>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <cfloat>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/lexical_cast.hpp>

#include "GameLogic/hlt.h"

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

static double total = 0;
static unsigned int times = 0;

typedef boost::interprocess::allocator<hlt::Move, boost::interprocess::managed_shared_memory::segment_manager>  MoveAllocator;
typedef boost::interprocess::set<hlt::Move, std::less<hlt::Move>, MoveAllocator> MoveSet;

typedef boost::interprocess::allocator<void, boost::interprocess::managed_shared_memory::segment_manager> VoidAllocator;
typedef boost::interprocess::allocator<hlt::Site, boost::interprocess::managed_shared_memory::segment_manager>  SiteAllocator;
typedef boost::interprocess::vector<hlt::Site, SiteAllocator> SiteVector;
typedef boost::interprocess::allocator<SiteVector, boost::interprocess::managed_shared_memory::segment_manager>  SiteVectorAllocator;
typedef boost::interprocess::vector<SiteVector, SiteVectorAllocator> MapContents;

static boost::interprocess::managed_shared_memory *mapSegment;


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
static unsigned short getMaxSize(type object)
{
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive & object;
	std::string serializedString(archiveStream.str());

	return serializedString.size();
}

static void sendSize(unsigned char playerTag, unsigned short size) 
{
	std::string initialQueueName = "size" + std::to_string(playerTag);
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned short));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned short getSize(unsigned char playerTag)
{
	std::string initialQueueName = "playersize" + std::to_string(playerTag);
	unsigned int priority;
	unsigned int size;
	boost::interprocess::message_queue::size_type recvd_size;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned short));
	sizeQueue.receive(&size, sizeof(size), recvd_size, priority);
	return size;
}

static void setupMapMemory() {
	mapSegment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, "map", 65536);
}

static double handleInitNetworking(unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map &m, boost::interprocess::managed_shared_memory *&movesSegment)
{
	// Get and send sizes of objects
	InitPackage package = { playerTag, ageOfSentient, m };
	unsigned int packageSize = getMaxSize(package);

	sendSize(playerTag, packageSize);
	// Send Init package
	std::string initialQueueName = "initpackage" + std::to_string(playerTag);
	boost::interprocess::message_queue packageQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, packageSize);
	sendObject(packageQueue, package);

	// Receive confirmation
	std::string stringQueueName = "initstring" + std::to_string(playerTag);
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

	// Get rid of initial message queues
    boost::interprocess::message_queue::remove(std::string("initpackage" + std::to_string(playerTag)).c_str());
    boost::interprocess::message_queue::remove(std::string("initstring" + std::to_string(playerTag)).c_str());
	
	// Setup memory
                                               movesSegment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, std::string("moves" + std::to_string(playerTag)).c_str(), 65536);


	if(stringQueueString != confirmation) return FLT_MAX;
	return timeElapsed;
}

static void sendMap(hlt::Map &m) {
	VoidAllocator allocator(mapSegment->get_segment_manager());
	MapContents *mapContents = mapSegment->find<MapContents>("map").first;
	if(!mapContents)
	{
		mapContents = mapSegment->construct<MapContents>("map")(allocator);
	}

	mapContents->clear();
	for(int a = 0; a < m.map_height; a++) {
		mapContents->push_back(SiteVector(m.contents[a].begin(), m.contents[a].end(), allocator));
	}
	
}

static double handleFrameNetworking(unsigned char playerTag, boost::interprocess::managed_shared_memory *movesSegment, std::set<hlt::Move> * moves)
{
	sendSize(playerTag, 1);
	clock_t initialTime = clock();
	// Wait until player has finished
	getSize(playerTag);
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	moves->clear();
	MoveSet mySet = *(movesSegment->find<MoveSet>("moves").first);
	std::cout << "size: " << mySet.size() << "\n";
	*moves = std::set<hlt::Move>(mySet.begin(), mySet.end());

	sendSize(playerTag, 1);

	return timeElapsed;
}

#endif