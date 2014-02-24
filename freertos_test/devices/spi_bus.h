#ifndef SPI_BUS_H
#define SPI_BUS_H

#include "stm32f10x.h"
#include "indicator.h"

struct bus{
	uint8_t indicators_num;//
	uint16_t bus_buf[IND_COMMAND_LEN][IND_SPI_BUS_1_NUM];
};

#define BUS_NUM	1//

#define BUS_SPI_1	0		//
#define BUS_SPI_2	1
#define BUS_SPI_3	2

uint8_t spi_buses_init(void);//

void	spi1_config(void);//


void spi1_write_buf(uint16_t* pBuffer, uint16_t len);//


#endif
