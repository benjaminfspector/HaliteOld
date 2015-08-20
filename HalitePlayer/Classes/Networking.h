#ifndef NETWORKING_H
#define NETWORKING_H

#include <SFML\Network.hpp>
#include <time.h>
#include <set>
#include <iostream>
#include <Windows.h>
#include <cstdlib> 

#include "hlt.h"
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

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

typedef boost::interprocess::allocator<hlt::Move, boost::interprocess::managed_shared_memory::segment_manager>  SharedMemoryAllocator;
typedef boost::interprocess::set<hlt::Move, std::less<hlt::Move>, SharedMemoryAllocator> MoveSet;

typedef boost::interprocess::allocator<void, boost::interprocess::managed_shared_memory::segment_manager> VoidAllocator;
typedef boost::interprocess::allocator<hlt::Site, boost::interprocess::managed_shared_memory::segment_manager>  SiteAllocator;
typedef boost::interprocess::vector<hlt::Site, SiteAllocator> SiteVector;
typedef boost::interprocess::allocator<SiteVector, boost::interprocess::managed_shared_memory::segment_manager>  SiteVectorAllocator;
typedef boost::interprocess::vector<SiteVector, SiteVectorAllocator> MapContents;

static void removeQueues(unsigned char playerTag) 
{
	boost::interprocess::message_queue::remove("playersize" + (short)playerTag);
	boost::interprocess::message_queue::remove("size" + (short)playerTag);
	boost::interprocess::message_queue::remove("initpackage" + (short)playerTag);
	boost::interprocess::message_queue::remove("initstring" + (short)playerTag);
	boost::interprocess::message_queue::remove("map" + (short)playerTag);
	boost::interprocess::shared_memory_object::remove("map" + (short)playerTag);
	boost::interprocess::message_queue::remove("moves" + (short)playerTag);
	boost::interprocess::shared_memory_object::remove("moves" + (short)playerTag);
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
	std::string initialQueueName = "playersize" + (short)playerTag;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned int getSize(unsigned char playerTag) 
{
	std::string initialQueueName = "size" + (short)playerTag;
	unsigned int priority;
	unsigned int size;
	boost::interprocess::message_queue::size_type recvd_size;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.receive(&size, sizeof(size), recvd_size, priority);
	return size;
}

static void getInit(unsigned char playerTag, unsigned char& ageOfSentient, hlt::Map& m)
{
	hlt::Move exampleMove = { { USHRT_MAX, USHRT_MAX }, UCHAR_MAX };
	moveSize = getMaxSize(exampleMove);

	mapSize = getSize(playerTag);
	unsigned int packageSize = getSize(playerTag);
	
	// Receive initpackage
	InitPackage package;
	std::string packageQueueName = "initpackage" + (short)playerTag;
	std::cout << "max: " << packageSize << "\n";
	boost::interprocess::message_queue packageQueue(boost::interprocess::open_or_create, packageQueueName.c_str(), 1, packageSize);
	receiveObject(packageQueue, packageSize, package);

	std::cout << "pack: " << package.map.contents.size() << "\n";

	std::cout << "Age of sentient: " << (unsigned short)package.ageOfSentient << "\n";

	ageOfSentient = package.ageOfSentient;
	m = package.map;
	if(playerTag != package.playerTag)
	{
		std::cout << "There was a problem with player tag assignment.\n";
		throw 1;
	}
}

static void sendInitResponse(unsigned int playerTag) 
{
	std::cout << "gonna send...\n";
	std::string stringQueueName = "initstring" + (short)playerTag;
	boost::interprocess::message_queue stringQueue(boost::interprocess::open_or_create, stringQueueName.c_str(), 1, confirmation.size());

	std::cout << "sending...\n";
	stringQueue.send(confirmation.data(), confirmation.size(), 0);
	std::cout << "sent...\n";
}

static void getFrame(unsigned char playerTag, hlt::Map& m)
{
	getSize(playerTag);

	m.contents.clear();
	boost::interprocess::managed_shared_memory mapSegment(boost::interprocess::open_only, "map" + (short)playerTag);
	MapContents *mapContents = mapSegment.find<MapContents>("map").first;
	for(auto a = mapContents->begin(); a != mapContents->end(); ++a)
	{
		std::vector<hlt::Site> mapRow;
		for(auto b = a->begin(); b != a->end(); ++b)
		{
			mapRow.push_back(*b);
		}
		m.contents.push_back(mapRow);
	}
}

static void sendFrame(unsigned char playerTag, std::set<hlt::Move>& moves)
{
	boost::interprocess::managed_shared_memory segment(boost::interprocess::open_or_create, "moves" + (short)playerTag, 65536);
	MoveSet *mySet = segment.find<MoveSet>("moves").first;
	if(!mySet) {
		std::cout << "yea\n\n\n\n";
		const SharedMemoryAllocator alloc_inst(segment.get_segment_manager());
		mySet = segment.construct<MoveSet>("moves")(alloc_inst);
	}

	mySet->clear();
	for(auto a = moves.begin(); a != moves.end(); ++a)
	{
		mySet->insert(*a);
	}
	sendSize(playerTag, moves.size());
}

#endif