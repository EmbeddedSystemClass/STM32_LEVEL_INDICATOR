#include "channels.h"
#include "tablo.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private variables ---------------------------------------------------------*/
uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t Data = 0x3210ABCD;
uint32_t NbrOfPage = 0x00;
volatile TestStatus MemoryProgramStatus = PASSED;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
uint8_t FlashError=0;
//-----------------------------------
struct Channel  channels[CHANNEL_NUMBER];
extern struct tablo tab;//
//-----------------------------------
void ChannelsInit(void)
{

			channels[0].number=0;
			channels[0].settings.set.type=CHNL_ADC;
			channels[0].settings.set.modific=CHNL_ADC_FIX_16;
			channels[0].settings.set.state_byte_1=0x40;
			channels[0].settings.set.state_byte_2=0x06;
			channels[0].channel_data=0;
			channels[0].channel_data_calibrate=0;
//			channels[0].calibrate.cal.calibrate=0;

	return;
}

//-----------------------------------
void Store_Channels_Data(void) //
{
	unsigned char i=0,j=0;


	 FLASH_UnlockBank1();

	/* Define the number of page to be erased */
	NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	/* Erase the FLASH pages */
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
	}

	/* Program Flash Bank1 */
	Address = BANK1_WRITE_START_ADDR;

	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		channels[i].calibrate.cal.brightness=tab.indicators[i].brightness;
		for(j=0;j<2;j++)
		{
			FLASHStatus = FLASH_ProgramWord(Address, channels[i].calibrate.serialize[j]);
		    Address = Address + 4;
		}
	}
    FLASH_LockBank1();

    /* Check the corectness of written data */
    Address = BANK1_WRITE_START_ADDR;

    while((Address < BANK1_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
    {
        if((*(__IO uint32_t*) Address) != Data)
        {
            MemoryProgramStatus = FAILED;
        }
        Address += 4;
    }

    if( MemoryProgramStatus == FAILED)
    {
       // while(1);
    	//FlashError=1;
    }
	return;
}
//-----------------------------------
void Restore_Channels_Data(void) //using 0//
{
	unsigned char i=0,j=0;
	/* Program Flash Bank1 */
	Address = BANK1_WRITE_START_ADDR;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		for(j=0;j<2;j++)
		{
			channels[i].calibrate.serialize[j]=(*(__IO uint32_t*) Address);
			Address += 4;
		}
			if((channels[i].calibrate.cal.area<0)||(channels[i].calibrate.cal.area>999.9))
			{
				channels[i].calibrate.cal.area=0;
			}

			if((channels[i].calibrate.cal.offset<0)||(channels[i].calibrate.cal.offset>9999))
			{
				channels[i].calibrate.cal.offset=0;
			}

			if((channels[i].calibrate.cal.type_output>TYPE_OUTPUT_M3))
			{
				channels[i].calibrate.cal.type_output=TYPE_OUTPUT_LINEAR;
			}

			if((channels[i].calibrate.cal.brightness>0xF))
			{
				channels[i].calibrate.cal.brightness=0xF;
			}
			tab.indicators[i].brightness=channels[i].calibrate.cal.brightness;

	}
	return;
}
