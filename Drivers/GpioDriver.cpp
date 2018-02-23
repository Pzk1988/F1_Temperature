#include <GpioDriver.hpp>

namespace Drivers
{

GpioDriver::GpioDriver(GPIO_TypeDef* port, uint8_t pin) : port(port)
{
	this->pin = 1 << pin;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_InitTypeDef.GPIO_Pin = this->pin;
	gpio_InitTypeDef.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(port, &gpio_InitTypeDef);
}

void GpioDriver::SwitchToInput()
{
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(port, &gpio_InitTypeDef);
}

void GpioDriver::SwitchToOutput()
{
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(port, &gpio_InitTypeDef);
}

void GpioDriver::SetState(bool state)
{
	if(state == true)
	{
		port->BSRR |= pin;
	}
	else
	{
		port->BRR |= pin;
	}
}

bool GpioDriver::GetState()
{
	return port->IDR & pin;
}
} // namespace Drivers
