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
#include <boost/interprocess/windows_shared_memory.hpp>
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

static boost::interprocess::managed_shared_memory *mapSegment = 0;
static boost::interprocess::managed_shared_memory *movesSegment = 0;

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
	boost::interprocess::shared_memory_object::remove("map");
	boost::interprocess::shared_memory_object::remove("moves" + (short)playerTag);
}

static unsigned unsigned char getTag()
{
	std::string in;
	unsigned char playerTag;
	std::cout << "What is this player's tag? Please enter a valid player tag: ";
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
			std::cout << "That isn't a valid input. Please enter a valid player tag: ";
		}
	}

	removeQueues(playerTag);

	return playerTag;
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
	std::string initialQueueName = "playersize" + (short)playerTag;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned short));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned short getSize(unsigned char playerTag)
{
	std::string initialQueueName = "size" + (short)playerTag;
	unsigned int priority;
	unsigned int size;
	boost::interprocess::message_queue::size_type recvd_size;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned short));
	sizeQueue.receive(&size, sizeof(size), recvd_size, priority);
	return size;
}

static void initNetwork(unsigned char playerTag, unsigned char& ageOfSentient, hlt::Map &m, MoveSet *&moves)
{

	unsigned int packageSize = getSize(playerTag);
	
	// Receive initpackage
	InitPackage package;
	std::string packageQueueName = "initpackage" + (short)playerTag;
	boost::interprocess::message_queue packageQueue(boost::interprocess::open_or_create, packageQueueName.c_str(), 1, packageSize);
	receiveObject(packageQueue, packageSize, package);

	ageOfSentient = package.ageOfSentient;
	m = package.map;

	if(playerTag != package.playerTag)
	{
		std::cout << "There was a problem with player tag assignment.\n";
		throw 1;
	}
	
	// Send response
	std::string stringQueueName = "initstring" + (short)playerTag;
	boost::interprocess::message_queue stringQueue(boost::interprocess::open_or_create, stringQueueName.c_str(), 1, confirmation.size());

	stringQueue.send(confirmation.data(), confirmation.size(), 0);
	
	// Setup memory
	mapSegment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, "map", 65536);
	movesSegment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, "moves" + (short)playerTag, 65536);

	// Setup moves set
	const SharedMemoryAllocator alloc_inst(movesSegment->get_segment_manager());
	moves = movesSegment->construct<MoveSet>("moves")(alloc_inst);
}

static void getFrame(unsigned char playerTag, hlt::Map &m)
{
	getSize(playerTag);
	
	MapContents *mapContents = mapSegment->find<MapContents>("map").first;
	m.contents.clear();
	for(auto a = mapContents->begin(); a != mapContents->end(); a++)
	{
		m.contents.push_back(std::vector<hlt::Site>(a->begin(), a->end()));
	}
	m.map_height = m.contents.size();
	m.map_width = m.contents[0].size();
}

static void sendFrame(unsigned char playerTag)
{
	sendSize(playerTag, 1);
	getSize(playerTag);
}

#endif