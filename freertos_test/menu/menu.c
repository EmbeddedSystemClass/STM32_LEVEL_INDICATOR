//#include "globals.h"
#include "menu.h"
#include "keyboard.h"
#include <stdio.h>
#include <math.h>

#include <string.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_adc.h>
#include "stm32f10x_dac.h"
#include "misc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tablo.h"
#include "indicator.h"
#include "channels.h"

#include "string_utils.h"

extern struct tablo tab;//
extern xQueueHandle xKeyboardQueue;


extern struct Channel channels[CHANNEL_NUMBER];//обобщенная структура каналов
extern unsigned char brightness	;
extern unsigned char signal;

static void DisplayProcess(void *pvParameters);//



enum
{
	CAL_HI=0,
	CAL_LO,
	UST_HI,
	UST_LO
};


uint8_t flag_menu_entry=0;//вошли в меню

typedef struct {
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Child;
	uint8_t     Select;
	uint8_t 	 *Text;
} const menuItem;

menuItem * selectedMenuItem; // текущий пункт меню

menuItem * menuStack[10];
uint8_t menuStackTop;

//char buf[20];
//unsigned char dynamic_disp=0;//номер отображаемого динамического экрана


//unsigned //char num_buf[8];

#define MAKE_MENU(Name, Next, Previous, Parent, Child, Select, Text) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, (unsigned char)Select, { Text }}

#define PREVIOUS   (void*)(selectedMenuItem->Previous)
#define NEXT       (void*)(selectedMenuItem->Next)
#define PARENT     (void*)(selectedMenuItem->Parent)
#define CHILD      (void*)(selectedMenuItem->Child)
#define SELECT	   (void*)(selectedMenuItem->Select)

const uint8_t  strNULL[]  = "";

#define NULL_ENTRY Null_Menu
menuItem        Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, 0, {0x00}};

//                 NEXT,      PREVIOUS     PARENT,     CHILD
MAKE_MENU(m_s0i1,  NULL_ENTRY,NULL_ENTRY,  NULL_ENTRY, m_s1i1,       0, 	"DATA SCREEN");


MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  m_s0i1,     m_s3i1,       0, 					" 5Et");
MAKE_MENU(m_s1i2,  m_s1i3,	  m_s1i1,      m_s0i1,     NULL_ENTRY,   MENU_SET_BRIGHTNESS,   " brI");
MAKE_MENU(m_s1i3,  NULL_ENTRY,m_s1i2,      m_s0i1,     NULL_ENTRY,   MENU_SET_TYPE_OUT  ,   "tYPE");

// Настройка канала	 1
MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_CHN1_SET_AREA,    "ArEA");
MAKE_MENU(m_s3i2,  NULL_ENTRY,m_s3i1,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_SET_OFFSET, 	"OFF5");
//MAKE_MENU(m_s3i3,  m_s3i4,	  m_s3i2,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_UST_HI, 		"U hI");
//MAKE_MENU(m_s3i4,  m_s3i5,    m_s3i3,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_UST_LO, 		"U LO");
//MAKE_MENU(m_s3i5,  NULL_ENTRY,m_s3i4,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_DIAP, 		"dIAP");

//MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_CHN1_CAL_HI, 		"C hI");
//MAKE_MENU(m_s3i2,  m_s3i3,    m_s3i1,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_CAL_LO, 		"C LO");
//MAKE_MENU(m_s3i3,  m_s3i4,	  m_s3i2,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_UST_HI, 		"U hI");
//MAKE_MENU(m_s3i4,  m_s3i5,    m_s3i3,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_UST_LO, 		"U LO");
//MAKE_MENU(m_s3i5,  NULL_ENTRY,m_s3i4,      m_s1i1,     NULL_ENTRY,   MENU_CHN1_DIAP, 		"dIAP");


enum
{
	BLINK_ALL=0xF,
	BLINK_NONE=0xFF
};

static uint8_t enter_flag=0; //зашли в поле ввода


unsigned char menuHandler(menuItem* currentMenuItem,unsigned char key);	 //обработка меню

