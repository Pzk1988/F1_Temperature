#ifndef C_INTERFACE_HPP
#define C_INTERFACE_HPP
#include "ISerial.hpp"
#include <stdint.h>

extern "C" void SerialWraperPut(char c);
extern Driver::ISerial* serial;
extern volatile uint64_t time;
extern uint64_t GetClockTick();

#endif // C_INTERFACE_HPP
