#ifndef CHANNELS_H
#define CHANNELS_H
#include <stm32f10x.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
//#include "stm32f10x_flash.h"
//#include "preferences.h"
#include "misc.h"
//---------------------------------------
#define CHANNEL_NUMBER	1//
//---------------------------------------
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define DESC_BANK1_WRITE_START_ADDR  ((uint32_t)0x08020400)
#define DESC_BANK1_WRITE_END_ADDR    ((uint32_t)0x08020800)


#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08020000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x08020400)

#ifdef STM32F10X_XL
  #define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
  #define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */

//---------------------------------------
#define CHNL_ADC	0//РєР°РЅР°Р» Р°С†Рї
	#define CHNL_ADC_FIX_16	0
	#define CHNL_ADC_FIX_16_LEN	6

	#define CHNL_ADC_SW_16	1
	#define CHNL_ADC_SW_16_LEN	6

	#define CHNL_ADC_FIX_24	3
	#define CHNL_ADC_FIX_24_LEN	7

	#define CHNL_ADC_SW_24	2
	#define CHNL_ADC_SW_24_LEN	7

#define CHNL_DOL	1 //РєР°РЅР°Р» РґРѕР»
	#define CHNL_DOL_ENC	0
	#define CHNL_DOL_ENC_LEN	7

	#define CHNL_DOL_FLOAT	1

#define CHNL_FREQ	2//С‡Р°СЃС‚РѕС‚РЅС‹Р№/СЃС‡РµС‚С‡РёРє
	#define	 CHNL_FREQ_COUNT_T	0
	#define	 CHNL_FREQ_COUNT_T_LEN	5

	#define	 CHNL_FREQ_256		1
	#define	 CHNL_FREQ_256_LEN		5

	#define  CHNL_FREQ_PERIOD	3
	#define  CHNL_FREQ_PERIOD_LEN	5

	#define	 CHNL_FREQ_COUNT	4
	#define	 CHNL_FREQ_COUNT_LEN	5

#define CHNL_DEV_STATE		4
	#define	 CHNL_DEV_STATE_GET		0
	#define	 CHNL_DEV_STATE_GET_LEN	3

	#define	 CHNL_DEV_STATE_SET		1
	#define	 CHNL_DEV_STATE_SET_LEN	3

	#define	 CHNL_DEV_STATE_GET_SET	2
	#define	 CHNL_DEV_STATE_GET_SET_LEN	3

#define CHNL_MEMORY		8
	#define CHNL_MEMORY_EEPROM	0
//---------------------------------------
struct Channel
{
	unsigned char number ;	  	//

	union
	{
		struct
		{
			unsigned char type;		  //
			unsigned char modific;	  //
			unsigned char state_byte_1;	//
			unsigned char state_byte_2;
		}set;

		unsigned long serialize;//
	} settings;

	unsigned long channel_data;		  // значение канала
    float  channel_data_calibrate;//калиброванное значение канала
	unsigned char string_buf[8];
	unsigned char string_mask[8];
	unsigned char blink;

	union
		{
			struct
			{
				unsigned long  adc_hi;
				float 		   cal_hi;

				unsigned long  adc_lo;
				float 		   cal_lo;

				float		   ust_hi;
				float		   ust_lo;

				uint16_t 	   offset;//offset in mm
				float 		   area;//
				uint8_t 	   type_output;// m3 or mm

				unsigned char calibrate;//
			} cal;
			unsigned long serialize[7];//
		} calibrate;

} ;
//---------------------------------------
void ChannelsInit(void);//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
//void Store_Channels_Data(void);//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅ
//void Restore_Channels_Data(void);//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅ

//---------------------------------------
#endif
