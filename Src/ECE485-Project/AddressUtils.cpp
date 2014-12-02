#include "AddressUtils.h"
using namespace std;

int AddressUtils::GetByteSelect(int byteSelectLength, unsigned int address)
{
	//Start out with a mask filled with 1s
	unsigned int mask = ~0;
	//shift the bits to the left for the appropriate amount of times, filling the space with 0s

		mask = mask << byteSelectLength;

	//do an AND operation with the complement of the mask, so the 0s are 1s and the 1s are 0s, blocking
	//any of the higher order bits and allowing the byte select bits to come through
	return ~mask & address;

}

int AddressUtils::GetIndex(int tagLength, int byteSelectLength, unsigned int address)
{
	//First, shift the address to the right by the number of bits in the byteSelectLength variable
	unsigned int FinalIndex = address >> byteSelectLength;

	//next, generate a mask filled with 1s;
	unsigned int mask = ~0;

	//shift the bits to the left for the appropriate amount of times, filling the spaces with 0s
		mask = mask << (sizeof(address)*8 - tagLength - byteSelectLength);

	//do an AND operation with the complement of the mask, so the 0s are 1s and the 1s are 0s, blocking
	//any of the higher order bits and allowing the index bits to come through
	return ~mask & FinalIndex;

}

unsigned int AddressUtils::GetTag(int tagLength, unsigned int address)
{
	return address >> (sizeof(address)*8 - tagLength);
}