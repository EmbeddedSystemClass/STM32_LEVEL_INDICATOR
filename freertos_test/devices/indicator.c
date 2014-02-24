#include "indicator.h"
#include "tablo.h"

#define SYM_TAB_LEN 31
const uint8_t Sym_table[2][SYM_TAB_LEN]={{'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F','h','I','I','J','L','O','P','r','t','U','u','.','-','_',' '},
                                         {0x7E/*0*/,0x30/*1*/,0x6D/*2*/,0x79/*3*/,0x33/*4*/,0x5B/*5*/,0x5F/*6*/,0x70/*7*/,0x7F/*8*/,0x7B/*9*/,0x77/*A*/,
                                          0x1F/*b*/,0x4E/*C*/,0x3D/*d*/,0x4F/*E*/,0x47/*F*/,0x17/*h*/,0x30/*I*/,0x10/*i*/,0x3C/*J*/,0xE/*L*/,0x7E/*O*/,
                                          0x67/*P*/,0x05/*r*/,0x0F/*t*/,0x3E/*U*/,0x1C/*u*/,0x80/*.*/,0x01/*-*/,0x08/*_*/,0x00/* */}};


extern struct tablo tab;

uint8_t indicators_init(void)//
{
	uint8_t error=0;


	return error;
}

void indicators_set_num(struct indicator *ind,float val)
{
	uint8_t i=0;

	tab.buses[ind->bus].bus_buf[0][ind->number_in_bus]=ind->shutdown;
	tab.buses[ind->bus].bus_buf[1][ind->number_in_bus]=ind->display_test;
	tab.buses[ind->bus].bus_buf[2][ind->number_in_bus]=ind->scan_limit;
	tab.buses[ind->bus].bus_buf[3][ind->number_in_bus]=ind->brightness;
	tab.buses[ind->bus].bus_buf[4][ind->number_in_bus]=ind->decode_mode;
	tab.buses[ind->bus].bus_buf[5][ind->number_in_bus]=0x117;
	tab.buses[ind->bus].bus_buf[6][ind->number_in_bus]=0x27E;
	tab.buses[ind->bus].bus_buf[7][ind->number_in_bus]=0x317;
	tab.buses[ind->bus].bus_buf[8][ind->number_in_bus]=0x47E;
	tab.buses[ind->bus].bus_buf[9][ind->number_in_bus]=0x517;
	tab.buses[ind->bus].bus_buf[10][ind->number_in_bus]=0x67E;
	tab.buses[ind->bus].bus_buf[11][ind->number_in_bus]=0x730;
	tab.buses[ind->bus].bus_buf[12][ind->number_in_bus]=0x830;

	switch(ind->decimal_point)//положение десятичной точки
	{
		case 0:
		{

		}
		break;

		case 1:
		{

		}
		break;

		case 2:
		{

		}
		break;

		case 3:
		{

		}
		break;

		default :
		{

		}
		break;
	}

	for(i=0;i<=ind->scan_limit;i++)
	{

	}
}
