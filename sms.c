
#include "shared.h"

/* SMS context */
t_sms sms;

unsigned char sms_emulate_voice=1;
unsigned char sms_tv=0;

/* Run the virtual console emulation for one frame */

void sms_frame(int skip_render)
{
	int snd_idx=0;
	
    /* Take care of hard resets */
    if(input.system & INPUT_HARD_RESET)
    {
        system_reset();
    }

    /* Debounce pause key */
    if(input.system & INPUT_PAUSE)
    {
        if(!sms.paused)
        {
            sms.paused = 1;

            z80_set_nmi_line(ASSERT_LINE);
            z80_set_nmi_line(CLEAR_LINE);
        }
    }
    else
    {
         sms.paused = 0;
    }

	/* 3d Glasses */
	if(rom_flag & f3D_GLASSES)
		skip_render = sms.ram[0x1ffb]==0 ? 1 :0;

//	sms.ram[0x1CA6]=vdp_hcounter_r();
	    
    for(vdp.line = 0; vdp.line < sms.lines; vdp.line += 1)
    {       
		/* Handle VDP line events */
        vdp_run();
	
        /* Draw the current frame */
        if(!skip_render) render_line(vdp.line);

	    /* Run the Z80 for a line */
        z80_execute(227);
						
		if(sms_emulate_voice==1)
		{
			SN76496Update(&snd.psg_buffer[0][snd_idx],2);
			snd_idx+=2;				
		}
    }    

	if(sms_emulate_voice==0)
	{
		SN76496Update(&snd.psg_buffer[0][0],snd.bufsize);			
	}
}


void sms_init(void)
{
#if PSX
    z80_set_context((void *)0x1F800000);
#endif
    cpu_reset();
    sms_reset();
	
}


void sms_reset(void)
{
    /* Clear SMS context */
	memset(sms.dummy, 0, 0x2000);
	memset(sms.ram, 0xff, 0x2000);
	memset(sms.sram, 0, 0x8000);
    sms.paused = sms.save = sms.port_3F = sms.port_F2 = sms.irq = 0x00;
    sms.psg_mask = 0xFF;

    /* Load memory maps with default values */
    cpu_readmap[0] = &cart.rom[0] + 0x0000;
    cpu_readmap[1] = &cart.rom[0] + 0x2000;
    cpu_readmap[2] = &cart.rom[0] + 0x4000;
    cpu_readmap[3] = &cart.rom[0] + 0x6000;
	cpu_readmap[4] = &cart.rom[0] + 0x8000;
	cpu_readmap[5] = &cart.rom[0] + 0xA000;
    cpu_readmap[6] = &sms.ram[0];            
    cpu_readmap[7] = &sms.ram[0];

    cpu_writemap[0] = &sms.dummy[0];         
    cpu_writemap[1] = &sms.dummy[0];
    cpu_writemap[2] = &sms.dummy[0];
    cpu_writemap[3] = &sms.dummy[0];
    cpu_writemap[4] = &sms.dummy[0];
    cpu_writemap[5] = &sms.dummy[0];

    cpu_writemap[6] = &sms.ram[0];
    cpu_writemap[7] = &sms.ram[0];     

    sms.fcr[0] = 0x00;
    sms.fcr[1] = 0x00;
    sms.fcr[2] = 0x01;
    sms.fcr[3] = 0x00;
}


/* Reset Z80 emulator */
void cpu_reset(void)
{
    z80_reset(0);
    z80_set_irq_callback(sms_irq_callback);
	z80_set_sp(0xdff0);
}


/* Write to memory */
void cpu_writemem16(int address, int data)
{
    if((rom_flag & fCODE_MASTER)!=0)
    {
        if(address==0x8000) 
    	{  sms_codemaster_mapper_w(address,data);	   
    	}
    	else
    	{  cpu_writemap[(address >> 13)][(address & 0x1FFF)] = data;	       
        }
    }
    else
    {
        cpu_writemap[(address >> 13)][(address & 0x1FFF)] = data;	
    
    	// normal mapper
        if(address >= 0xFFFC) sms_mapper_w(address & 3, data);

    }
	
			
}


