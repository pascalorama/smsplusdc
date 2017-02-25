
#include "dc_shared.h"

t_screenpos screen_pos;
t_config	emu_cfg;
t_config	emu_cfg_o;

t_sms_crc sms_db[]= {
{0xB519E833,"Sonic the Hedgehog",fFORCE_PAL},
{0x5B3B922C,"Sonic the Hedgehog 2 [A]",fFORCE_PAL},
{0x695A9A15,"Jungle Book",fFORCE_PAL},
{0xe959e820,"Jungle Book",fFORCE_PAL},
{0x95B9EA95,"PGA Tour Golf",fPGA_HACK},
{0xac56104f,"Terminator2 JD",fFORCE_PAL},
{0xEDC5C012,"Terminator2",fFORCE_PAL},
{0x0047B615,"Predator2",fFORCE_PAL},
{0xBE31D63F,"Flash, The (UE) [!].sms",fNO_SRAM},
{0xB137007A,"Desert Speedtrap",fNO_SRAM},
{0xC352C7EB,"Lion King",fFORCE_PAL},
{0xAEDF3BDF,"Sonic Chaos",fFORCE_PAL},
{0xfb14f1cf,"Sonic Chaos",fFORCE_PAL},
{0xc2e111ac,"Desert Speedtrap",fNO_SRAM},
{0x72420f38,"Addams Familly",fFORCE_PAL},
{0x3BE7F641,"Renegade.sms",fFORCE_PAL},
{0xE3EC5B2D,"Renegade.sms [BR]",fFORCE_PAL},


{0xFBF96C81,"Blade Eagle 3D (Brazil)",f3D_GLASSES},
{0x8ECD201C,"Blade Eagle 3D (UE) [!]",f3D_GLASSES},
{0x31B8040B,"Maze Hunter 3-D (U) [!]",f3D_GLASSES},
{0xFBE5CFBB,"Missile Defense 3D",f3D_GLASSES},
{0xABD48AD2,"Poseidon Wars 3D",f3D_GLASSES},
{0x6BD5C2BF,"Space Harrier 3D (UE) [!]",f3D_GLASSES},
{0xA3EF13CB,"Zaxxon 3D (UE) [!].sms",f3D_GLASSES},

{0xA577CE46,"Micro Machines.sms",fCODE_MASTER |fFORCE_PAL},
{0x6CAA625B,"Cosmic Space Head.gg",fCODE_MASTER |fFORCE_PAL},
{0xB9664AE1,"Fantastic dizzy.sms",fCODE_MASTER |fFORCE_PAL},
{0x29822980,"Cosmic space head.sms",fCODE_MASTER |fFORCE_PAL},
{0x8813514B,"Excellent Dizzy Collection, The [Proto]",fCODE_MASTER |fFORCE_PAL},
{0xAA140C9C,"Excellent Dizzy Collection, The [SMS-GG]",fCODE_MASTER |fFORCE_PAL},
{0xC888222B,"Fantastic Dizzy [SMS-GG]",fCODE_MASTER |fFORCE_PAL},
{0xB9664AE1,"Fantastic Dizzy",fCODE_MASTER |fFORCE_PAL},

{0xC1756BEE,"Pete Sampras Tennis",fCODE_MASTER |fFORCE_PAL},
{0xF7C524F6,"Micro Machines",fCODE_MASTER |fFORCE_PAL },
{0xDBE8895C,"Micro Machines 2 - Turbo Tournament",fCODE_MASTER |fFORCE_PAL},
{0x5E53C7F7,"Ernie Els Golf",fCODE_MASTER |fFORCE_PAL},
{0x152F0DCC,"Drop Zone",fCODE_MASTER |fFORCE_PAL },

// <heliophobe> 

//Light phaser games. 
{0x0CA95637,"Laser Ghost",fLIGHT_PHASER},
{0x5359762D,"Wanted",fLIGHT_PHASER}, //124 - 18 - 0
{0x5fc74d2a,"Gangster Town",fLIGHT_PHASER}, //124 - 18 - 0
{0xe8215c2e,"Marksman Shooting & Trap Shooting & Safari Hunt",fLIGHT_PHASER},
{0x4B051022,"Shooting Gallery",fLIGHT_PHASER},
{0x861b6e79,"Assault City - Light Phaser Version",fLIGHT_PHASER},
{0xe167a561,"Hang-On & Safari Hunt",fLIGHT_PHASER},
{0xfbe5cfbb,"Missile Defense 3D",fLIGHT_PHASER},
{0x205caae8,"Operation Wolf",fLIGHT_PHASER},
{0x23283f37,"Operation Wolf (alt)",fLIGHT_PHASER},
{0xda5a7013,"Rambo 3",fLIGHT_PHASER},
{0x79ac8e7f,"Rescue Mission",fLIGHT_PHASER},
{0xa908cff5,"Spacegun",fLIGHT_PHASER},
//Games that benefit from absolute paddle positioning.
{0xf9dbb533,"Alex Kidd BMX Trial",fABSOLUTE_PADDLE},
// </heliophobe>







{0,NULL,0},
};



//-----------------------------------------------------------------------------

void default_pal_sms_pos(t_screenpos *t)
{
	t->x=-1;
	t->y=-1;
	t->w=643;
	t->h=643;
}

void default_ntsc_sms_pos(t_screenpos *t)
{
	t->x=-1;
	t->y=-1;
	t->w=640;
	t->h=640;
}

