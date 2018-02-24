#include "TemperatureCard.hpp"
#include "Configuration.hpp"
#include <stddef.h>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include "C_Interface.hpp"
#include <cstring>
#include "Crc.hpp"

TemperatureCard::TemperatureCard()
{
	Drivers::OneWire* pOneWire = new Drivers::OneWire(GPIOC, GPIO_Pin_13, TIM3);

	memset(result, 0, sizeof(result));
	memset(floatResult, 0, sizeof(floatResult));
	memset(finalResult, 0, sizeof(finalResult));
	memset(resultBank, false, sizeof(resultBank));

	for(size_t i = 0; i < Configuration::GetTempSendorAmount(); i++)
	{
		temperatureSensors.push_back(new Drivers::Ds18B20(*pOneWire, Configuration::GetSensorList()[i].address));
	}
}

void TemperatureCard::Process()
{
	if( conversionState == false )
	{
		Convert();
		conversionState = true;
		conversionStart = GetClockTick();
	}
	else if( GetClockTick() - conversionStart > 1500 )
	{
		ReadResult();
		conversionState = false;
		UpdateAverageValue();
		UpdateChangeFlag();
	}
}

void TemperatureCard::Convert()
{
	std::for_each(temperatureSensors.begin(), temperatureSensors.end(), [](Drivers::Ds18B20* sensor)
	{
		sensor->Convert(false);
	});
}

void TemperatureCard::ReadResult()
{
	uint8_t i = 0;
	std::for_each(temperatureSensors.begin(), temperatureSensors.end(), [this, &i](Drivers::Ds18B20* sensor)
	{
		uint8_t tmp[9];
		sensor->GetResult(tmp);

		if(0x00 == Crc::Crc8(tmp, 9))
		{
			result[i] = tmp[0];
			result[i + 1] = tmp[1];
		}
		i += 2;
	});
}

void TemperatureCard::UpdateAverageValue()
{
	for(size_t i = 0; i < 16; i++)
	{
		uint16_t temp_digital = (result[(i * 2) + 1] << 8) | result[i * 2];
		float temp_c;

		if (temp_digital >= 0x800) //temperture is negative
		{
			temp_c = 0;
			//calculate the fractional part
			if(temp_digital & 0x0001) temp_c += 0.06250;
			if(temp_digital & 0x0002) temp_c += 0.12500;
			if(temp_digital & 0x0004) temp_c += 0.25000;
			if(temp_digital & 0x0008) temp_c += 0.50000;

			//calculate the whole number part
			temp_digital = (temp_digital >> 4) & 0x00FF;
			temp_digital = temp_digital - 0x0001; //subtract 1
			temp_digital = ~temp_digital; //ones compliment
			temp_c = temp_c - (float)(temp_digital & 0xFF);
		}
		else //temperture is positive
		{
			temp_c = 0;
			//calculate the whole number part
			temp_c = (temp_digital >> 4) & 0x00FF;
			//calculate the fractional part
			if(temp_digital & 0x0001) temp_c = temp_c + 0.06250;
			if(temp_digital & 0x0002) temp_c = temp_c + 0.12500;
			if(temp_digital & 0x0004) temp_c = temp_c + 0.25000;
			if(temp_digital & 0x0008) temp_c = temp_c + 0.50000;
		} //end if else


		floatResult[i][averageCounter] = temp_c * 10;
	}
	if(averageCounter == 2){
		averageCounter = 0;
	} else {
		averageCounter++; }
}

void TemperatureCard::UpdateChangeFlag()
{
	for(size_t i = 0; i < 16; i++)
	{
		int16_t tmp = (floatResult[i][0] + floatResult[i][1] + floatResult[i][2]) / 3;

		if(tmp != finalResult[i])
		{
			finalResult[i] = tmp;
			resultBank[i / 4] = true;
//			printf("New temp %d, sensor %d bank %d\r\n", tmp, i, i / 4);
		}
	}
}

bool TemperatureCard::StateChanged(uint8_t sensorBank)
{
	return resultBank[sensorBank];
}

void TemperatureCard::ResetStateChanged(uint8_t sensorBank)
{
	resultBank[sensorBank] = false;
}

uint8_t* TemperatureCard::GetBank(uint8_t sensorBank)
{
	return reinterpret_cast<uint8_t*>(&finalResult[sensorBank * 4]);
}
