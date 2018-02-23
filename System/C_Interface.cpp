#include "C_Interface.hpp"

volatile uint64_t time = 0;

extern "C" void SerialWraperPut(char c)
{
	serial->Put(c);
}


uint64_t GetClockTick()
{
	return time;
}
