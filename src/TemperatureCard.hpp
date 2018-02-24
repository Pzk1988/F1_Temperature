#ifndef TEMPERATURECARD_H
#define TEMPERATURECARD_H
#include <cstdint>
#include <Ds18B20.hpp>
#include <vector>

class TemperatureCard
{
public:
	TemperatureCard();
	virtual ~TemperatureCard() = default;
	void Process();
	bool StateChanged(uint8_t sensorBank);
	void ResetStateChanged(uint8_t sensorBank);
	uint8_t* GetBank(uint8_t sensorBank);

private:
	void DisplayTemp(uint8_t* pData);
	void Convert();
	void ReadResult();
	void UpdateAverageValue();
	void UpdateChangeFlag();

	std::vector<Drivers::Ds18B20*> temperatureSensors;
	uint8_t result[32];
	int16_t floatResult[16][3];
	int16_t finalResult[16];
	bool resultBank[4];
	uint32_t conversionStart = 0;
	bool conversionState = false;
	uint8_t averageCounter = 0;

};

#endif // TEMPERATURECARD_H
