#include <cstdio>
#include <cstdint>
#include <math.h>
#include "stm32f10x.h"

#include "Serial.hpp"
#include "GpioDriver.hpp"
#include "C_Interface.hpp"
#include "OneWire.hpp"
#include "Ds18B20.hpp"
#include "Configuration.hpp"
#include "ICommunication.hpp"
#include "Can.hpp"
#include "TemperatureCard.hpp"

Driver::ISerial* serial;
Driver::ICommunication* commDriver;
volatile uint8_t receivedSem = 0;
volatile CanRxMsg RxMessage;

static void ProcessCan(TemperatureCard& temperatureCard);

int main()
{
	// System clock
	if( SysTick_Config(SystemCoreClock / 1000) != 0)
	{
		printf("SysTick_config fail\r\n");
	}

	// Serial driver
	serial = new Driver::Serial();
	serial->Init(115200);

	// Can driver
	commDriver = new Driver::Can(Configuration::GetId());
	commDriver->Init(Configuration::GetId());

	// TempCard
	TemperatureCard* tempCard = new TemperatureCard();

	printf("Stm32F103C8 Hello World\r\n");

	{
		while(1)
		{
			// Process temperature sensors
			tempCard->Process();

			// Send temperature values if changed
			for(size_t i = 0; i < 4; i++)
			{
				if(tempCard->StateChanged(i) == true)
				{
					tempCard->ResetStateChanged(i);
					commDriver->SendDataFrame(0x01, tempCard->GetBank(i), 8);
					printf("Sending %d: ", i);
					for(int j = 0; j < 8; j++)
					{
						printf("%x ", tempCard->GetBank(i)[j]);
					}
					printf("\r\n");
				}
			}

			//Process can
			ProcessCan(*tempCard);
		}
	}
}

void ProcessCan(TemperatureCard& temperatureCard)
{
	if(receivedSem == 1)
	{
		receivedSem = 0;
		if(RxMessage.RTR == CAN_RTR_Remote)
		{
//			uint16_t input = temperatureCard.GetState();
//			commDriver->SendDataFrame(0x01, (uint8_t*)&input, 2);
			printf("RTR id 0x%x from 0x%x\r\n", (RxMessage.StdId & 0x1f), (RxMessage.StdId >> 5));
		}
		else
		{
			printf("Can 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n", RxMessage.StdId, ((RxMessage.Data[0]) | RxMessage.Data[1] << 8),
																			((RxMessage.Data[2]) | RxMessage.Data[3] << 8),
																			((RxMessage.Data[4]) | RxMessage.Data[5] << 8),
																			((RxMessage.Data[6]) | RxMessage.Data[7] << 8));
		}
	}
}

// TIM3
//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//TIM_TimeBaseInitTypeDef TIM_InitStructure;
//TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//TIM_InitStructure.TIM_Prescaler = 36000 - 1;
//TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us (10 ms)
//TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//TIM_InitStructure.TIM_RepetitionCounter = 0;
//TIM_TimeBaseInit(TIM3, &TIM_InitStructure);
//TIM_Cmd(TIM3, ENABLE);
//
//// One wire driver
//Drivers::OneWire oneWire(GPIOC, GPIO_Pin_13, TIM2);
//
//uint16_t startTime[2];
//uint16_t endTime[2];
//uint8_t rom[2][9];
//uint8_t devicesAmount = 2;
//
//Drivers::Ds18B20 tempOkno(oneWire, Configuration::GetInputList()[0].address);
//tempOkno.SetPrecision(0);
//Drivers::Ds18B20 tempBiurko(oneWire, Configuration::GetInputList()[1].address);
//
//startTime[0] = TIM3->CNT;
//tempOkno.Convert(true);
//endTime[0] = TIM3->CNT;
//
//startTime[1] = TIM3->CNT;
//tempBiurko.Convert(true);
//endTime[1] = TIM3->CNT;
//
//tempOkno.GetResult(rom[0]);
//tempBiurko.GetResult(rom[1]);
//
//for(size_t i = 0; i < devicesAmount; i++)
//{
//	for(size_t j = 0; j < 9; j++)
//	{
//		printf("%x ", rom[i][j]);
//	}
//	printf("\r\n");
//
//	float temp = (rom[i][1] << 8 | rom[i][0]) / powf(2, 4);
//	int rest = (temp - (int)temp) * 1000.0;
//
//	char buffer[10];
//	sprintf(buffer, "%d.%d\r\n", (int)temp, rest);
//	printf(buffer);
//}
////
//printf("Time conversion 1: %u, 2: %u, %u\r\n", startTime[0], endTime[0], endTime[0]-startTime[0]);
//printf("Time conversion 1: %u, 2: %u, %u\r\n", startTime[1], endTime[1], endTime[1]-startTime[1]);

//typedef struct {
//    s8 integer;
//    u16 fractional;
//    bool is_valid;
//} simple_float;
