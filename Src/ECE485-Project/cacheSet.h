#include "cacheLine.h"
#include "AddressUtils.h"
#include <iostream>



class Cache_set;

// Cache set
class Cache_set {

	unsigned int assoc;			// Set associativity
	bool* lru_state;		// LRU state array
	Cache_line ** lines;		// Array of pointers to cache lines

public:
	Cache_set(unsigned int);
	~Cache_set();

	Cache_line* LookUpCacheLine(unsigned int);
	bool placeLineInCache(unsigned int, Mesif_state);

	friend class Cache;
	friend class CacheController;
private:
	void UpdateLru(int, int, int);

	int FindEvictLineInLru(int, int);

};