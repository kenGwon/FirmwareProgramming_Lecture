#include "button.h"
#include "led.h"

extern void led_all_on(void);
extern void led_all_off(void);

void button_main(void);

void button_main(void)
{
	while(1)
	{
		if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == 1)
		{
			led_all_off();
		}
		if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == 0)
		{
			led_all_on();
		}
	}
}

