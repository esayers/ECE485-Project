// ECE 485 Cache Project
// Edward Sayers
// Nathan Becker

#include <iostream>

enum Mesif_state {
		MESIF_INVALID,
		MESIF_EXCLUSIVE,
		MESIF_MODIFIED,
		MESIF_SHARED,
		MESIF_FORWARD,
};

class Cache;
class Cache_set;
class Cache_line;

// Single cache line
class Cache_line {

	Mesif_state state;              	// MESIF state of line
	unsigned int tag;					// Current tag

public:
	Cache_line(void);
	friend class Cache;
	friend class Cache_set;
};

// Cache set
class Cache_set {

	unsigned int assoc;			// Set associativity
	unsigned int lru_state;		// LRU state bits
	Cache_line ** lines;		// Array of pointers to cache lines

public:
	Cache_set(unsigned int);
	~Cache_set();
	friend class Cache;
};

// Entire cache
// Constructor: n where 2^n = Total cache size in Bytes
//				n where 2^n = Line size in Bytes
//				n where 2^n = Associativity
class Cache {
	unsigned int total_size;	// Total size of cache in bytes
	unsigned int assoc;			// Set associativity
	unsigned int num_sets;		// Number of sets in cache
	unsigned int byte_bits;		// Number of bits used for byte offset
	unsigned int index_bits;	// Number of bits used for index
	Cache_set ** sets;			// Array of pointers to sets

public:
	Cache(unsigned int,unsigned int,unsigned int);
	~Cache();

	int test(void);

};
