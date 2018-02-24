#include <Ds18B20.hpp>
#include <cstring>

namespace Drivers
{

Ds18B20::Ds18B20(OneWire& rOneWire, uint8_t* pAddress) : rOneWire(rOneWire)
{
	memcpy(address, pAddress, sizeof(address));
}

void Ds18B20::SetId(uint8_t* pAddress)
{
	memcpy(address, pAddress, sizeof(address));
}

void Ds18B20::Convert(bool blocking)
{
	rOneWire.MatchRom(address);
	rOneWire.WriteByte(static_cast<uint8_t>(DS18B20_CMD::CONVERT));

	if( blocking == true)
	{
		while( rOneWire.ReadBit() == false ){};
	}
}

void Ds18B20::GetResult(uint8_t* pResult)
{
	rOneWire.MatchRom(address);
	rOneWire.WriteByte(static_cast<uint8_t>(DS18B20_CMD::READ_SCRATCHPAD));
	for(size_t i = 0; i < 9; i++)
	{
		pResult[i] = rOneWire.ReadByte();
	}
}

void Ds18B20::SetPrecision(uint8_t precission)
{
	rOneWire.MatchRom(address);
	rOneWire.WriteByte(0x4E);

	rOneWire.WriteByte(0x4B);
	rOneWire.WriteByte(0x46);

	rOneWire.WriteByte(0x1F | (precission << 5));
}

} // namespace Drivers
