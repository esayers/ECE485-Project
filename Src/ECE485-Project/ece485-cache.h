// ECE 485 Cache Project
// Edward Sayers
// Nathan Becker


#include <fstream>
#include <string>
#include <sstream>
#include "cacheSet.h"


class Cache;


// Entire cache
// Constructor: n where 2^n = Total cache size in Bytes
//				n where 2^n = Line size in Bytes
//				n where 2^n = Associativity
class Cache {

	unsigned int total_size;	// Total size of cache in bytes
	unsigned int assoc;			// Set associativity
	unsigned int num_sets;		// Number of sets in cache
	int LineSizeBytes;
	int TagLength;
	int IndexLength;
	int ByteSelectLength;
	Cache_set ** sets;			// Array of pointers to sets

	//Statistics
	int CacheReads;
	int CacheWrites;
	int CacheHits;
	int CacheMisses;

public:
	Cache(unsigned int,unsigned int,unsigned int);
	~Cache();

	Cache_line* LookupCacheLine(unsigned int);

	bool PlaceLineInCache(unsigned int, Mesif_state);

	friend class CacheController;
};
