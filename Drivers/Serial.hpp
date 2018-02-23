#ifndef DRIVER_SERIAL_H
#define DRIVER_SERIAL_H

#include "ISerial.hpp"

namespace Driver
{

class Serial : public ISerial
{
public:
	Serial() = default;
	virtual ~Serial() = default;
	void Init(int baudRate);
	int Write(const char* str);
	int Put(char c);
};

};
#endif // DRIVER_SERIAL_H


