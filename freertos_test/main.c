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
#include "buzzer.h"

extern struct tablo tab;//
static void Init_Task(void *pvParameters);//

static void Init_Task(void *pvParameters)
{
	uint8_t i=0, in_val;

	uint16_t buf_out[2]={0xFFFF,1234};
	uint16_t buf_in[1]={0x00};

	spi1_config();
//	spi2_config();
//	spi3_config();


//-------------------------------
//	while(1)
//	{
//        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//
//        /* Send SPI2 data */
//        SPI_I2S_SendData(SPI1, 0xAC);
//
//		/* Actual read */
//        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//
//        /* Read SPI2 received data */
//       in_val= SPI_I2S_ReceiveData(SPI1);
//	  	vTaskDelay(20);
//	}
//-------------------------------

//	spi1_read_buf(&buf_in,1);

//	spi1_write_buf(&buf_out,2);
//
//
//	 while(DMA_GetFlagStatus(DMA1_FLAG_TC3)==RESET)
//	 {
//		 taskYIELD();
//	 }
//	 DMA_Cmd(DMA1_Channel3, DISABLE);
//
//
//	 while(SPI1->SR & SPI_SR_BSY)
//	 {
//		 taskYIELD();
//	 }
//
//	vTaskDelay(20);
//
//	spi1_read_buf(&buf_in,1);
//
//	 while(DMA_GetFlagStatus(DMA1_FLAG_TC2)==RESET)
//	 {
//		 taskYIELD();
//	 }
//	 DMA_Cmd(DMA1_Channel2, DISABLE);
//
//
//	 while(SPI1->SR & SPI_SR_BSY)
//	 {
//
//	 }


	 tablo_devices_init();

//	tablo_proto_parser(&test_frame_1);

//	vTaskDelay(2000);

	//2 кадр
//	tablo_proto_parser(&test_frame_2);

	vTaskDelay(2000);

//    Proto_Init();
//    keyboard_init();
//    buzzer_init();
    vTaskDelete( NULL );

}

int main(void)
{
	SystemInit();


	 xTaskCreate(Init_Task,(signed char*)"INIT",128,NULL, tskIDLE_PRIORITY + 1, NULL);

	//spi1_config();
	//spi2_config();
////	spi3_config();
////
	//tablo_devices_init();
//
   // Proto_Init();



    vTaskStartScheduler();

    while(1);
}
