#include "main.h"
#include "dcmotor.h"
#include "button.h"
#include "led.h"

extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);

extern TIM_HandleTypeDef htim4;
extern volatile int TIM10_10ms_fan_rotate_direction_led;
extern volatile int TIM10_10ms_fan_auto_mode;

void dcmotor_processing(void);
void dcmotor_pwm_auto_control(void);
void dcmotor_pwm_manual_control(void);
void dcmotor_ledbar_control(uint16_t ChannelControlRegister_Value);
void FAN_ROTATE_DERECTION_LED_control(uint8_t forward_backwrd_dcmotor);
uint16_t Get_Current_ChannelControlRegister_Value(void);
void dcmotor_auto_mode_display(int auto_mode_remain_time);

//uint16_t ChannelControlRegister_Value = 0; // 현재 상태의 ChannelControlRegister 값을 담기 위한 변수
uint16_t ChannelControlRegister_UP_Value = 0; // PWM UP control // 16비트 타이머니까 uint16_t로 선언함
uint16_t ChannelControlRegister_DOWN_Value = 0; // PWM DOWN control

uint8_t pwm_start_flag = 0; // dcmotor start/stop indicator flag
uint8_t forward_backwrd_dcmotor = 0; // 정방향 0, 역방향 1 플래그
uint8_t break_dcmotor = 0; // no break상태 0, break 상태 1 플래그
uint8_t manual_auto_flag = 0; // 0은 manual mode, 1은 auto mode

int auto_mode_remain_time = 30; // auto mode


void dcmotor_processing(void)
{
//	if (manual_auto_flag == 0)
	if (manual_auto_flag == 1)
	{
		dcmotor_pwm_manual_control();
	}
	else
	{
		dcmotor_pwm_auto_control();
	}

//	if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == GPIO_PIN_RESET)
//	{
//		if (manual_auto_flag == 0)
//		{
//			manual_auto_flag = 1;
//		}
//		else
//		{
//			manual_auto_flag = 0;
//		}
//	}
}


void dcmotor_pwm_auto_control(void)
{
	dcmotor_auto_mode_display(auto_mode_remain_time); // 오토모드 디스플레이 출력

	// 1초마다 자동으로 남은시간 1초 감소
	if (TIM10_10ms_fan_auto_mode >= 100)
	{
		TIM10_10ms_fan_auto_mode = 0;
		auto_mode_remain_time--;

		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

		if (auto_mode_remain_time < 0)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			auto_mode_remain_time = 0;
		}
	}
	// BUTTON0 누르면 남은시간 10초 증가
	if (Get_Button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		auto_mode_remain_time += 10;
	}
	// BUTTON1 누르면 남은시간 10초 감소
	if (Get_Button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		auto_mode_remain_time -= 10;
		if (auto_mode_remain_time < 0) auto_mode_remain_time = 0; // 남은시간이 마이너스로 떨어지는거 방지
	}


	// BUTTON2 누르면 가속
	if (Get_Button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED1

		ChannelControlRegister_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		ChannelControlRegister_UP_Value += 10;

		if (ChannelControlRegister_UP_Value > 100) // ChannelControlRegister_UP_Value의 쓰레시 홀드(최대치 상한): 듀디사이클 최대 상한치는 100%(풀스피트)
		{
			ChannelControlRegister_UP_Value = 100;
		}

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ChannelControlRegister_UP_Value);

		dcmotor_ledbar_control(Get_Current_ChannelControlRegister_Value());
	}

	// BUTTON3 누르면 감속
	if (Get_Button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED1

		ChannelControlRegister_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		ChannelControlRegister_DOWN_Value -= 10;

		if (ChannelControlRegister_DOWN_Value < 60) // ChannelControlRegister_DOWN_Value의 쓰레시 홀드(최대치 하한): 듀디사이클 펄스 최대 하한치는 60%
		{
			ChannelControlRegister_DOWN_Value = 60;
		}

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ChannelControlRegister_DOWN_Value);

		dcmotor_ledbar_control(Get_Current_ChannelControlRegister_Value());
	}

}


void dcmotor_auto_mode_display(int auto_mode_remain_time)
{
	int auto_mode_remain_time_lv = auto_mode_remain_time; // 전역변수를 지역변수(lv: local variable)로 값 대피
	char lcd_buff_1[20], lcd_buff_2[20];



	sprintf(lcd_buff_1, "remain_time: %ds", auto_mode_remain_time_lv);
	move_cursor(0,0);
	lcd_string(lcd_buff_1);
//
//	sprintf(lcd_buff_1, "curr : %dcm", auto_mode_remain_time_lv);
//	move_cursor(1,0);
//	lcd_string(lcd_buff_2);
}


