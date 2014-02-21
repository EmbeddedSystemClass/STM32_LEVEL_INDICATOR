#ifndef SPI_BUS_H
#define SPI_BUS_H

#include "stm32f10x.h"
#include "indicator.h"

struct bus{//структура шины
	//struct indicator *indicators;//указатель на динамически выделяемую память свойств индикаторов
	uint8_t indicators_num;//количество индикаторов на шине
//	uint16_t **bus_buf;//динамически выделяемая память под буфер шины
	uint16_t bus_buf[IND_COMMAND_LEN][IND_SPI_BUS_1_NUM];
};

#define BUS_NUM	1//

#define BUS_SPI_1	0		//шина индикатора
#define BUS_SPI_2	1
#define BUS_SPI_3	2

uint8_t spi_buses_init(void);//инициализация шин SPI и выделение памяти под буферы

void	spi1_config(void);//конфигурация аппаратных интерфейсов
void	spi2_config(void);//
void 	spi3_config(void);//

void spi1_write_buf(uint16_t* pBuffer, uint16_t len);//перекинуть буфер через dma
void spi1_read_buf(uint16_t* pBuffer, uint16_t len);
void spi2_write_buf(uint16_t* pBuffer, uint16_t len);
void spi2_read_buf(uint16_t* pBuffer, uint16_t len);
void spi3_write_buf(uint16_t* pBuffer, uint16_t len);
void spi3_read_buf(uint16_t* pBuffer, uint16_t len);

#endif