/* Write to an I/O port */
void cpu_writeport(int port, int data)
{
    switch(port & 0xFF)
    {
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            break;

        case 0x06: /* GG STEREO */
            if(snd.log) {
            snd.callback(0x04);
            snd.callback(data);
            }
            sms.psg_mask = (data & 0xFF);
            break;

        case 0x7E: /* SN76489 PSG */
        case 0x7F:
            if(snd.log) {
            snd.callback(0x03);
            snd.callback(data);
            }
            if(snd.enabled) SN76496Write(data);
            break;

        case 0xBE: /* VDP DATA */
            vdp_data_w(data);
            break;

        case 0xBD: /* VDP CTRL */ 
        case 0xBF:
            vdp_ctrl_w(data);
            break;

        case 0xF0: /* YM2413 */
        case 0xF1:
            if(snd.log) {
            snd.callback((port & 1) ? 0x06 : 0x05);
            snd.callback(data);
            }
//            if(snd.enabled && sms.use_fm) ym2413_write(0, port & 1, data);
            break;

        case 0xF2: /* YM2413 DETECT */
            if(sms.use_fm) sms.port_F2 = (data & 1);
            break;

        case 0x3F: /* TERRITORY CTRL. */
             sms.port_3F = ((data & 0x80) | (data & 0x20) << 1) & 0xC0;
            if(sms.country == TYPE_DOMESTIC) sms.port_3F ^= 0xC0;
                        
            sms.port_3F_raw = data;
                        
            break;
    }
}


/* Read from an I/O port */
int cpu_readport(int port)
{
    byte temp = 0xFF;

    switch(port & 0xFF)
    {
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            return (0x00);
    
      case 0x7E: /* V COUNTER */
            //For lightgun games, the v counter will be latched with the v beam position
            //Not totally accurate emulation but it works for most gun games.
            if(rom_flag & fLIGHT_PHASER) 
            {   return input.lightgun_y >> 8;}

            return (vdp_vcounter_r());
            break;
    
        case 0x7F: /* H COUNTER */
          //<heliophobe>
            //For lightgun games, this will be latched with the h beam position. 
            //Most games take a few samples and each have different ways of averaging the results
            //Because of this, it may be necessary to adjust the h position a bit for each game.
            //TO DO BEFORE NEXT RELEASE: do adjustments for various games based on CRC. A switch/case should suffice.
            if(rom_flag & fLIGHT_PHASER) 
            {   return ((input.lightgun_x >> 8) * 124) / 255 + 18; //updated 
            }
            //</heliophobe>
            return (vdp_hcounter_r());
            break;
    
        case 0x00: /* INPUT #2 */
            temp = 0xFF;
            if(input.system & INPUT_START) temp &= ~0x80;
            if(sms.country == TYPE_DOMESTIC) temp &= ~0x40;
            return (temp);
    
        case 0xC0: /* INPUT #0 */  
        case 0xDC:
            temp = 0xFF;
            if(input.pad[0] & INPUT_UP)      temp &= ~0x01;
            if(input.pad[0] & INPUT_DOWN)    temp &= ~0x02;
            if(input.pad[0] & INPUT_LEFT)    temp &= ~0x04;
            if(input.pad[0] & INPUT_RIGHT)   temp &= ~0x08;
            if(input.pad[0] & INPUT_BUTTON2) temp &= ~0x10;
            if(input.pad[0] & INPUT_BUTTON1) temp &= ~0x20;
            if(input.pad[1] & INPUT_UP)      temp &= ~0x40;
            if(input.pad[1] & INPUT_DOWN)    temp &= ~0x80;
            //<heliophobe>
            //For paddle emulation. If a specific value is written to port 0x3f, the bottom nibble
            //will contain either the high or low nibble of the paddle position.
            //paddle position ranges 0 - 255, with 127 being the centered value.
   			if(sms.port_3F_raw == 0xfd) 
            {
				sms.port_3F_raw = 0;
				return (temp & 0xf0) | ((input.paddle >> 12) & 0xf); //Update - scaled paddle values by 256			
            }
			else if(sms.port_3F_raw == 0xdd)
            {
				sms.port_3F_raw = 0;
				return (temp & 0xf0) | ((input.paddle >>8) & 0xf);  //Update - scaled paddle values by 256
			}
            //<heliophobe>      
            
            return (temp);

    
        case 0xC1: /* INPUT #1 */
        case 0xDD:
            temp = 0xFF;
            //<heliophobe>
            //I'm not 100% confident any of this makes sense, I should definately revisit it in the future.
            //The idea here is that if the light gun is enabled, bit 6 of port dd will be cleared when it
            //sees the white light from the electron beam. It should stay lit long enough  to be detected
            //by a z80 loop (20-30ms according to the official docs).
            if(rom_flag & fLIGHT_PHASER) {
               if((vdp_hcounter_r() - (input.lightgun_x>>9)) < 30 ) { //Updated for 16bit analog sensitivity
                 if( (vdp.line - (input.lightgun_y>>8)) < 4) temp &= ~0x40; //Updated for 16bit analog sensitivity
               }
            }
            //</heliophobe>
            if(input.pad[1] & INPUT_LEFT)    temp &= ~0x01;
            if(input.pad[1] & INPUT_RIGHT)   temp &= ~0x02;
            if(input.pad[1] & INPUT_BUTTON2) temp &= ~0x04;
            if(input.pad[1] & INPUT_BUTTON1) temp &= ~0x08;
            if(input.system & INPUT_SOFT_RESET) temp &= ~0x10;
            //<heliophobe>
            //Special handling of the upper bits if the gun is enabled.
            //Otherwise they're read back from port 0x3f (used for region detection I think)
            //This might interfere with region detection for gun games -- though it doesn't seem to matter so far.
            if(rom_flag & fLIGHT_PHASER)
            {   return (temp & 0xff);}
            return ((temp & 0x3F) | (sms.port_3F & 0xC0));

        case 0xBE: /* VDP DATA */
            return (vdp_data_r());
    
        case 0xBD:
        case 0xBF: /* VDP CTRL */
            return (vdp_ctrl_r());

        case 0xF2: /* YM2413 DETECT */
            if(sms.use_fm) 
                return (sms.port_F2);
            break;
    }
    return (0xFF);     
}


