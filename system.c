/*
    Copyright (C) 1998, 1999, 2000  Charles Mac Donald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "shared.h"

t_bitmap bitmap;
t_cart cart;   
t_sms_bios bios_sms;           
t_snd snd;
t_input input;

#ifdef FMCHIP
FM_OPL *ym3812;
#endif

void system_init(int rate,byte tv)
{
	/* Initilize tv */
	system_set_tv(tv);

    /* Initialize the VDP emulation */
    vdp_init();

    /* Initialize the SMS emulation */
    sms_init();

    /* Initialize the look-up tables and related data */
    render_init();

    /* Enable sound emulation if the sample rate was specified */
    audio_init(rate);

    /* Don't save SRAM by default */
    sms.save = 0;
	
    /* Clear emulated button state */
    memset(&input, 0, sizeof(t_input));
}

void audio_init(int rate)
{
    /* Clear sound context */
    memset(&snd, 0, sizeof(t_snd));

    /* Reset logging data */
    snd.log = 0;
    snd.callback = NULL;

    /* Oops.. sound is disabled */
    if(!rate) return;

    /* Calculate buffer size in samples */
    snd.bufsize = (rate / (sms_tv==TV_PAL ? 50 : 60) );	
			
    /* Sound output */
    //snd.buffer[0] = (signed short *)malloc(snd.bufsize * 2);
    //snd.buffer[1] = (signed short *)malloc(snd.bufsize * 2);
    //if(!snd.buffer[0] || !snd.buffer[1]) return;

	/* YM3812/YM2413 sound stream */
    //snd.fm_buffer = (signed short *)malloc(snd.bufsize * 2);
    //if(!snd.fm_buffer) return;

    /* SN76489 sound stream */
    //snd.psg_buffer[0] = (signed short *)malloc(snd.bufsize * 2);
    //snd.psg_buffer[1] = (signed short *)malloc(snd.bufsize * 2);

    //if(!snd.psg_buffer[0] || !snd.psg_buffer[1]) return;

    /* Set up SN76489 emulation */
    SN76496_init(MASTER_CLOCK, 255, rate);

#ifdef FMCHIP
    /* Set up YM3812 emulation */
    ym3812 = OPLCreate(OPL_TYPE_YM3812, MASTER_CLOCK, rate);
    if(!ym3812) return;

    /* Set up YM2413 emulation */
    ym2413_init(1);
#endif

    /* Inform other functions that we can use sound */
    snd.enabled = 1;
}


void system_shutdown(void)
{
#ifdef FMCHIP
    if(snd.enabled) OPLDestroy(ym3812);
#endif
}


void system_reset(void)
{
    cpu_reset();
    vdp_reset();
    sms_reset();
    render_reset();
    system_load_sram();	
}

void system_set_tv(byte tv)
{
	switch(tv)
	{
		case TV_NTSC_ENHANCED:
		case TV_PAL:
		{	sms.lines= PAL_LINES;		
		}break;

		case TV_NTSC:
		{	sms.lines= NTSC_LINES;
		}break;	
	}	
}


void system_save_state(void *fd)
{

}


void system_load_state(void *fd)
{
	int i;

	/* Restore callbacks */
    z80_set_irq_callback(sms_irq_callback);

    cpu_readmap[0] = &cart.rom[0] + 0x0000; /* 0000-3FFF */
    cpu_readmap[1] = &cart.rom[0] + 0x2000;
    cpu_readmap[2] = &cart.rom[0] + 0x4000; /* 4000-7FFF */
    cpu_readmap[3] = &cart.rom[0] + 0x6000;
    cpu_readmap[4] = &cart.rom[0] + 0x0000; /* 0000-3FFF */
    cpu_readmap[5] = &cart.rom[0] + 0x2000;
    cpu_readmap[6] = sms.ram;
    cpu_readmap[7] = sms.ram;

    cpu_writemap[0] = sms.dummy;
    cpu_writemap[1] = sms.dummy;
    cpu_writemap[2] = sms.dummy;         
    cpu_writemap[3] = sms.dummy;
    cpu_writemap[4] = sms.dummy;         
    cpu_writemap[5] = sms.dummy;
    cpu_writemap[6] = sms.ram;           
    cpu_writemap[7] = sms.ram;

    sms_mapper_w(3, sms.fcr[3]);
    sms_mapper_w(2, sms.fcr[2]);
    sms_mapper_w(1, sms.fcr[1]);
    sms_mapper_w(0, sms.fcr[0]);

    /* Force full pattern cache update */
    is_vram_dirty = 1;
    memset(vram_dirty, 1, 0x200);

    /* Restore palette */
    for(i = 0; i < PALETTE_SIZE; i += 1)
        palette_sync(i);
}


void system_fix_rom()
{
	unsigned char *p;		
	unsigned char	nibble;
	unsigned short	header_checksum;
	unsigned short	checksum=0;
	int i=0;
	
	// check for the tmr SEGA
	p=&cart.rom[0x7ff0];	
	if(	p[0]!=0x54 || p[1]!=0x4D || p[2]!=0x52 || p[3]!=0x20 || p[4]!=0x53 || p[5]!=0x45 || p[6]!=0x47 ||
		p[7]!=0x41 || p[8]!=0x20 || p[9]!=0x20)
	{
		p[0]=0x54;
		p[1]=0x4D;
		p[2]=0x52;
		p[3]=0x20;
		p[4]=0x53;
		p[5]=0x45;
		p[6]=0x47;
		p[7]=0x41;
		p[8]=0x20;
		p[9]=0x20;
	}

	// Checksum
	header_checksum = (cart.rom[0x7ffa]) | (cart.rom[0x7ffb]<<8);
	nibble = cart.rom[0x7fff] & 0xf;

	for(i=0;i<=0x7FEF;i++)
	{	checksum+=cart.rom[i];}	
	
	switch (nibble)
	{
		//case 0x0: also checksum 0x8000 to 0x3ffff (256KB)
		case 0x0:
		{
			for(i=0x8000;i<=0x3ffff;i++)
			{	checksum+=cart.rom[i];}	
		}break;

		//case 0x1: also checksum 0x8000 to 0x7ffff (512KB)
		case 0x1:
		{
			for(i=0x8000;i<=0x7ffff;i++)
			{	checksum+=cart.rom[i];}	
		}break;

		//also checksum 0x8000 to 0x1ffff (128KB)
		case 0xE:
		case 0xF: 
		{
			for(i=0x8000;i<=0x1ffff;i++)
			{	checksum+=cart.rom[i];}				  
					  
		}break;
	}
	
	if(checksum!=header_checksum)
	{		
		cart.rom[0x7ffa] = checksum & 0xff;
		cart.rom[0x7ffb] = (checksum & 0xff00)>>8;					
	}	
}