void default_gg_pos(t_screenpos *t)
{	
	t->x=32;
	t->y=10;
	t->w=900;
	t->h=820;	
}

//-----------------------------------------------------------------------------

void default_config(void)
{
	/*
	screen_pos.x=64;
	screen_pos.y=48;
	screen_pos.w=512;
	screen_pos.h=512;
	*/
		
	emu_cfg.sms_tv	= TV_NTSC;
    emu_cfg.country = TYPE_OVERSEAS;
	emu_cfg.bilinear= 1;
	emu_cfg.sprite_limit=0;
	emu_cfg.sram_slot=0;
	emu_cfg.state_slot=0;
	emu_cfg.invert_buttons=0;
	emu_cfg.version=CONFIG_VERSION;
	emu_cfg.paint_backdrop=0;
	emu_cfg.auto_save=1;
	
	emu_cfg.paddle_sensivity=0;

	default_pal_sms_pos(&emu_cfg.pal_sms_pos);
	default_ntsc_sms_pos(&emu_cfg.ntsc_sms_pos);
	default_gg_pos(&emu_cfg.gg_pos);
}

//-----------------------------------------------------------------------------
// Set screen coord

void get_screen_pos(void)
{
	if(cart.type == TYPE_SMS)
	{
		if(emu_cfg.sms_tv==TV_PAL)
		{
			screen_pos.x= emu_cfg.pal_sms_pos.x;
			screen_pos.y= emu_cfg.pal_sms_pos.y;
			screen_pos.w= emu_cfg.pal_sms_pos.w;
			screen_pos.h= emu_cfg.pal_sms_pos.h;
		}
		else
		{
			screen_pos.x= emu_cfg.ntsc_sms_pos.x;
			screen_pos.y= emu_cfg.ntsc_sms_pos.y;
			screen_pos.w= emu_cfg.ntsc_sms_pos.w;
			screen_pos.h= emu_cfg.ntsc_sms_pos.h;				
		}
	}
	else
	{
		screen_pos.x= emu_cfg.gg_pos.x;
		screen_pos.y= emu_cfg.gg_pos.y;
		screen_pos.w= emu_cfg.gg_pos.w;
		screen_pos.h= emu_cfg.gg_pos.h;
	}

}

int check_config()
{
	int changed=0;
	
//	printf("emu_cfg_o\n x %d y %d w %d h %d\n",emu_cfg_o.ntsc_sms_pos.x,emu_cfg_o.ntsc_sms_pos.y,emu_cfg_o.ntsc_sms_pos.w,emu_cfg_o.ntsc_sms_pos.h);
//	printf("emu_cfg\n x %d y %d w %d h %d\n",emu_cfg.ntsc_sms_pos.x,emu_cfg.ntsc_sms_pos.y,emu_cfg.ntsc_sms_pos.w,emu_cfg.ntsc_sms_pos.h);
	
	if(emu_cfg_o.bilinear!= emu_cfg.bilinear) changed=1;
	if(emu_cfg_o.country!= emu_cfg.country) changed=1;
	if(emu_cfg_o.bilinear!= emu_cfg.bilinear) changed=1;
	if(emu_cfg_o.invert_buttons!=emu_cfg.invert_buttons) changed=1;
	if(emu_cfg_o.sms_tv!=emu_cfg.sms_tv) changed=1;
	if(emu_cfg_o.sprite_limit!=emu_cfg.sprite_limit) changed=1;
	if(emu_cfg_o.sram_slot!=emu_cfg.sram_slot) changed=1;
	if(emu_cfg_o.state_slot!=emu_cfg.state_slot) changed=1;
	if(emu_cfg_o.auto_save!=emu_cfg.auto_save) changed=1;
	if(emu_cfg_o.paddle_sensivity!=emu_cfg.paddle_sensivity) changed=1;

	if(emu_cfg_o.ntsc_sms_pos.x !=emu_cfg.ntsc_sms_pos.x) changed=1;
	if(emu_cfg_o.ntsc_sms_pos.y !=emu_cfg.ntsc_sms_pos.y) changed=1;
	if(emu_cfg_o.ntsc_sms_pos.w !=emu_cfg.ntsc_sms_pos.w) changed=1;
	if(emu_cfg_o.ntsc_sms_pos.h !=emu_cfg.ntsc_sms_pos.h) changed=1;


	if(emu_cfg_o.pal_sms_pos.x !=emu_cfg.pal_sms_pos.x) changed=1;
	if(emu_cfg_o.pal_sms_pos.y !=emu_cfg.pal_sms_pos.y) changed=1;
	if(emu_cfg_o.pal_sms_pos.w !=emu_cfg.pal_sms_pos.w) changed=1;
	if(emu_cfg_o.pal_sms_pos.h !=emu_cfg.pal_sms_pos.h) changed=1;

	if(emu_cfg_o.gg_pos.x !=emu_cfg.gg_pos.x) changed=1;
	if(emu_cfg_o.gg_pos.y !=emu_cfg.gg_pos.y) changed=1;
	if(emu_cfg_o.gg_pos.w !=emu_cfg.gg_pos.w) changed=1;
	if(emu_cfg_o.gg_pos.h !=emu_cfg.gg_pos.h) changed=1;

//	printf("config changed %d\n",changed);
	
	return 0;

}
