#ifndef TABLO_H
#define TABLO_H

#include "spi_bus.h"

struct tablo{//основная структура устройства
	struct bus buses[BUS_NUM];//создаем шиины табло
	uint8_t *uart_buf;//буфер UART


};

uint8_t tablo_devices_init(void);//инициализация буферов устройств табло

#endif