#include "spi_bus.h"

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

#include "tablo.h"
#include "indicator.h"

extern struct tablo tab;//

static void spi1_task(void *pvParameters);//

xSemaphoreHandle xSPI_Buf_Mutex;

uint8_t spi_buses_init(void)//
{
	 uint8_t error=0;
	 xTaskCreate(spi1_task,(signed char*)"SPI_1_TASK",64,NULL, tskIDLE_PRIORITY + 1, NULL);

	 xSPI_Buf_Mutex=xSemaphoreCreateMutex();

	 if( xSPI_Buf_Mutex != NULL )
	 {
		//
	 }

	 //test
	 tab.indicators[0].decimal_point=3;
	 indicators_set_num(&tab.indicators[0],9.312);
	return error;
}

void	spi1_config(void)//
{
//	uint32_t i=0;
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);

	    GPIO_InitTypeDef GPIO_InitStructure;
	    SPI_InitTypeDef SPI_InitStructure;

	    /* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 |GPIO_Pin_7;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    // MISO:
	 /* GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);*/

	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	    //SPI_InitStructure.SPI_CRCPolynomial = 7;
	    SPI_Init(SPI1, &SPI_InitStructure);



	    /* Enable SPI1 */
	    SPI_CalculateCRC(SPI1, DISABLE);
	   // SPI_SSOutputCmd(SPI1, ENABLE);
	    SPI_Cmd(SPI1, ENABLE);

	    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
	//--------------------------------------------------------------------------
		DMA_InitTypeDef DMA_InitStructure;
	  //  NVIC_InitTypeDef NVIC_InitStructure;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//---------------------------
		DMA_StructInit(&DMA_InitStructure);//mosi

		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&SPI1->DR);
		//DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txBuffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		//DMA_InitStructure.DMA_BufferSize = INDICATORS_NUM;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);


		DMA_ClearFlag(DMA1_FLAG_TC3);
//-----------------------------
		DMA_StructInit(&DMA_InitStructure);//miso

		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&SPI1->DR);
		//DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txBuffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		//DMA_InitStructure.DMA_BufferSize = INDICATORS_NUM;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel2, &DMA_InitStructure);


		DMA_ClearFlag(DMA1_FLAG_TC2);
//-----------------------------
		// Enable DMA request
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
}

void spi1_write_buf(uint16_t* pBuffer, uint16_t len)//перекинуть буфер через dma
{
	  DMA1_Channel3->CMAR = (uint32_t)pBuffer;
	  DMA1_Channel3->CNDTR = len;

	  DMA_ClearFlag(DMA1_FLAG_TC3);

	  DMA_Cmd(DMA1_Channel3, ENABLE);
}

//void spi1_read_buf(uint16_t* pBuffer, uint16_t len)//перекинуть буфер через dma
//{
//	  DMA1_Channel2->CMAR = (uint32_t)pBuffer;
//	  DMA1_Channel2->CNDTR = len;
//
//	  DMA_ClearFlag(DMA1_FLAG_TC2);
//
//	  DMA_Cmd(DMA1_Channel2, ENABLE);
//}

static void spi1_task(void *pvParameters)//
{
	uint8_t i=0;
	while(1)
	{
		for(i=0;i<IND_COMMAND_LEN;i++)
		{
			GPIO_WriteBit(GPIOA, GPIO_Pin_8,0);

			if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
			{
				 spi1_write_buf(&tab.buses[BUS_SPI_1].bus_buf[i][0],IND_SPI_BUS_1_NUM);

				 while(DMA_GetFlagStatus(DMA1_FLAG_TC3)==RESET)
				 {
					 taskYIELD();
				 }
				 DMA_Cmd(DMA1_Channel3, DISABLE);


				 while(SPI1->SR & SPI_SR_BSY)
				 {
					 taskYIELD();
				 }

				 xSemaphoreGive( xSPI_Buf_Mutex );
			}

			GPIO_WriteBit(GPIOA, GPIO_Pin_8,1);

			GPIO_WriteBit(GPIOA, GPIO_Pin_8,0);
		}
		vTaskDelay(50);
	}
}


