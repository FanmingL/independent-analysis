#include <led.h>
static GPIO_Handle hGpio;
#define LED_GPIO GPIO_PIN13

void led_configuration(void)
{
	hGpio = GPIO_open(GPIO_DEV0,GPIO_OPEN_RESET);
	GPIO_reset(hGpio);
	GPIO_pinEnable(hGpio,LED_GPIO);
	GPIO_pinDirection(hGpio,LED_GPIO,GPIO_OUTPUT);
}

void led_on(void)
{
	GPIO_pinWrite(hGpio,LED_GPIO,0);
}

void led_off(void)
{
	GPIO_pinWrite(hGpio,LED_GPIO,1);
}

void led_toggle(void)
{
	GPIO_pinWrite(hGpio,LED_GPIO,1 - GPIO_pinRead(hGpio, LED_GPIO));
}
