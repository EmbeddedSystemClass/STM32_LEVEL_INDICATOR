#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_dma.h"
#include <misc.h>

#include <stdio.h>
#include <math.h>

//FreeRTOS:
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "spi_bus.h"
#include "tablo.h"

#include "keyboard.h"
//#include "buzzer.h"
#include "adc_sensor.h"
#include "dac.h"

extern struct tablo tab;//
static void Init_Task(void *pvParameters);//

static void Init_Task(void *pvParameters)
{
	spi1_config();

	tablo_devices_init();
	DAC_Out_Init();
	ADC_Sensor_Init();
//	  vTaskDelay(2000);

//    Proto_Init();
//    keyboard_init();
//    buzzer_init();
    vTaskDelete( NULL );
}

int main(void)
{
	SystemInit();

	xTaskCreate(Init_Task,(signed char*)"INIT",128,NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();

    while(1);
}
