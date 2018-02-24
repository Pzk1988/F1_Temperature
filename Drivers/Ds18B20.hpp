#ifndef DS18B20_H
#define DS18B20_H
#include "OneWire.hpp"

namespace Drivers
{

class Ds18B20
{
public:
	enum class DS18B20_CMD
	{
		READ_ROM = 0x33,
		MATCH_ROM = 0x55,
		SKIP_ROM = 0xCC,
		ALARM_SEARCH = 0xEC,
		CONVERT = 0x44,
		WRITE_SCRATCHPAD = 0x4E,
		READ_SCRATCHPAD = 0xBE,
		COPY_SCRATCHPAD = 0x48,
	};

public:
	Ds18B20(OneWire& rOneWire, uint8_t* pAddress);
	void SetId(uint8_t* pAddress);
	void Convert(bool blocking);
	void GetResult(uint8_t* pResult);
	void SetPrecision(uint8_t precission);

	virtual ~Ds18B20() = default;

private:
	OneWire& rOneWire;
	uint8_t address[8];
};

} // namespace Drivers

#endif // DS18B20_H
