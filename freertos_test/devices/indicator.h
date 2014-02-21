#ifndef INDICATOR_H
#define INDICATOR_H

#include "stm32f10x.h"

struct indicator{
    //uint16_t number;//Номер индикатора по протоколу
    uint16_t number_in_bus;//порядковый номер индикатора на шине
    uint16_t bus;//шина, на которой расположен индикатор
    uint16_t type;//тип индикатора
    uint16_t character_num;//количество знакомест 3-5
    uint16_t brightness;//яркость
    uint16_t decode_mode;//режим декодирования
    uint16_t scan_limit;//лимит знакомест
    uint16_t shutdown;//отключить индикаторы
    uint16_t display_test;//включить все сегменты
};

#define IND_BRIGHTNESS      0xA00 //0-15
#define IND_DECODE          0x900//
    #define IND_DECODE_ON       0xFF
    #define IND_DECODE_OFF      0x0
#define IND_SCAN_LIMIT      0xB00//0-8
#define IND_SHUTDOWN        0xC00
    #define IND_SHUTDOWN_ON     0x0
    #define IND_SHUTDOWN_OFF    0x1
#define IND_DISPLAY_TEST    0xF00
    #define IND_DISPLAY_TEST_ON     0x1
    #define IND_DISPLAY_TEST_OFF    0x0
/*
Типы индикаторов
  */
#define IND_TYPE_NONE       0x0//отсутствует
#define IND_TYPE_SEGMENT    0x1//Сегментный
#define IND_TYPE_LINE       0x2//Линейный
#define IND_TYPE_ARC        0x3//Дуга?

#define IND_COMMAND_LEN		13//длина командного буфера для одного индикатора

#define IND_ALL_NUM			1//общее количество индикаторов

#define IND_SPI_BUS_1_NUM	1 //количество индикаторов на шине
#define IND_SPI_BUS_2_NUM	0
#define IND_SPI_BUS_3_NUM	0

uint8_t indicators_init(void);//инициализация и выделение памяти под индикаторы

#endif
