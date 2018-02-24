#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
#include <stdint.h>
#include "OneWire.hpp"

class Configuration
{
public:
	static uint16_t GetTempSendorAmount()
	{
		return tempSensorAmount;
	}

	static Drivers::ONE_WIRE_ADDR* GetSensorList()
	{
		return tempSensors;
	}

	static uint8_t GetId()
	{
		return ID;
	}

private:
	static const uint8_t ID = 4;
	static const uint16_t tempSensorAmount = 2;
	static Drivers::ONE_WIRE_ADDR tempSensors[tempSensorAmount];
};

#endif // CONFIGURATION_HPP
