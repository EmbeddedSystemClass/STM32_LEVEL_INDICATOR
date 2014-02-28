#include "adc_sensor.h"


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
	 ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE); // вкл. прерывание по окончанию преобразования инжектированной группы
	 /* Enable EOC interupt */
//	 ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // вкл. прерывание по окончанию преобразования
	 /* Start ADC1 calibaration */
	 ADC_StartCalibration(ADC1);
//	 /* Check the end of ADC1 calibration */
	 while (ADC_GetCalibrationStatus(ADC1));

//	 ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE); // программный запуск преобразования инжектированной группы
//	 ADC_SoftwareStartConvCmd(ADC1,ENABLE); // программный запуск преобразования регулярной группы
	 xTaskCreate(ADC_Poll_task,(signed char*)"ADC_Poll_task",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

uint8_t adc_ready=0;
ADC1_IRQHandler(void)
{
	if (ADC_GetITStatus(ADC1, ADC_IT_JEOC))
	{
        ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
        ADC_Data_1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
//        ADC_Data_2 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
//        ADC_Data_3 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3);
//        ADC_Data_4 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4);
        adc_ready=1;
    }

//	if (ADC_GetITStatus(ADC1, ADC_IT_EOC))
//	{
//        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//
//    }

}

static void ADC_Poll_task(void *pvParameters)
{
	static uint32_t adc_accum=0;
	uint8_t i=0;
	while(1)
	{
		adc_accum=0;
		for(i=0;i<32;i++)
		{
			vTaskDelay(5);
			ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
			 while(!adc_ready)
			 {
				 taskYIELD();
			 }
			 adc_ready=0;
			 adc_accum+=ADC_Data_1;
		}
//		vTaskDelay(50);
		channels[0].channel_data=adc_accum/32;

		DAC_SetChannel1Data(DAC_Align_12b_R,ADC_Data_1);
	}
}

