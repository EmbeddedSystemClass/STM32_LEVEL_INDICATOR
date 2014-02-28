#include "string_utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

uint8_t float_to_string(float val, uint8_t *str,uint8_t decimal_point,uint8_t fill_char)
{
	if(isnan(val))
	{
		return 1;
	}
	switch(decimal_point)//положение десятичной точки
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

			if(fill_char==' ')
			{
				snprintf(str,8,"%4u",(unsigned int)val);
			}
			else
			{
				snprintf(str,8,"%04u",(unsigned int)val);
			}
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


			if(fill_char==' ')
			{
				snprintf(str,8,"%3u.%01u",(unsigned int)val,(unsigned int)(val*10)%10);
			}
			else
			{
				snprintf(str,8,"%03u.%01u",(unsigned int)val,(unsigned int)(val*10)%10);
			}
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


			if(fill_char==' ')
			{
				snprintf(str,8,"%2u.%02u",(unsigned int)val,(unsigned int)(val*100)%100);
			}
			else
			{
				snprintf(str,8,"%02u.%02u",(unsigned int)val,(unsigned int)(val*100)%100);
			}
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


			if(fill_char==' ')
			{
				snprintf(str,8,"%1u.%03u",(unsigned int)val,(unsigned int)(val*1000)%1000);
			}
			else
			{
				snprintf(str,8,"%01u.%03u",(unsigned int)val,(unsigned int)(val*1000)%1000);
			}
		}
		break;

		default :
		{

		}
		break;
	}
	return 0;//success
}

uint8_t string_to_float(uint8_t *str, float *val)
{
	uint8_t i=0;
	uint8_t pnt_count=0;
	float temp_val=0.0;
	uint16_t divider=1;
	while(str[i]!=0)
	{
		if(isdigit(str[i])==0)//not num 0..9
		{
			if(str[i]=='.')
			{
				pnt_count++;
			}
			else
			{
				return 1;
			}
		}
		else
		{
			if(pnt_count==0)
			{
				temp_val*=10;
				temp_val+=(str[i]-'0');
			}
			else
			{
				divider*=10;
				temp_val+=((float)(str[i]-'0')/divider);
			}
		}
		i++;
	}
	if(pnt_count>1)
	{
		return 1;
	}

	*val=temp_val;
	return 0;//success
}
