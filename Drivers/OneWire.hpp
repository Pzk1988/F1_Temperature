#ifndef ONEWIRE_H
#define ONEWIRE_H
#include <stdint.h>
#include "stm32f10x_gpio.h"

namespace Drivers
{

struct ONE_WIRE_ADDR
{
	uint8_t address[8];
};

class OneWire
{

public:
	OneWire(GPIO_TypeDef* port, uint16_t pin, TIM_TypeDef* timer);
	virtual ~OneWire() = default;
	bool ResetPulse();
	void WriteBit(bool bit);
	bool ReadBit();
	void WriteByte(uint8_t data);
	uint8_t ReadByte();
	uint8_t WriteCrc(uint8_t data);
	void ReadRom(uint8_t* pData);
	int Search(ONE_WIRE_ADDR* pOneWireData);
	bool MatchRom(uint8_t* pAddress);
	uint8_t ValidateCrc(uint8_t data);


private:
	GPIO_TypeDef* port;
	uint16_t pin;
	TIM_TypeDef* timer;
	uint8_t crc8;
	uint8_t ROM_NO[8];

	static const unsigned char crc_table[];
	uint8_t lastMismatch;
	uint8_t lastDeviceFlag;

	int Search();
	void WriteOne();
	void WriteZero();
	void DelayUs(uint32_t time);
};

} // namespace Drivers

#endif // ONEWIRE_H