void InputFieldKey(unsigned char key,unsigned char channel,unsigned char type);
void SetBrightnessKey(unsigned char key);
void SetTypeOutKey(unsigned char key);

void Set_Blink_Sym(struct Channel *chn,unsigned char sym_position);


void menuChange(menuItem * NewMenu)
{
	if (NewMenu == &NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

void menuChange_NEXT(void)
{
	  menuItem  * tempMenu;

	if((selectedMenuItem->Next==&NULL_ENTRY)&&(selectedMenuItem->Parent!=&NULL_ENTRY))
	{
		tempMenu = selectedMenuItem->Parent;
		menuChange(tempMenu->Child);	
	}
	else
	{
		menuChange(selectedMenuItem->Next);	
	}	
}

unsigned char dispMenu(void)
{
	unsigned char i=0;

	if (selectedMenuItem == &m_s0i1) 
	{ // мы на верхнем уровне
	} 
	else 
	{
		Set_Blink_Sym(&channels[0],BLINK_NONE);
		str_to_ind(&tab.indicators[0],selectedMenuItem->Text,"XXXXX");
	}
	enter_flag=0;
	return (1);
}

unsigned char menuKey(unsigned char key) 
{
	menuItem* sel;

	if(!flag_menu_entry)
	{
		switch (key) 
		{
			case 0: 
			{
				return 1;
			}
			break;
		
			case '+': 
			{
//				menuChange(PREVIOUS);	
			}
			break;
		
			case '>': 
			{
			//	menuChange(NEXT);
				menuChange_NEXT();		
			}
			break;
		
		
			case 'E':
			{ // выбор пункта
//					sel = selectedMenuItem->Select;//SELECT;
					if (selectedMenuItem->Select != 0) 
					{
//						flag_menu_entry=1;
						menuHandler(selectedMenuItem,key);	
						return (1);
					} 
					else 
					{
						menuChange(CHILD);
					}
			}
			break;
	
			case 'Q': 
			{ // отмена выбора (возврат)
				menuChange(PARENT);
			}
			break;
	
			default:
			{
	
			}		
		} 
	    dispMenu(); 
	}
	else
	{
		if(key== 'Q') 
		{ // отмена выбора (возврат)
			flag_menu_entry=0;
			dispMenu(); 
		}
		else
		{
			sel = SELECT;
			if (sel != 0) 
			{
				menuHandler(selectedMenuItem,key);
			}			 
		}
	}
	return (1);
}
//-----------------------------------------------------
unsigned char menuHandler(menuItem* currentMenuItem,unsigned char key)	 //обработка меню
{
	flag_menu_entry=1;
	
	switch (currentMenuItem->Select) 
	{
		case MENU_SET_BRIGHTNESS:
		{	
			SetBrightnessKey(key);
		}
		break;

		case MENU_SET_TYPE_OUT:
		{
			SetTypeOutKey(key);
		}
		break;
		
		case MENU_CHN1_SET_AREA:
		{
			InputFieldKey(key,0,MENU_CHN1_SET_AREA);
		}
		break; 

		case MENU_CHN1_SET_OFFSET:
		{
			InputFieldKey(key,0,MENU_CHN1_SET_OFFSET);
		}
		break; 
	}	
	return 0;
}
//-----------------------------------------------------
unsigned char startMenu(void)
 {
	selectedMenuItem = &m_s0i1;

	dispMenu();
	xTaskCreate(DisplayProcess,(signed char*)"DisplayProcess",256,NULL, tskIDLE_PRIORITY + 1, NULL);
    Set_Blink_Sym(&channels[0],BLINK_NONE);
    tab.indicators[0].decimal_point=0;
	return 0;
}
//-------------------------------------------------------
void SetBrightnessKey(unsigned char key)
{
		switch(key)
		{
			case 'E'://enter
			{
				if(!enter_flag)
				{
				   //отобразить параметр
				   enter_flag=1;
				}
				else
				{
					//сохранить параметр в eeprom
					enter_flag=0;
				    flag_menu_entry=0;
					dispMenu(); 
				}
			}
			break;

//			case 'Q'//quit
//			{
//			}
//			break;

			case '>'://shift
			{
				
			}
			break;

			case '+'://increment
			{
				tab.indicators[0].brightness=IND_BRIGHTNESS|((tab.indicators[0].brightness+1)&0xF);
			}
			break;
		}
		
		if(enter_flag)
		{
			indicators_set_num(&tab.indicators[0],(float)(tab.indicators[0].brightness&0xF),0,"XXXXX");
		}
}
//-------------------------------------------------------
enum
{
	TYPE_OUTPUT_LINEAR=0,
	TYPE_OUTPUT_M3=1
};

void SetTypeOutKey(unsigned char key)
{
	switch(key)
	{
		case 'E'://enter
		{
			if(!enter_flag)
			{
			   //отобразить параметр
			   channels[0].calibrate.cal.type_output&=0x1;
			   enter_flag=1;
			}
			else
			{
				//сохранить параметр в eeprom
				enter_flag=0;
			    flag_menu_entry=0;
				dispMenu();
			}
		}
		break;

//			case 'Q'//quit
//			{
//			}
//			break;

		case '>'://shift
		{

		}
		break;

		case '+'://increment
		{
			channels[0].calibrate.cal.type_output=(channels[0].calibrate.cal.type_output^1)&0x1;
		}
		break;
	}

	if(enter_flag)
	{
		if(channels[0].calibrate.cal.type_output==TYPE_OUTPUT_M3)
		{
			str_to_ind(&tab.indicators[0]," CUb","XXXXX");
		}
		else
		{
			str_to_ind(&tab.indicators[0],"LInE","XXXXX");
		}
	}
}
//-------------------------------------------------------
#define CAL_ENTER_FIELD	0
void InputFieldKey(unsigned char key,unsigned char channel,unsigned char type)
{
		float value;
		static unsigned char  current_char=0;
		switch(key)
		{
			case 'E'://enter
			{
				if(!enter_flag)
				{
					current_char=0;

					switch(type)
					{
						case MENU_CHN1_SET_AREA:
						{
							value=channels[channel].calibrate.cal.area;
							tab.indicators[0].decimal_point=1;
							  if((isnan(value)==0)&&(value>=0.0)&&(value<=999.9))
							  {
								  float_to_string(value,channels[CAL_ENTER_FIELD].string_buf,tab.indicators[0].decimal_point,'1');
							  }
							  else
							  {
								  sprintf(channels[CAL_ENTER_FIELD].string_buf,"000.0");
							  }
						}
						break;

						case MENU_CHN1_SET_OFFSET:
						{
							value=(float)channels[channel].calibrate.cal.offset;
							tab.indicators[0].decimal_point=0;
						  if((value>=0.0)&&(value<=9999))
						  {
							  float_to_string(value,channels[CAL_ENTER_FIELD].string_buf,tab.indicators[0].decimal_point,'0');
						  }
						  else
						  {
							  sprintf(channels[CAL_ENTER_FIELD].string_buf,"0000");
						  }
						}
						break;
					}


				  str_to_ind(&tab.indicators[0],channels[CAL_ENTER_FIELD].string_buf,"XXXXX");

				  enter_flag=1;
				  Set_Blink_Sym(&channels[CAL_ENTER_FIELD],current_char);
			    }
				else
				{
					  if(string_to_float(channels[CAL_ENTER_FIELD].string_buf,&value)==0)
					  {
						  if((isnan(value)==0)&&(value>=0.00)&&(value<=9999))
						  {
								switch(type)
								{
									case MENU_CHN1_SET_AREA:
									{
										channels[channel].calibrate.cal.area=value;
									}
									break;

									case MENU_CHN1_SET_OFFSET:
									{
										channels[channel].calibrate.cal.offset=(uint16_t)value;
									}
									break;
								}
								//SetCalibration(channel,type);
						 }
					}
					enter_flag=0;
				    flag_menu_entry=0;
					dispMenu();
				}
			}
			break;

//			case 'Q'//quit
//			{
//			}
//			break;

			case '>'://shift
			{
			 	current_char++;

				switch(tab.indicators[0].decimal_point)
				{
					case 0:
					{
						if(current_char>3)
						{
							current_char=0;
						}
					}
					break;

					case 1:
					{
						if(current_char==3)
						{
							current_char=4;
						}

						if(current_char>4)
						{
							current_char=0;
						}
					}
					break;

					case 2:
					{
						if(current_char==2)
						{
							current_char=3;
						}

						if(current_char>4)
						{
							current_char=0;
						}
					}
					break;

					case 3:
					{
						if(current_char==1)
						{
							current_char=2;
						}

						if(current_char>4)
						{
							current_char=0;
						}
					}
					break;

					default:
					{

					}
					break;
				}
				Set_Blink_Sym(&channels[CAL_ENTER_FIELD],current_char);
			}
			break;

			case '+'://increment
			{
				channels[CAL_ENTER_FIELD].string_buf[current_char]++;
				if((channels[CAL_ENTER_FIELD].string_buf[current_char]<'0')||(channels[CAL_ENTER_FIELD].string_buf[current_char]>'9'))
				{
					channels[CAL_ENTER_FIELD].string_buf[current_char]='0';
				}
				//channels[CAL_ENTER_FIELD].string_buf[1]='.';
				//str_to_ind(&tab.indicators[0],channels[CAL_ENTER_FIELD].string_buf,"XXXXX");
			}
			break;
		}

		if(enter_flag)
		{
		}
}

void Set_Blink_Sym(struct Channel *chn,unsigned char sym_position)
{
	if(sym_position<5)
	{
			sprintf(chn->string_mask,"XXXXX");
			chn->string_mask[sym_position]=' ';
	}
	else
	{
		if(sym_position==0xF)
		{
			sprintf(chn->string_mask,"     ");
		}
		else
		{
			sprintf(chn->string_mask,"XXXXX");
		}
	}
}
//--------------------------------------------------------
#define BEAM_LEN_MM	2500.0
static void DisplayProcess(void *pvParameters)
{

  static uint8_t i=0;
  static float value;
  uint8_t key;
  uint8_t blink_flag=0;
  uint8_t blink_count=0;
  //  wdt_count[Display_Proc].process_state=RUN;
  while(1) 
  {
//	wdt_count[Display_Proc].process_state=IDLE;  

	vTaskDelay(50);
	if( uxQueueMessagesWaiting(xKeyboardQueue) != 0 )//keyboard poll
	{
		xQueueReceive( xKeyboardQueue, &key, 0 );
		menuKey(key);
	}

	if(blink_count<2)
	{
		blink_count++;
	}
	else
	{
		blink_count=0;
		blink_flag^=1;
	}


   if(selectedMenuItem == &m_s0i1)//main screen
   {
		if(channels[0].calibrate.cal.type_output==TYPE_OUTPUT_M3)
		{
			float volume;
			volume=(((float)channels[0].calibrate.cal.offset+((float)channels[0].channel_data/4096)*BEAM_LEN_MM)/1000)*(float)channels[0].calibrate.cal.area;
			indicators_set_num(&tab.indicators[0],volume,1,"XXXXX");
		}
		else
		{
			indicators_set_num(&tab.indicators[0],((float)channels[0].channel_data/4096)*BEAM_LEN_MM,0,"XXXXX");
		}
   }

   if(((selectedMenuItem == &m_s3i1)||(selectedMenuItem == &m_s3i2)/*||(selectedMenuItem == &m_s3i3)||(selectedMenuItem == &m_s3i4)||(selectedMenuItem == &m_s3i5)*/)&&enter_flag)//settings screen
   {
	   if(blink_flag)
	   {
		   str_to_ind(&tab.indicators[0],channels[CAL_ENTER_FIELD].string_buf,channels[0].string_mask);
	   }
	   else
	   {
		   str_to_ind(&tab.indicators[0],channels[CAL_ENTER_FIELD].string_buf,"XXXXX");
	   }

   }
	taskYIELD();
	//	wdt_count[Display_Proc].count++;
  }
 }
