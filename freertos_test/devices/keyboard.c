#include "keyboard.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_dma.h"
#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "menu.h"

uint8_t keyboard_poll(void);

void keyboard_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure port -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    GPIO_InitStructure.GPIO_Pin   = KEY_Y1|KEY_Y2|KEY_Y3;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
//    GPIO_Init(PORT_Y, &GPIO_InitStructure);

//    GPIO_WriteBit(PORT_X, KEY_X1,0);
//    GPIO_WriteBit(PORT_X, KEY_X2,0);
//    GPIO_WriteBit(PORT_X, KEY_X3,0);
//
//    GPIO_WriteBit(PORT_Y, KEY_Y1,0);
//    GPIO_WriteBit(PORT_Y, KEY_Y2,0);
//    GPIO_WriteBit(PORT_Y, KEY_Y3,0);

    xTaskCreate(keyboard_task,(signed char*)"KEYBOARD",64,NULL, tskIDLE_PRIORITY + 1, NULL);
}
void keyboard_task(void *pvParameters )
{
   uint8_t key_1, key_2, last_key=0,key_code;
	while(1)
	{
		vTaskDelay(10);
		key_1= keyboard_poll();
		vTaskDelay(10);
		key_2= keyboard_poll();

		if((key_1==key_2)&&(key_1!=last_key)/*&&(key_1!=0x0)*/)
		{
			last_key=key_1;
			key_code=key_1;

			if(key_1!=0)
			{
				menuKey(key_code);
			}
		}
	}
}

uint8_t keyboard_poll(void)
{
	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10))
	{
		return 'E';
	}

	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11))
	{
		return 'Q';
	}

	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12))
	{
		return '>';
	}

	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13))
	{
		return '+';
	}

	 return 0;
}
