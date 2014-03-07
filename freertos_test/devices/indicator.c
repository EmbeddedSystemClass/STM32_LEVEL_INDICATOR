#include "indicator.h"
#include "tablo.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "string_utils.h"
extern xSemaphoreHandle xSPI_Buf_Mutex;

#define SYM_TAB_LEN 32
const uint8_t Sym_table[2][SYM_TAB_LEN]={{'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F','h','I','I','J','L','O','P','r','t','U','u','.','-','_',' ','n'},
                                         {0x7E/*0*/,0x30/*1*/,0x6D/*2*/,0x79/*3*/,0x33/*4*/,0x5B/*5*/,0x5F/*6*/,0x70/*7*/,0x7F/*8*/,0x7B/*9*/,0x77/*A*/,
                                          0x1F/*b*/,0x4E/*C*/,0x3D/*d*/,0x4F/*E*/,0x47/*F*/,0x17/*h*/,0x30/*I*/,0x10/*i*/,0x3C/*J*/,0xE/*L*/,0x7E/*O*/,
                                          0x67/*P*/,0x05/*r*/,0x0F/*t*/,0x3E/*U*/,0x1C/*u*/,0x80/*.*/,0x01/*-*/,0x08/*_*/,0x00/* */,0x15/*n*/}};


extern struct tablo tab;

uint8_t indicators_init(void)//
{
	uint8_t error=0;


	return error;
}

void indicators_set_num(struct indicator *ind,float val, uint8_t decimal_point, uint8_t *mask)
{
	uint8_t str[16];


	if(float_to_string(val,str,decimal_point,' ')==1)
	{
		return;//NaN
	}

	if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
	{
		str_to_ind(ind,str,mask);
		xSemaphoreGive( xSPI_Buf_Mutex );
	}
	return;
}

uint8_t str_to_ind(struct indicator *ind,uint8_t *str, uint8_t *mask)
{
    uint8_t i=0,j=0;
    uint8_t buf_count=0;//
    uint8_t str_len=0;
    uint8_t str_temp[16]={0};

    str_len=strlen(str);

    for(i=0;i<str_len;i++)
    {
    	if(mask[i]==' ')
    	{
    		str_temp[i]=' ';
    	}
    	else
    	{
    		str_temp[i]=str[i];
		}
    }

    	tab.buses[ind->bus].bus_buf[0][ind->number_in_bus]=ind->shutdown;
    	tab.buses[ind->bus].bus_buf[1][ind->number_in_bus]=ind->display_test;
    	tab.buses[ind->bus].bus_buf[2][ind->number_in_bus]=ind->scan_limit;
    	tab.buses[ind->bus].bus_buf[3][ind->number_in_bus]=ind->brightness;
    	tab.buses[ind->bus].bus_buf[4][ind->number_in_bus]=ind->decode_mode;
    	tab.buses[ind->bus].bus_buf[5][ind->number_in_bus]=0x100;
    	tab.buses[ind->bus].bus_buf[6][ind->number_in_bus]=0x200;
    	tab.buses[ind->bus].bus_buf[7][ind->number_in_bus]=0x300;
    	tab.buses[ind->bus].bus_buf[8][ind->number_in_bus]=0x400;
    	tab.buses[ind->bus].bus_buf[9][ind->number_in_bus]=0x500;
    	tab.buses[ind->bus].bus_buf[10][ind->number_in_bus]=0x600;
    	tab.buses[ind->bus].bus_buf[11][ind->number_in_bus]=0x700;
    	tab.buses[ind->bus].bus_buf[12][ind->number_in_bus]=0x800;

        buf_count+=5;

        for(i=0;i<str_len;i++)//
        {
            if((str_temp[i]>=0x30)&&(str_temp[i]<=0x39))//цифры
            {
            	tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]=(Sym_table[1][(str_temp[i]-0x30)])|(0x100*((buf_count-5)+1));
                buf_count++;

                continue;
            }

            if(str_temp[i]=='.')
            {
                if(i==0)
                {
                	tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]|=0x80;
                	buf_count++;
                }

            	if(i>0)
                {
                	if(str_temp[i-1]=='.')
                	{
                    	tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]|=0x80;
                    	buf_count++;
                	}
                	else
                	{
                		tab.buses[ind->bus].bus_buf[buf_count-1][ind->number_in_bus]|=0x80;
                	}
                }
                continue;
            }

            for(j=10;j<SYM_TAB_LEN;j++)//
            {
               if(str_temp[i]==Sym_table[0][j])//
               {
            	    tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]=(Sym_table[1][j])|(0x100*((buf_count-5)+1));//
                    buf_count++;

                    break;
               }
            }

            if(buf_count>(ind->character_num+5))//буфер больше количества знакомест
            {
            	break;
            }
        }

//        for(i=buf_count;i<IND_ALL_NUM;i++)
//        {
//        	tab.buses[ind->bus].bus_buf[i][ind->number_in_bus]=0x0;
//        }
        return buf_count;
	//перед доступом к буферу шины критическая секция

}
