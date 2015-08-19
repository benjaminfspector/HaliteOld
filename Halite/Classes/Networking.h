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
#include <cstdlib> 

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

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

typedef boost::interprocess::allocator<hlt::Move, boost::interprocess::managed_shared_memory::segment_manager>  ShmemAllocator;
typedef boost::interprocess::set<hlt::Move, std::less<hlt::Move>, ShmemAllocator> MySet;

static void test(unsigned char playerTag) {
	using namespace boost::interprocess;

	shared_memory_object::remove("MySharedMemory" + playerTag);
	//Create a new segment with given name and size
	managed_shared_memory segment(create_only, "MySharedMemory" + playerTag, 65536);

	//Initialize shared memory STL-compatible allocator
	const ShmemAllocator alloc_inst(segment.get_segment_manager());

	//Construct a vector named "MySet" in shared memory with argument alloc_inst
	MySet *mySet = segment.construct<MySet>("MySet")(alloc_inst);

	hlt::Move move = { { 0, 0 }, 1 };

	mySet->clear();

	for(int i = 0; i < 100; ++i) mySet->insert(move);

	//if(segment.find<MySet>("MySet").first) std::cout << "worked\n";

	//segment.destroy<MySet>("MySet");

	//if(segment.find<MySet>("MySet").first) std::cout << "didnt work\n";
	//else std::cout << "worked again\n";
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
	std::string initialQueueName = "size" + (short)playerTag;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.send(&size, sizeof(size), 0);
}

static unsigned int getSize(unsigned char playerTag) 
{
	std::string initialQueueName = "playersize" + (short)playerTag;
	unsigned int priority;
	unsigned int size;
	boost::interprocess::message_queue::size_type recvd_size;
	boost::interprocess::message_queue sizeQueue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, sizeof(unsigned int));
	sizeQueue.receive(&size, sizeof(size), recvd_size, priority);
	return size;
}

static double handleInitNetworking(unsigned char playerTag, unsigned char ageOfSentient, std::string name, hlt::Map& m)
{
	test(playerTag);
	hlt::Move exampleMove = { { USHRT_MAX, USHRT_MAX }, UCHAR_MAX };
	moveSize = getMaxSize(exampleMove);

	hlt::Map exampleMap(m);
	for(int a = 0; a < exampleMap.contents.size(); a++) for(int b = 0; b < exampleMap.contents[a].size(); b++) exampleMap.contents[a][b] = { UCHAR_MAX, UCHAR_MAX };
	mapSize = getMaxSize(exampleMap);

	InitPackage package = { playerTag, ageOfSentient, m };
	unsigned int packageSize = getMaxSize(package);

	sendSize(playerTag, mapSize);
	sendSize(playerTag, packageSize);

	// Send Init package
	std::string initialQueueName = "initpackage" + (short)playerTag;
	std::cout << "max: " << packageSize << "\n";
	std::cout << "m: " << m.contents.size() << "\n";
	std::cout << "pack: " << package.map.contents.size() << "\n";
	boost::interprocess::message_queue packageQueue = boost::interprocess::message_queue(boost::interprocess::open_or_create, initialQueueName.c_str(), 1, packageSize);
	sendObject(packageQueue, package);

	// Receive confirmation
	std::string stringQueueName = "initstring" + (short)playerTag;
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

static double handleFrameNetworking(unsigned char playerTag, hlt::Map &m, std::set<hlt::Move> * moves)
{
	// Sending Map
	boost::interprocess::managed_shared_memory mapSegment(boost::interprocess::open_or_create, "map" + (short)playerTag, 65536);
	hlt::Map *map = mapSegment.find<hlt::Map>("map").first;
	if(!map) {
		std::cout << "ya\n\n\n\n";
		map = mapSegment.construct<hlt::Map>("map")();
	}
	map->contents = std::vector< std::vector<hlt::Site> >(m.contents);
	map->map_width = m.map_width;
	map->map_height = m.map_height;

	sendSize(playerTag, 1);

	// Receiving moves
	moves->clear();

	clock_t initialTime = clock();
	getSize(playerTag);
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	boost::interprocess::managed_shared_memory movesSegment(boost::interprocess::open_or_create, "moves" + (short)playerTag, 65536);
	MySet *mySet = movesSegment.find<MySet>("MySet").first;
	for(auto a = mySet->begin(); a != mySet->end(); ++a)
	{
		moves->insert(*a);
	}

	return timeElapsed;

	/*
	std::string mapQueueName = "map" + (short)playerTag;
	boost::interprocess::message_queue mapQueue(boost::interprocess::open_or_create, mapQueueName.c_str(), 1, mapSize);
	sendObject(mapQueue, m);
	
	
	std::string movesQueueName = "moves" + (short)playerTag;
	boost::interprocess::message_queue movesQueue(boost::interprocess::open_or_create, movesQueueName.c_str(), 10, moveSize);

	moves->clear();

	clock_t initialTime = clock();
	unsigned int numberOfMoves = getSize(playerTag);
	for(int a = 0; a < numberOfMoves; a++) 
	{
		hlt::Move move;
		receiveObject(movesQueue, moveSize, move);
		moves->insert(move);
	}
	clock_t finalTime = clock() - initialTime;
	double timeElapsed = float(finalTime) / CLOCKS_PER_SEC;

	return timeElapsed;*/
}

#endif