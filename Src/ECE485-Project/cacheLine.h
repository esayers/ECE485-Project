// ECE 485 Cache Project
// Edward Sayers
// Nathan Becker

#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include "CacheConstants.h"

class Cache_line;
// Single cache line
class Cache_line {

	Mesif_state State;			// MESIF state of line
	unsigned int Tag;			// Current tag

public:
	Cache_line(unsigned int, Mesif_state);
	friend class Cache;
	friend class Cache_set;
	friend class CacheController;

};

#endif