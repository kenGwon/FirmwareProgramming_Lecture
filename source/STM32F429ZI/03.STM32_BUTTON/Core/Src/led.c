#include "led.h"
#include "button.h"

void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_up(void);
void led_on_down(void);
void led_keepon_up(void);
void led_keepon_down(void);
void led_flower_on(void);
void led_flower_off(void);

void button0_led_all_on_off_toggle(void);
void button0_toggle(void);

void led_main(void)
{
	while(1)
	{
		// phase#6
		button0_toggle();

		// phase#5
//		led_flower_on();
//		led_flower_off();

		// phase#4
//		led_keepon_up();
//		led_all_off();
//		led_keepon_down();
//		led_all_off();

		// phase#3
//		led_on_up();
//		led_on_down();

		// phase#2
//		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_ALL);
//		HAL_Delay(500);

		// phase#1
//		led_all_on();
//		HAL_Delay(300); // 아직 우리가 timer를 배우지 않았기 때문에, 그냥 HAL을 Sleep시키는 방법으로 딜레이를 준다.
//		led_all_off();
//		HAL_Delay(300);
	}
}

void led_all_on(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
	HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void led_all_off(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//				GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

void led_on_up(void)
{
	for (int i = 0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

void led_on_down(void)
{
	for (int i = 0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}

void led_keepon_up(void)
{
	led_all_off();
	for (int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

void led_keepon_down(void)
{
	led_all_off();
	for (int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}

void led_flower_on(void)
{
	int i, delay = 100;

	led_all_off();

	for (i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << (4 + i), 1);
		HAL_GPIO_WritePin(GPIOD, 0x01 << (3 - i), 1);
		HAL_Delay(delay);
		delay += 100;
	}
}

void led_flower_off(void)
{
	int i, delay = 400;

	led_all_on();

	for (i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << (0 + i), 0);
		HAL_GPIO_WritePin(GPIOD, 0x01 << (7 - i), 0);
		HAL_Delay(delay);
		delay -= 100;
	}

}

void led_gradation(void)
{
	uint16_t temp[8] = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080};
	int i;

	i = 0;
	while(1)
	{
		HAL_GPIO_WritePin(GPIOD, temp[i], 1);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOD, temp[i], 0);
		i++;
		if (i == 8)
		{
			i = 0;
		}
	}

}

void button0_toggle(void)
{
	static int button0_count = 0;
	void(*fp[])(void) = {
			led_all_on, led_all_off, led_keepon_up, led_keepon_down,
			led_flower_on, led_flower_off, led_on_up, led_on_down
	};

	if (Get_Button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 8;
	}

	fp[button0_count]();
}

