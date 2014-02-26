#include "indicator.h"
#include "tablo.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
extern xSemaphoreHandle xSPI_Buf_Mutex;

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
	uint8_t str[16];

	switch(ind->decimal_point)//��������� ���������� �����
	{
		case 0:
		{
			if(val<0)
			{
				val=0;
			}

			if(val>9999)
			{
				val=9999;
			}
			sprintf(str,"%4d",(unsigned int)val);
		}
		break;

		case 1:
		{
			if(val<0)
			{
				val=0;
			}

			if(val>999.9)
			{
				val=999.9;
			}
			sprintf(str,"%3d.%1d",(unsigned int)val,(unsigned int)(val*10)%10);
		}
		break;

		case 2:
		{
			if(val<0)
			{
				val=0;
			}

			if(val>99.99)
			{
				val=99.99;
			}
			sprintf(str,"%2d.%2d",(unsigned int)val,(unsigned int)(val*100)%100);
		}
		break;

		case 3:
		{
			if(val<0)
			{
				val=0;
			}

			if(val>9.999)
			{
				val=9.999;
			}
			sprintf(str,"%1d.%3d",(unsigned int)val,(unsigned int)(val*1000)%1000);
		}
		break;

		default :
		{

		}
		break;
	}

	if( xSemaphoreTake( xSPI_Buf_Mutex, portMAX_DELAY ) == pdTRUE )
	{
		str_to_ind(ind,str);
		xSemaphoreGive( xSPI_Buf_Mutex );
	}
}

uint8_t str_to_ind(struct indicator *ind,uint8_t *str)
{
    uint8_t i=0,j=0;
    uint8_t buf_count=0;//
    uint8_t str_len=0;

    str_len=strlen(str);

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
            if((str[i]>=0x30)&&(str[i]<=0x39))//�����
            {
            	tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]=(Sym_table[1][(str[i]-0x30)])|(0x100*((buf_count-5)+1));
                buf_count++;

                continue;
            }

            if(str[i]=='.')
            {
                if(i==0)
                {
                	tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]|=0x80;
                	buf_count++;
                }

            	if(i>0)
                {
                	if(str[i-1]=='.')
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
               if(str[i]==Sym_table[0][j])//
               {
            	   tab.buses[ind->bus].bus_buf[buf_count][ind->number_in_bus]=(Sym_table[1][j])|(0x100*((buf_count-5)+1));//
                    buf_count++;

                    break;
               }
            }

            if(buf_count>(ind->character_num+5))//����� ������ ���������� ���������
            {
            	break;
            }
        }

//        for(i=buf_count;i<IND_ALL_NUM;i++)
//        {
//        	tab.buses[ind->bus].bus_buf[i][ind->number_in_bus]=0x0;
//        }
        return buf_count;
	//����� �������� � ������ ���� ����������� ������

}
