#include "adc_sensor.h"
#include <stdlib.h>
#include <string.h>


#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_adc.h>
#include "stm32f10x_dac.h"
#include "misc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tablo.h"
#include "indicator.h"
#include "channels.h"

#define ADC_CHN_1 ADC_Channel_0
#define ADC_CHN_2 ADC_Channel_1
#define ADC_CHN_3 ADC_Channel_2
#define ADC_CHN_4 ADC_Channel_3

#define ADC_PIN_1	GPIO_Pin_0
#define ADC_PIN_2	GPIO_Pin_1
#define ADC_PIN_3	GPIO_Pin_2
#define ADC_PIN_4	GPIO_Pin_3
#define ADC_GPIO	GPIOA

#define RCC_GPIO_ADC	RCC_APB2Periph_GPIOA

static void ADC_Poll_task(void *pvParameters);//
uint16_t ADC_Data_1;
extern struct tablo tab;//
extern struct Channel  channels[CHANNEL_NUMBER];

#define ADC_BUF_SIZE	32


xQueueHandle xADCQueue;

void ADC_Sensor_Init(void)//
{

	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef nvic;
	GPIO_InitTypeDef GPIO_InitStructure;


	nvic.NVIC_IRQChannel = ADC1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_GPIO_ADC, ENABLE);



	 GPIO_InitStructure.GPIO_Pin = ADC_PIN_1 /*| ADC_PIN_2 | ADC_PIN_3|ADC_PIN_4*/;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(ADC_GPIO, &GPIO_InitStructure);
	/* ADC1 configuration ------------------------------------------------------*/
	 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //
	 ADC_InitStructure.ADC_ScanConvMode = ENABLE; //
	 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //
	 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//
	 ADC_InitStructure.ADC_NbrOfChannel = /*3*/0; //
	 ADC_Init(ADC1, &ADC_InitStructure);



	 /* Set injected sequencer length */
	 ADC_InjectedSequencerLengthConfig(ADC1, 1);

	 /* ADC1 injected channel Configuration */
	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_1, 1, ADC_SampleTime_239Cycles5);
//	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_2, 2, ADC_SampleTime_239Cycles5);
//	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_3, 3, ADC_SampleTime_239Cycles5);
//	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_4, 4, ADC_SampleTime_239Cycles5);

	 /* ADC1 injected external trigger configuration */
	 ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);
	 ADC_AutoInjectedConvCmd(ADC1, ENABLE);

	 /* Enable ADC1 */
	 ADC_Cmd(ADC1, ENABLE);

	 /* Enable ADC1 reset calibaration register */
	 ADC_ResetCalibration(ADC1);
	 /* Check the end of ADC1 reset calibration register */
	 while (ADC_GetResetCalibrationStatus(ADC1));

	 /* Enable JEOC interupt */
	 ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE); //
	 /* Enable EOC interupt */
//	 ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); //
	 /* Start ADC1 calibaration */
	 ADC_StartCalibration(ADC1);
//	 /* Check the end of ADC1 calibration */
	 while (ADC_GetCalibrationStatus(ADC1));

//	 ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE); //
//	 ADC_SoftwareStartConvCmd(ADC1,ENABLE); //
	 xADCQueue = xQueueCreate( 1, sizeof( uint16_t ) );
	 xTaskCreate(ADC_Poll_task,(signed char*)"ADC_Poll_task",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

ADC1_IRQHandler(void)
{
	if (ADC_GetITStatus(ADC1, ADC_IT_JEOC))
	{
        ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
        ADC_Data_1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        xQueueSendToBackFromISR( xADCQueue, &ADC_Data_1, 0 );
    }

//	if (ADC_GetITStatus(ADC1, ADC_IT_EOC))
//	{
//        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//
//    }

}

void bubblesort(uint16_t *array, uint16_t length)
{
    uint16_t i, j;
    uint16_t tmp;
    for (i = 0; i < length -1; ++i)
    {

		for (j = 0; j < length - i - 1; ++j)
		   {
				if (array[j] > array[j + 1])
				{
				    tmp = array[j];
					array[j] = array[j + 1];
					array[j + 1] = tmp;
				}
		 }
    }
}

static void ADC_Poll_task(void *pvParameters)
{
	static uint32_t adc_accum=0;
	static uint8_t adc_counter=0;
	static uint16_t adc_buf[ADC_BUF_SIZE];
	static uint16_t temp_buf[ADC_BUF_SIZE];

	uint8_t i;
	while(1)
	{
	    vTaskDelay(5);
	    ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);

		while( uxQueueMessagesWaiting(xADCQueue) == 0 )//adc poll
		{
			taskYIELD();
		}
		xQueueReceive( xADCQueue, &adc_buf[adc_counter], 0 );
		adc_counter=(adc_counter+1)&(ADC_BUF_SIZE-1);

		memcpy(temp_buf,adc_buf,ADC_BUF_SIZE*sizeof(uint16_t));
		bubblesort(temp_buf, ADC_BUF_SIZE);

//		adc_accum=0;
//		for(i=0;i<ADC_BUF_SIZE;i++)
//		{
//			adc_accum+=adc_buf[i];
//		}
//
//		channels[0].channel_data=adc_accum/ADC_BUF_SIZE;
		channels[0].channel_data=(temp_buf[15]+temp_buf[16])/2;

		DAC_SetChannel1Data(DAC_Align_12b_R,(uint16_t)channels[0].channel_data);
		taskYIELD();
	}
}

