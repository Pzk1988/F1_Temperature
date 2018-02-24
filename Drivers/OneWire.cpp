#include <OneWire.hpp>
#include <stddef.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

namespace Drivers
{

const unsigned char OneWire::crc_table[256] =
{
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

OneWire::OneWire(GPIO_TypeDef* port, uint16_t pin, TIM_TypeDef* timer) : port(port), pin(pin), timer(timer)
{
	// Configur I/O pin
    if (port == GPIOA)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }
    else if (port == GPIOB)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
    else if (port == GPIOC)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    else
    {
    	while(1){}
    }

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStructure);

    // Timer
    TIM_DeInit(timer);
    // Enable Timer clock
    if (timer == TIM2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    }
    else if (timer == TIM3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    }
    else if (timer == TIM4)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    }
    else
    {
    	while(1){}
    }

    // Configure timer
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us (10 ms)
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(timer, &TIM_InitStructure);

    TIM_Cmd(timer, ENABLE);
}

bool OneWire::ResetPulse()
{
    // Pull bus down for 500 us (min. 480 us)
	bool retVal = false;

    GPIO_ResetBits(port, pin);
    DelayUs(500);
    GPIO_SetBits(port, pin);

    // Wait 70 us, bus should be pulled up by resistor and then
    // pulled down by slave (15-60 us after detecting rising edge)
    DelayUs(70);
    BitAction bit = (BitAction)GPIO_ReadInputDataBit(port, pin);
    if (bit == Bit_RESET)
    {
    	retVal = true;
    }

    // Wait additional 430 us until slave keeps bus down (total 500 us, min. 480 us)
    DelayUs(430);
    return retVal;
}

void OneWire::WriteBit(bool bit)
{
	if( bit == true)
	{
		WriteOne();
	}
	else
	{
		WriteZero();
	}
}

bool OneWire::ReadBit()
{
    // Pull bus down for 5 us
    GPIO_ResetBits(port, pin);
    DelayUs(5);
    GPIO_SetBits(port, pin);

    // Wait 5 us and check bus state
    DelayUs(5);

    static BitAction bit;
    bit = (BitAction)GPIO_ReadInputDataBit(port, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    DelayUs(55);

    if (bit == Bit_SET)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void OneWire::WriteByte(uint8_t data)
{
	uint8_t i;
	for (i = 0; i < 8; ++i)
	{
		if ((data >> i) & 1)
		{
			WriteOne();
		}
		else
		{
			WriteZero();
		}
	}
}

uint8_t OneWire::ReadByte()
{
    uint8_t i;
    uint8_t data = 0;
    bool bit;
    for (i = 0; i < 8; ++i)
    {
        bit = ReadBit();
        data |= bit << i;
    }
    return data;
}

void OneWire::ReadRom(uint8_t* pData)
{

}

uint8_t OneWire::ValidateCrc(uint8_t data)
{
    crc8 = crc_table[crc8 ^ data];
    return crc8;
}

int OneWire::Search()
{
    uint8_t id_bit_number;
    int last_zero, rom_byte_number, search_result;
    bool id_bit, cmp_id_bit;
    uint8_t rom_byte_mask, search_direction;

    // initialize for search
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = 0;
    crc8 = 0;
    // if the last call was not the last one
    if (!lastDeviceFlag)
    {
        // 1-Wire reset
        if (!ResetPulse()) {
            // reset the search
        	lastMismatch = 0;
        	lastDeviceFlag = false;
            return false;
        }
        // issue the search command
        WriteByte(0xF0);
        // loop to do the search
        do
        {
            // read a bit and its complement
            id_bit = ReadBit();
            cmp_id_bit = ReadBit();
            // check for no devices on 1-wire
            if ((id_bit == 1) && (cmp_id_bit == 1))
                break; // no devices

            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
                search_direction = id_bit; // bit write value for search
            else
            {
                // if this discrepancy if before the Last Discrepancy
                // on a previous next then pick the same as last time
                if (id_bit_number < lastMismatch)
                    search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
                else
                    // if equal to last pick 1, if not then pick 0
                    search_direction = (id_bit_number == lastMismatch);
                // if 0 was picked then record its position in LastZero
                if (search_direction == 0)
                {
                    last_zero = id_bit_number;
                }
            }
            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
                ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
                ROM_NO[rom_byte_number] &= ~rom_byte_mask;
            // serial number search direction write bit
            WriteBit(search_direction);
            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;
            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                rom_byte_number++;
                rom_byte_mask = 1;
            }

        }
        while(rom_byte_number < 8); // loop until through all ROM bytes 0-7
        // if the search was successful then
        if (!((id_bit_number < 65) || (crc8 != 0)))
        {
            // search successful so set last_mismatch, last_device_flag, search_result
        	lastMismatch = last_zero;
            // check for last device
            if (lastMismatch == 0)
            	lastDeviceFlag = true;

            search_result = true;
        }
    }
    // if no device found then reset counters so next 'search' will be like a first
    if (!search_result || !ROM_NO[0])
    {
    	lastMismatch = 0;
    	lastDeviceFlag = false;
        search_result = false;
    }
    return search_result;
}

int OneWire::Search(ONE_WIRE_ADDR* pOneWireData)
{
    int result;
    uint8_t oneWireDeviceAmount = 0;

    lastMismatch = 0;
    lastDeviceFlag = false;
    result = Search();

    while (result)
    {
    	for(size_t i = 0; i < 8; i++)
    	{
        	pOneWireData[oneWireDeviceAmount].address[i] = ROM_NO[i];
    	}
    	oneWireDeviceAmount++;
        result = Search();
    }

    return oneWireDeviceAmount;
}

bool OneWire::MatchRom(uint8_t* pAddress)
{
    int i;
    ResetPulse();
    WriteByte(0x55);

    for (i = 0; i < 8; ++i)
    {
        WriteByte(pAddress[i]);
    }

    return false;
}

void OneWire::DelayUs(uint32_t time)
{
    timer->CNT = 0;
    time -= 3;
    while (timer->CNT <= time) {}
}

void OneWire::WriteOne()
{
    // Pull bus down for 15 us
    GPIO_ResetBits(port, pin);
    DelayUs(15);
    GPIO_SetBits(port, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    DelayUs(50);
}

void OneWire::WriteZero()
{
	// Pull bus down for 60 us
    GPIO_ResetBits(port, pin);
    DelayUs(60);
    GPIO_SetBits(port, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    DelayUs(5);
}

} // namespace Drivers
