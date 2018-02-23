#include <cstdio>
#include <cstdint>
#include <math.h>
#include "stm32f10x.h"

#include "Serial.hpp"
#include "GpioDriver.hpp"
#include "C_Interface.hpp"
#include "PiesCieJebal.hpp"
//#include "one_wire.h"

Driver::ISerial* serial;

void setup_delay_timer(TIM_TypeDef *timer) {
    TIM_DeInit(timer);
    // Enable Timer clock
    if (timer == TIM2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    } else if (timer == TIM3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    } else if (timer == TIM4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    } else {
        // TODO: not implemented
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

extern void delayMicroseconds(uint16_t delay);

typedef struct {
    s8 integer;
    u16 fractional;
    bool is_valid;
} simple_float;

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

	printf("Stm32F103C8 Hello World\r\n");


	//one_wire_reset_pulse();
	setup_delay_timer(TIM2);
	one_wire_init(GPIOC, GPIO_Pin_13, TIM2);
	//printf("Reset %d\r\n", one_wire_reset_pulse());
	uint8_t tab[10], i=0;
    tab[i++] = one_wire_reset_pulse();
    one_wire_write_byte(0x33);
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();
    tab[i++] = one_wire_read_byte();

    for(i = 0; i < 9; i++)
    {
    	printf("%x\r\n", tab[i]);
    }

    uint8_t c;
    one_wire_device* temp1 = one_wire_search_rom(&c);
    printf("one wire search %d\r\n", c);

    for(int i = 0; i < 8; i++)
    {
        printf("%x ", temp1->address[i]);
    }
    printf("\r\n");

    for(int i = 0; i < 8; i++)
    {
        printf("%x ", (temp1 + 1)->address[i]);
    }
    printf("\r\n");

	one_wire_reset_pulse();
	one_wire_device wire1;
	wire1.address[0] = 0x28;
	wire1.address[1] = 0xec;
	wire1.address[2] = 0xdc;
	wire1.address[3] = 0xbb;
	wire1.address[4] = 0x05;
	wire1.address[5] = 0x00;
	wire1.address[6] = 0x00;
	wire1.address[7] = 0x05;
	one_wire_match_rom(wire1);
	one_wire_write_byte(0x44); // Convert temperature

	one_wire_device wire2;
	wire2.address[0] = 0x28;
	wire2.address[1] = 0xe6;
	wire2.address[2] = 0xe0;
	wire2.address[3] = 0xbb;
	wire2.address[4] = 0x05;
	wire2.address[5] = 0x00;
	wire2.address[6] = 0x00;
	wire2.address[7] = 0x6f;
	one_wire_match_rom(wire2);
	one_wire_write_byte(0x44); // Convert temperature

    one_wire_reset_pulse();
    one_wire_match_rom(wire1); // Skip ROM
    one_wire_write_byte(0xBE); // Read scratchpad

    {
		u8 crc;
		u8 data[9];
		one_wire_reset_crc();

		for (i = 0; i < 9; ++i) {
			data[i] = one_wire_read_byte();
			crc = one_wire_crc(data[i]);
		}
		if (crc != 0)
		{
			printf("crc!=0\r\n");
		}

		u8 temp_msb = data[1]; // Sign byte + lsbit
		u8 temp_lsb = data[0]; // Temp data plus lsb

		float temp = (temp_msb << 8 | temp_lsb) / powf(2, 4);
		int rest = (temp - (int)temp) * 1000.0;

		char buffer[10];
		sprintf(buffer, "%d.%d\r\n", (int)temp, rest);
		printf(buffer);
    }

	one_wire_reset_pulse();
	one_wire_match_rom(wire2); // Skip ROM
	one_wire_write_byte(0xBE); // Read scratchpad
    {
		u8 crc;
		u8 data[9];
		one_wire_reset_crc();

		for (i = 0; i < 9; ++i) {
			data[i] = one_wire_read_byte();
			crc = one_wire_crc(data[i]);
		}
		if (crc != 0)
		{
			printf("crc!=0\r\n");
		}

		u8 temp_msb = data[1]; // Sign byte + lsbit
		u8 temp_lsb = data[0]; // Temp data plus lsb

		float temp = (temp_msb << 8 | temp_lsb) / powf(2, 4);
		int rest = (temp - (int)temp) * 1000.0;

		char buffer[10];
		sprintf(buffer, "%d.%d\r\n", (int)temp, rest);
		printf(buffer);
    }

	while(1)
	{

	}
}

