// ECE 485 Project
// Edward Sayers
// Nathan Becker

#include "ece485-cache.h"
using namespace std;

//
// Cache line
//

// Constructor for cache line
// INPUT:	index_bits, Number of bits used for the index
//			byte_bits, Number of bits used for the byte offset
Cache_line::Cache_line(unsigned int index_bits, unsigned int byte_bits)
{
	this->index_bits = index_bits;
	this->byte_bits = byte_bits;
    state = MESIF_INVALID;
}

//
// Cache Set
//

// Constructor for cache set
// INPUT:	assoc, Associativity of set
//			index_bits, number of bits used for index
//			byte_bits, number of bits used for byte offset
Cache_set::Cache_set(unsigned int assoc, unsigned int index_bits, unsigned int byte_bits)
{
	this->assoc = assoc;
	this->index_bits = index_bits;
	this->byte_bits = byte_bits;
	lines = new Cache_line * [assoc];
}

// Destructor for cache set
Cache_set::~Cache_set(void)
{
	for (unsigned int i = 0; i < assoc; ++i) delete lines[i];
	delete[] lines;
}

//
// Cache
//

// Constructor for cache
// INPUT:	total_size, log base 2 of total size of cache
//			line_size, log base 2 of size of one line
//			assoc_pow, log base 2 of associativity of cache
Cache::Cache(unsigned int total_size, unsigned int line_size, unsigned int assoc_pow)
{
	byte_bits = line_size;
	index_bits = total_size - line_size - assoc_pow;
	assoc = (1 << assoc_pow);
	num_sets = (1 << index_bits);
	sets = new Cache_set *[num_sets];
}

// Destructor for cache
Cache::~Cache()
{
	for (unsigned int i = 0; i < num_sets; ++i) delete sets[i];
	delete[] sets;
}

int Cache::test(void)
{
	sets[0] = new Cache_set(assoc, index_bits, byte_bits);
	sets[0]->lines[0] = new Cache_line(index_bits, byte_bits);
	return 0;
}
