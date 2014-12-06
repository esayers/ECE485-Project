// ECE 485 Cache Project
// Edward Sayers
// Nathan Becker

#ifndef ADDRESS_UTILS_H
#define ADDRESS_UTILS_H

 class AddressUtils{
public:
	static int GetByteSelect(int, unsigned int);
	static int GetIndex(int, int, unsigned int);
	static unsigned int GetTag(int, unsigned int);

};

#endif