void sms_codemaster_mapper_w(int address,int data)
{
  	byte page=data % cart.pages;	
	cpu_readmap[(address >> 13)] = &cart.rom[(page << 14) + 0x0000];
    cpu_readmap[(address >> 13)+1] = &cart.rom[(page << 14) + 0x2000];                       	   
}



void sms_mapper_w(int address, int data)
{
    /* Calculate ROM page index */
    byte page = (data % cart.pages);

    /* Save frame control register data */
    sms.fcr[address] = data;

    switch(address)
    {
        case 0:
            if(data & 8)
            {
                sms.save = 1;
                /* Page in ROM */
                cpu_readmap[4]  = &sms.sram[(data & 4) ? 0x4000 : 0x0000];
                cpu_readmap[5]  = &sms.sram[(data & 4) ? 0x6000 : 0x2000];
                cpu_writemap[4] = &sms.sram[(data & 4) ? 0x4000 : 0x0000];
                cpu_writemap[5] = &sms.sram[(data & 4) ? 0x6000 : 0x2000];
            }
            else
            {
                /* Page in RAM */
                cpu_readmap[4]  = &cart.rom[((sms.fcr[3] % cart.pages) << 14) + 0x0000];
                cpu_readmap[5]  = &cart.rom[((sms.fcr[3] % cart.pages) << 14) + 0x2000];
                cpu_writemap[4] = &sms.dummy[0];
                cpu_writemap[5] = &sms.dummy[0];
            }
            break;

        case 1:
            cpu_readmap[0] = &cart.rom[(page << 14) + 0x0000];
            cpu_readmap[1] = &cart.rom[(page << 14) + 0x2000];
            break;

        case 2:
            cpu_readmap[2] = &cart.rom[(page << 14) + 0x0000];
            cpu_readmap[3] = &cart.rom[(page << 14) + 0x2000];
            break;

        case 3:
            if(!(sms.fcr[0] & 0x08))
            {
                cpu_readmap[4] = &cart.rom[(page << 14) + 0x0000];
                cpu_readmap[5] = &cart.rom[(page << 14) + 0x2000];
            }
            break;
    }
}


int sms_irq_callback(int param)
{
    return (0xFF);
}

