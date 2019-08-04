/*
 * lgt8f328p dsu test
 *
 * Created: 2017/9/2 11:08:55
 *  Author: LGT
 */ 

#include "lgt8f328p_udsu.h"



int array_dx[] = {
	8, 1000, -20, 44, 80, -100, 107, 20,
	9, 10, -1, -1, 40, 20, 10, 7,
	0, 5, 7, 13, 20, -31, 6, -1,
	9, 120, -22, 34, 80, -110, 117, 11	
};

int array_dy[] = {
	1, 2, 1, 3, 4, 2, 1, 2,
	3, 2, -1, -1, 4, 2, 1, 3,
	3, 1, 0, 4, 2, 11, -17, 1,
	1, 2, 1, 3, 4, 2, 1, 2	
};


int main(void)
{
	volatile unsigned int uint16 = 0;
	volatile int int16 = 0;
	volatile unsigned long uint32 = 0;
	volatile long int32 = 0;
	
	// initial PC0 to show test result
	DDRC = 0xff;
	PORTC &= 0xfe;
	
	// initial uDSU module to fast io addressing mode
	dsu_init(DSU_MM_FAST);
	
	// test code start
	// =======================================================
	uint16 = dsu_xadd(1000, 3111);
	if(uint16 != 4111)
		goto error;
	
	int16 = dsu_xads(-1000, -3111);
	if(int16 != -4111)
		goto error;
	
	uint32 = dsu_aday(0x70000000, 0x1000);
	if(uint32 != 0x70001000)
		goto error;
	

	uint32 = dsu_xmuluu(20, 300);	
	if(uint32 != 6000)
		goto error;
	

	int32 = dsu_xmulsu(-20, 300);
	if(int32 != -6000)
		goto error;
	
	int32 = dsu_xmulss(-20, 100);
	if(int32 != -2000)
		goto error;
	
	int32 = dsu_xmulss(-20, -200);
	if(int32 != 4000)
		goto error;
	
	uint32 = dsu_xmacuu1(0, 40, 300);
	if(uint32 != 12000)
		goto error;
		
	uint32 = dsu_xmacuu1(1000, 40, 300);
	if(uint32 != 13000)
	goto error;		
		
	uint32 = dsu_xmacuu0(20, 500);
	if(uint32 != 23000)
		goto error;
	
	uint32 = dsu_usqx1(200);
	if(uint32 != 40000)
		goto error;
	
	uint32 = dsu_usqy1(300);
	if(uint32 != 90000)
		goto error;
	
	dsu_clr();
	uint32 = dsu_da();
	if(uint32 != 0)
		goto error;
	
	uint32 = dsu_xadd(0, 0x400);
	if(uint32 != 0x400)
		goto error;
	
	int32 = dsu_uneg2();
	if(int32 != -0x400)
		goto error;
	
	uint32 = dsu_abs2();
	if(uint32 != 0x400)
		goto error;
	
	dsu_xadd(0, 0);
	uint32 = dsu_da();
	if(uint32 != 0)
		goto error;
	
	uint32 = dsu_div1(33333, 11111);
	if(uint32 != 3)
		goto error;
	
	uint16 = dsu_dy();
	if(uint16 != 0)
		goto error;
	
	uint32 = dsu_div1(5666, 1111);
	if(uint32 != 5)
		goto error;
	
	uint16 = dsu_dy();
	if(uint16 != 111)
		goto error;
	
	// clean up dsu status
	dsu_clr();	

	// add 0x2000 offset to map to 16bit SRAM
	int32 = dsu_fmacss(((uint16_t)array_dx | 0x2000), ((uint16_t)array_dy | 0x2000), 32);
	
	PORTC |= 0x01;

	// test done
	while(1);
	
error:	// if test failed!
	while(1)
	{
		PORTC |= 0x01;
		PORTC &= 0xFE;
		
	}	
	
	return 0;
}
