#include "tablo.h"

struct tablo tab;//

void tablo_indicator_struct_init(void)//
{
	tab.buses[BUS_SPI_1].indicators_num=IND_SPI_BUS_1_NUM;//
//---------------------------------------------------------------------------
	tab.indicators[0].bus=BUS_SPI_1;
	tab.indicators[0].number_in_bus=(IND_SPI_BUS_1_NUM-1)-0;
	tab.indicators[0].type=IND_TYPE_SEGMENT;
	tab.indicators[0].character_num=4;
	tab.indicators[0].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[0].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[0].scan_limit=IND_SCAN_LIMIT|(tab.indicators[0].character_num-1);
	tab.indicators[0].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[0].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
}

uint8_t tablo_devices_init(void)//
{
	uint8_t error=0;
	tablo_indicator_struct_init();
	spi_buses_init();

	return error;
}
