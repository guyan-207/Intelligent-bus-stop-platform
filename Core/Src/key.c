#include "gpio.h"
#include "My_Delay.h"

int Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin) == 0)
	{
		 my_delay_ms(20);
		 while(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin) == 0)
		 my_delay_ms(20);
		 KeyNum = 1;
	}
	if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,KEY_DOWN_Pin) == 0)
	{
		 my_delay_ms(20);
		 while(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,KEY_DOWN_Pin) == 0)
		 my_delay_ms(20);
		 KeyNum = 2;
	}
	if (HAL_GPIO_ReadPin(KEY_SET_GPIO_Port,KEY_SET_Pin) == 0)
	{
		 my_delay_ms(20);
		 while(HAL_GPIO_ReadPin(KEY_SET_GPIO_Port,KEY_SET_Pin) == 0)
		 my_delay_ms(20);
		 KeyNum = 3;
	}
	if (HAL_GPIO_ReadPin(KEY_EXTI_GPIO_Port,KEY_EXTI_Pin) == 0)
	{
		 my_delay_ms(20);
		 while(HAL_GPIO_ReadPin(KEY_EXTI_GPIO_Port,KEY_EXTI_Pin) == 0)
		 my_delay_ms(20);
		 KeyNum = 4;
	}
	
	return KeyNum;
}