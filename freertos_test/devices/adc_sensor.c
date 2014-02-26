#include "adc_sensor.h"


#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_adc.h>
#include "misc.h"


#define ADC_CHN_1 ADC_Channel_4
#define ADC_CHN_2 ADC_Channel_5
#define ADC_CHN_3 ADC_Channel_7
#define ADC_CHN_4 ADC_Channel_8

#define ADC_PIN_1	GPIO_Pin_4
#define ADC_PIN_2	GPIO_Pin_5
#define ADC_PIN_3	GPIO_Pin_7
#define ADC_PIN_4	GPIO_Pin_8
#define ADC_GPIO	GPIOA

#define RCC_GPIO_ADC	RCC_APB2Periph_GPIOA


uint16_t ADC_Data_1,ADC_Data_2,ADC_Data_3,ADC_Data_4;

void ADC_Sensor_Init(void)//
{

	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef nvic;
	GPIO_InitTypeDef GPIO_InitStructure;
	//TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//TIM_OCInitTypeDef TIM_OCInitStructure;

	nvic.NVIC_IRQChannel = ADC1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

//-----------------------------------------------------------------------------
	/*тактирование нужных модулей*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  /*АЦП*/
	RCC_APB2PeriphClockCmd(RCC_GPIO_ADC, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//-----------------------------------------------------------------------------
	 /* Конфигурация ПВВ. PA*/
	 GPIO_InitStructure.GPIO_Pin = ADC_PIN_1 | ADC_PIN_2 | ADC_PIN_3|ADC_PIN_4;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(ADC_GPIO, &GPIO_InitStructure);
	/* ADC1 configuration ------------------------------------------------------*/
	 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //
	 ADC_InitStructure.ADC_ScanConvMode = ENABLE; //
	 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //
	 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//
	 ADC_InitStructure.ADC_NbrOfChannel = /*3*/0; //
	 ADC_Init(ADC1, &ADC_InitStructure);

//	 /* ADC1 regular channel14 configuration */
//	 ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_239Cycles5);
//	 ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_239Cycles5);
//	 ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_239Cycles5);

	 /* Set injected sequencer length */
	 ADC_InjectedSequencerLengthConfig(ADC1, 4);

	 /* ADC1 injected channel Configuration */
	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_1, 1, ADC_SampleTime_239Cycles5);
	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_2, 2, ADC_SampleTime_239Cycles5);
	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_3, 3, ADC_SampleTime_239Cycles5);
	 ADC_InjectedChannelConfig(ADC1, ADC_CHN_4, 4, ADC_SampleTime_239Cycles5);

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
	 ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // вкл. прерывание по окончанию преобразования
	 /* Start ADC1 calibaration */
	 ADC_StartCalibration(ADC1);
	 /* Check the end of ADC1 calibration */
	 while (ADC_GetCalibrationStatus(ADC1));

	 ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE); // программный запуск преобразования инжектированной группы
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE); // программный запуск преобразования регулярной группы
}

ADC1_IRQHandler(void)
{
	if (ADC_GetITStatus(ADC1, ADC_IT_JEOC))
	{
        ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
        ADC_Data_1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_Data_2 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
        ADC_Data_3 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3);
        ADC_Data_4 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4);
    }

	if (ADC_GetITStatus(ADC1, ADC_IT_EOC))
	{
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

    }

}