// BUTTON0: start/stop : 버튼이 눌리면 모터를 작동하거나 끄며 LED1 toggle
// BUTTON1: speed up : 버튼이 눌리면 모터를 가속하며 LED2 toggle
// BUTTON2: speed down : 버튼이 눌리면 모터를 감속하며 LED3 toggle
// BUTTON3: 정회전 역회전 반전
void dcmotor_pwm_manual_control(void)
{
	if (pwm_start_flag)
	{
		FAN_ROTATE_DERECTION_LED_control(forward_backwrd_dcmotor); // start 상태면, 정방향 역방향을 알리는 LED가 깜빡깜빡 거림
	}
	else
	{
		HAL_GPIO_WritePin(FAN_ROTATE_DERECTION_LED_GPIO_Port, FAN_ROTATE_DERECTION_LED_Pin, 0); // stop 상태면, 정방향 역방향을 알리는 LED가 꺼짐
	}

	// BUTTON0: start/stop : 버튼이 눌리면 모터를 작동하거나 끄며 LED1 toggle
	if (Get_Button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1

		if (!forward_backwrd_dcmotor) // 역방향 회전중이라면
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}


		if (!pwm_start_flag) // 앞에 not 있음.. pwm이 첫음 누른 상태라면..?
		{
			pwm_start_flag = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // START_it가 아니라 그냥 Start이다. START_it는 main()단에서 이미 했다.

			dcmotor_ledbar_control(Get_Current_ChannelControlRegister_Value()); // start상태가 되면 현재 클락 듀디 사이클에 맞게 ledbar가 켜지게 됨
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

			HAL_GPIO_WritePin(GPIOD, 0xff, 0); // stop상태가 되면 아예 ledbar가 꺼져있어야 함
		}

	}

	// BUTTON1: speed up : 버튼이 눌리면 모터를 가속하며 LED2 toggle
	if (Get_Button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED1

		ChannelControlRegister_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		ChannelControlRegister_UP_Value += 10;

		if (ChannelControlRegister_UP_Value > 100) // ChannelControlRegister_UP_Value의 쓰레시 홀드(최대치 상한): 듀디사이클 최대 상한치는 100%(풀스피트)
		{
			ChannelControlRegister_UP_Value = 100;
		}

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ChannelControlRegister_UP_Value);

		dcmotor_ledbar_control(Get_Current_ChannelControlRegister_Value());
	}

	// BUTTON2: speed down : 버튼이 눌리면 모터를 감속하며 LED3 toggle
	if (Get_Button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED1

		ChannelControlRegister_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		ChannelControlRegister_DOWN_Value -= 10;

		if (ChannelControlRegister_DOWN_Value < 60) // ChannelControlRegister_DOWN_Value의 쓰레시 홀드(최대치 하한): 듀디사이클 펄스 최대 하한치는 60%
		{
			ChannelControlRegister_DOWN_Value = 60;
		}

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ChannelControlRegister_DOWN_Value);

		dcmotor_ledbar_control(Get_Current_ChannelControlRegister_Value());
	}

	// BUTTON3: 정회전 역회전 반전
	if (Get_Button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		if (!forward_backwrd_dcmotor) // 역방향 회전중이라면
		{
			forward_backwrd_dcmotor = 1;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

			FAN_ROTATE_DERECTION_LED_control(forward_backwrd_dcmotor);
		}
		else
		{
			forward_backwrd_dcmotor = 0;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			FAN_ROTATE_DERECTION_LED_control(forward_backwrd_dcmotor);
		}

	}

}

uint16_t Get_Current_ChannelControlRegister_Value(void)
{
	uint16_t result;
	result = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
	return result;
}

void dcmotor_ledbar_control(uint16_t Current_ChannelControlRegister_Value)
{
	int ledbar_level;

	ledbar_level = (Current_ChannelControlRegister_Value - 44) / 8;

	HAL_GPIO_WritePin(GPIOD, 0xff, 0);

	for (int i = 0; i < ledbar_level; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x00|(0x80 >> i), 1);
	}
}

void FAN_ROTATE_DERECTION_LED_control(uint8_t forward_backwrd_dcmotor)
{
	if (!forward_backwrd_dcmotor) // 역방향 회전 중이라면
	{
		if (TIM10_10ms_fan_rotate_direction_led >= 5) // 50ms마다 점멸
		{
			TIM10_10ms_fan_rotate_direction_led = 0;
			HAL_GPIO_TogglePin(FAN_ROTATE_DERECTION_LED_GPIO_Port, FAN_ROTATE_DERECTION_LED_Pin);
		}
	}
	else // 정회전 중이라면
	{
		if (TIM10_10ms_fan_rotate_direction_led >= 10) // 100ms마다 점멸
		{
			TIM10_10ms_fan_rotate_direction_led = 0;
			HAL_GPIO_TogglePin(FAN_ROTATE_DERECTION_LED_GPIO_Port, FAN_ROTATE_DERECTION_LED_Pin);
		}
	}
}
