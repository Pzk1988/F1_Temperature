#ifndef CRC_HPP
#define CRC_HPP
#include "stdint.h"

class Crc
{
public:
	static bool Crc8(uint8_t* pData, uint8_t len);

private:
	static const unsigned char crc_table[];
};

#endif // CRC_HPP
