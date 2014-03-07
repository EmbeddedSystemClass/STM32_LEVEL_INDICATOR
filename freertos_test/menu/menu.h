#ifndef _MENU_H_
#define _MENU_H_


enum {
    MENU_CANCEL=1,

    MENU_CHN_SET,
	    MENU_CHN1_SET,
//			MENU_CHN1_CAL_HI,
//			MENU_CHN1_CAL_LO,
//			MENU_CHN1_UST_HI,
//			MENU_CHN1_UST_LO,
//			MENU_CHN1_DIAP,

			MENU_CHN1_SET_AREA,
			MENU_CHN1_SET_OFFSET,

	MENU_SET_BRIGHTNESS,
	MENU_SET_TYPE_OUT
};

enum
{
	SIGNAL_OFF=0,
	SIGNAL_1=1,
	SIGNAL_2=3,
	SIGNAL_3=5,
	SIGNAL_4=7	
};

unsigned char startMenu(void);
void initMenu(void);
unsigned char dispMenu(void);

//void DynamicDisplay(void);//динамическое отображение параметра
unsigned char menuKey(unsigned char key);
//PT_THREAD(DisplayProcess(struct pt *pt));

#endif
