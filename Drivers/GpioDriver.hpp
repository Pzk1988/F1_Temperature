#ifndef GPIODRIVER_H
#define GPIODRIVER_H
#include "stm32f10x_gpio.h"

namespace Drivers
{

class GpioDriver
{
public:
	GpioDriver(GPIO_TypeDef* Port, uint8_t pin);
	virtual ~GpioDriver() = default;
	void SwitchToInput();
	void SwitchToOutput();
	void SetState(bool state);
	bool GetState();

private:
	GPIO_InitTypeDef gpio_InitTypeDef;
	GPIO_TypeDef* port;
	uint16_t pin;
};

} // namespace Drivers

#endif // GPIODRIVER_H
