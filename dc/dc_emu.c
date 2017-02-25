
//-----------------------------------------------------------------------------
#include "shared.h"
#include "dc_shared.h"

//#define DEBUG

//-----------------------------------------------------------------------------
// External stuff


extern t_vdp vdp;

extern	word pixel[PALETTE_SIZE];

extern	int sms_rgb_conv[4];

unsigned char old_sram[0x8000];

int	rom_flag=0;

//-----------------------------------------------------------------------------
// Varaibles


static pvr_poly_hdr_t dc_pvr_emu_poly_hdr;

pvr_ptr_t txr_emu;
pvr_poly_cxt_t poly;
pvr_ptr_t txr_menu;
pvr_ptr_t txr_menutext;
pvr_ptr_t txr_crosshairs;

uint16 emu_vbuffer[256 * 256] __attribute__ ((aligned (32)));

uint16 menu_vbuffer[128 * 128] __attribute__ ((aligned (32)));
uint16 menutext_vbuffer[128 * 128] __attribute__ ((aligned (32)));
uint16 crosshairs_vbuffer[32 *32] __attribute__ ((aligned (32)));

t_screenpos menutext_pos;

// input data
uint8 pads_adr[2]={0,0};

uint8 emu_state=1;
int	emu_join=0;
int emu_pause=0;
int emu_menu_pos=0;
int loaded_tv_mode=-1;

int bma=0;

// emulator structure
t_bitmap bitmap;
int rom_crc=0;
int sram_crc=0;
t_screenpos menu_pos;

maple_device_t *vmu;

uint8   rapidfire_table[10]={0,24,21,18,15,12,9,6,3,2};
int rapidfire_level=0;
int rapidfire_cnt=0;
int rapidfire_flipflop=0;
#define MAX_RAPIDFIRE_LEVEL 10

//------------------------------------------------------------------------------
// Init emu 

void dc_emu_init(void)
{						
    txr_emu = pvr_mem_malloc (256 * 256 * 2);

	txr_menu = pvr_mem_malloc (128 * 128 * 2);
	txr_menutext = pvr_mem_malloc (128 * 128 * 2);
    txr_crosshairs =  pvr_mem_malloc (32 * 32 * 2);
	
	// load de la texture
	LoadPcx2Buffer("/rd/emumenu.pcx",menu_vbuffer);
	fastmemcpy(txr_menu,menu_vbuffer,128*128*2);	
		
	fastmemcpy(txr_menutext,menutext_vbuffer,128*128*2);
	
	LoadPcx2Buffer("/rd/crosshairs.pcx",crosshairs_vbuffer);
	fastmemcpy(txr_crosshairs,crosshairs_vbuffer,32*32*2);	
}


//-----------------------------------------------------------------------------

void display_emu_menu_statustext(char* text)
{
	fastmemset(status_vbuffer,0,256*16*2);
	amiga_textout(&status_vbuffer[0],256,text,STATUS_COLOR);
	fastmemcpy(txr_status,status_vbuffer,256*16*2);
}

//-----------------------------------------------------------------------------
//

void display_emu_menu_screen(int pos)
{
    char    t[256]={""};

	fastmemset(menutext_vbuffer,0,128*128*2);
    
    if(rapidfire_level!=0)  		
        sprintf(t,"Rapid fire %d",rapidfire_level );
    else
        sprintf(t,"Rapid fire %d",0);
    
	// country
	amiga_textout(&menutext_vbuffer[8+0],128,"Load state",pos==0 ? SELECTION_COLOR : LIST_COLOR);
	amiga_textout(&menutext_vbuffer[8+(20*128)],128,"Save state",pos==1 ? SELECTION_COLOR : LIST_COLOR);

	amiga_textout(&menutext_vbuffer[(40*128)+28],128,"Reset",pos==2 ? SELECTION_COLOR : LIST_COLOR);
	amiga_textout(&menutext_vbuffer[(60*128)],128,t,pos==3 ? SELECTION_COLOR : LIST_COLOR);

	if(emu_autobooted==1)
		amiga_textout(&menutext_vbuffer[(80*128)+16],128,"Settings",pos==4 ? SELECTION_COLOR : LIST_COLOR);	
	else
		amiga_textout(&menutext_vbuffer[(80*128)],128,"Back to menu",pos==4 ? SELECTION_COLOR : LIST_COLOR);	

	fastmemcpy(txr_menutext,menutext_vbuffer,128*128*2);

	switch(pos)
	{
		case 0:
		{	display_emu_menu_statustext("Load game state from VMU");	
		}break;

		case 1:
		{	display_emu_menu_statustext("Save game state to VMU");	
		}break;

		case 2:
		{	display_emu_menu_statustext("Reset emulation");	
		}break;

		case 3:
		{	display_emu_menu_statustext("Set rapid fire level");	
		}break;

		case 4:
		{	
			if(emu_autobooted==1)
				display_emu_menu_statustext("Go to settings");	
			else
				display_emu_menu_statustext("Get back to rom selection");	

		}break;
	
	}
}

void loop_display_menu(int cnt)
{

	while(--cnt>0) dc_draw_menu_frame();
}

void loop_display_status(int cnt)
{

	while(--cnt>0) dc_draw_status_frame();
}

//------------------------------------------------------------------------------
// Init emu 

void manage_emu_menu(void)
{
	uint8 mcont1 = 0;
	cont_cond_t cond1;	
	int max_pos=4;
	int ret;

	up_timer=0;
	down_timer=0;
	left_timer=0;
	right_timer=0;
	
	if (!mcont1)
	{	mcont1= maple_first_controller();}	

	if(mcont1)
	{
		if (cont_get_cond(mcont1, &cond1))
		{	return;}

		//-- test down key --
		ret=check_buttons(cond1.buttons,CONT_DPAD_DOWN);
		if( (ret==KEY_ASSERTED) || (down_timer>KEY_DELAY))
		{	
			if(emu_menu_pos<max_pos)emu_menu_pos++;
			display_emu_menu_screen(emu_menu_pos);
			down_timer=0;				
		}
		else
		{
			if(ret==KEY_PUSHED)
				down_timer++;			
			else
				down_timer=0;					
		}

		ret=check_buttons(cond1.buttons,CONT_DPAD_UP);			
		if ( (ret==KEY_ASSERTED) || (up_timer>KEY_DELAY) )
		{	if(emu_menu_pos>0)emu_menu_pos--;
			display_emu_menu_screen(emu_menu_pos);								
			up_timer=0;				
		}
		else
		{
			if(ret==KEY_PUSHED)
				up_timer++;			
			else
				up_timer=0;					
		}
		
		// rapid fire
		if(emu_menu_pos==3)
		{
       		ret=check_buttons(cond1.buttons,CONT_DPAD_RIGHT);			
    		if ( (ret==KEY_ASSERTED) || (right_timer>KEY_DELAY) )
    		{	//if(emu_menu_pos>0)emu_menu_pos--;
                if(rapidfire_level<MAX_RAPIDFIRE_LEVEL-1)
                {   rapidfire_level++;
                    rapidfire_flipflop=0;
                    rapidfire_cnt=0;                
                }
    		
    			display_emu_menu_screen(emu_menu_pos);								
    			right_timer=0;				
    		}
    		else
    		{
    			if(ret==KEY_PUSHED)
    				right_timer++;			
    			else
    				right_timer=0;					
    		}
    		
    		ret=check_buttons(cond1.buttons,CONT_DPAD_LEFT);			
    		if ( (ret==KEY_ASSERTED) || (left_timer>KEY_DELAY) )
    		{	if(rapidfire_level>0)
                {   rapidfire_level--;
                    rapidfire_flipflop=0;
                    rapidfire_cnt=0;
                }
    		
    			display_emu_menu_screen(emu_menu_pos);								
    			right_timer=0;				
    		}
    		else
    		{
    			if(ret==KEY_PUSHED)
    				left_timer++;			
    			else
    				left_timer=0;					
    		}				  
  
        }
		
		// effectue un choix		
		if(check_buttons(cond1.buttons,CONT_A)==KEY_ASSERTED )
		{
			switch(emu_menu_pos)
			{
				case 0:
				{
					display_emu_menu_statustext("Now loading from VMU");
					loop_display_menu(20);										
					switch(load_state(NULL))
					{
						case NO_VMU_FOUND:
						{	display_emu_menu_statustext("No VMU");
						}break;

						case NO_FILE:
						{	display_emu_menu_statustext("No state to load");
						}break;
						
						case LOAD_FAILED:
						{	display_emu_menu_statustext("Load failed");
						}break;

						case 0:
						{	display_emu_menu_statustext("State loaded");						
							emu_pause=0;
						}break;
					
					}
					loop_display_menu(60);					
					display_emu_menu_statustext("");						
					
				}break;

				case 1:
				{				   				
					display_emu_menu_statustext("Now saving to VMU");
					loop_display_menu(20);					
					switch(save_state(NULL))
					{
						case NO_VMU_FOUND:
						{	display_emu_menu_statustext("No VMU");
						}break;

						case NOT_ENOUGHT_BLOCKS:
						{	display_emu_menu_statustext("Not enought free blocks (25)");
						}break;

						case SAVE_FAILED:
						{	display_emu_menu_statustext("Save failed");
						}break;

						case 0:
						{	display_emu_menu_statustext("State saved");
							emu_pause=0;
						}break;
					
					}
					loop_display_menu(60);
					display_emu_menu_statustext("");
					
					

				}break;;

				case 2:
				{
					pcm_clear_buffer();					
					cpu_reset();
									
					emu_pause=0;
				}break;

                // rapid fire
				case 3:
				{	emu_pause=0;                    								
				
				}break;

				case 4:
				{	emu_pause=0;
					if(emu_autobooted)
					{	gui_state=OPTIONS_SCREEN;
     				    memcpy(&emu_cfg_o,&emu_cfg,sizeof(t_config));
					}
					else
						gui_state=ROMS_SCREEN;
				}break;
			}		
		}

		// retourne a lému
		if ( check_buttons(cond1.buttons,CONT_START)==KEY_ASSERTED && (cond1.ltrig>0 || cond1.rtrig>0))		    
		{	emu_pause=0;}	
		
		if(check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED)
		{	emu_pause=0;}

		if(check_buttons(cond1.buttons,CONT_START)==KEY_ASSERTED)
		{	emu_pause=0;}

		if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
			&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
		{	(*(void(**)())0x8c0000e0)(1);	
		}

		//if(check_buttons(cond1.buttons,CONT_X)==KEY_ASSERTED)
		//{	vid_screen_shot("/pc/gui.ppm");}

		


	}

}

//-----------------------------------------------------------------------------
//

// <heliophobe>
// Function to actually draw the crosshairs
// Pascal, you might want to take a closer look at this, I haven't used the PVR api before
// and it's been a while since I used the TA api so I might have messed something up.
// <heliophobe>
// Function to actually draw the crosshairs
// Pascal, you might want to take a closer look at this, I haven't used the PVR api before
// and it's been a while since I used the TA api so I might have messed something up.
__inline void dc_draw_crosshairs(void)
{
	pvr_vertex_t vert;
	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr; 
    
	pvr_poly_cxt_txr (&cxt, PVR_LIST_TR_POLY,PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED,32, 32, txr_crosshairs, PVR_FILTER_BILINEAR );		
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));;
	
	vert.flags = PVR_CMD_VERTEX;
	vert.x = screen_pos.x + (((input.lightgun_x >> 8) * screen_pos.w) / 256.0) - 16.0; //Update to make it 16 bit friendly
    vert.y = screen_pos.y + (((input.lightgun_y >> 8) * screen_pos.h) / 256.0) - 16.0; //Update to make it 16 bit friendly
    vert.z = 2.0f;
	vert.u = 0.0f;
	vert.v = 0.0f;
	vert.argb = vertex_color_value;
	vert.oargb = 0;	
	pvr_prim(&vert, sizeof(vert));

	vert.x = screen_pos.x + (((input.lightgun_x >> 8) * screen_pos.w) / 256.0) + 16.0;//Update to make it 16 bit friendly
    vert.y = screen_pos.y + (((input.lightgun_y >> 8) * screen_pos.h) / 256.0) - 16.0;//Update to make it 16 bit friendly
	vert.v = 0.0f;
	vert.z = 2.0f;
    vert.u = 1.0f;   
	pvr_prim(&vert, sizeof(vert));

	vert.x = screen_pos.x + (((input.lightgun_x >> 8) * screen_pos.w) / 256.0) - 16.0;//Update to make it 16 bit friendly
    vert.y = screen_pos.y + (((input.lightgun_y >> 8) * screen_pos.h) / 256.0) + 16.0;//Update to make it 16 bit friendly
    vert.z = 2;
    vert.u = 0.0f;
	vert.v = 1.0f;
	pvr_prim(&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.x = screen_pos.x + (((input.lightgun_x >> 8) * screen_pos.w) / 256.0) + 16.0;//Update to make it 16 bit friendly
    vert.y = screen_pos.y + (((input.lightgun_y >> 8) * screen_pos.h) / 256.0) + 16.0;//Update to make it 16 bit friendly
    vert.z = 2;
    vert.u = 1.0f;
    vert.v = 1.0f;
	pvr_prim(&vert, sizeof(vert));	    

    return;
}
// </heliophobe>




__inline dc_draw_emu_bg(void)
{
	pvr_vertex_t vert;

	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       
	
	if(cart.type==TYPE_GG)
	{	pvr_poly_cxt_txr (&poly, PVR_LIST_OP_POLY,PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED,256, 256, txr_emu, emu_cfg.bilinear ? PVR_FILTER_BILINEAR : PVR_FILTER_NONE);		
		pvr_poly_compile (&dc_pvr_emu_poly_hdr, &poly);
		pvr_list_begin(PVR_LIST_OP_POLY);
	}
	else
	{	pvr_poly_cxt_txr (&poly, PVR_LIST_TR_POLY,PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED,256, 256, txr_emu, emu_cfg.bilinear ? PVR_FILTER_BILINEAR : PVR_FILTER_NONE);		
		pvr_poly_compile (&dc_pvr_emu_poly_hdr, &poly);
		pvr_list_begin(PVR_LIST_TR_POLY);
	}

	pvr_prim(&dc_pvr_emu_poly_hdr, sizeof(pvr_poly_hdr_t));

	vert.flags = PVR_CMD_VERTEX;
	vert.x = screen_pos.x;
    vert.y = screen_pos.y;
    vert.z = 1.0f;
	vert.u = 0.0f;
	vert.v = 0.0f;
	vert.argb = vertex_color_value;
	vert.oargb = 0;	
	pvr_prim(&vert, sizeof(vert));

	vert.x = screen_pos.x + screen_pos.w;
    vert.y = screen_pos.y;	
	vert.z = 1;
    vert.u = 1.0f;   
	vert.v = 0.0f;
	pvr_prim(&vert, sizeof(vert));

	vert.x = screen_pos.x;
    vert.y = screen_pos.y + screen_pos.h;
    vert.z = 1;
    vert.u = 0.0f;
	vert.v = 1.0f;
	pvr_prim(&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.x = screen_pos.x + screen_pos.w;
    vert.y = screen_pos.y + screen_pos.h;
    vert.z = 1;
    vert.u = 1.0f;
    vert.v = 1.0f;
	pvr_prim(&vert, sizeof(vert));		
	
#ifdef DEBUG
	vid_border_color(255, 255, 0);
#endif

	// affiche le menu
	if(cart.type ==TYPE_GG)
	{	pvr_list_finish();
		pvr_list_begin(PVR_LIST_TR_POLY);
	}

}


//-----------------------------------------------------------------------------
//

__inline void dc_draw_menu_panel(void)
{
	pvr_vertex_t vert;
	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       

	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 128, 128, txr_menu, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

	vert.argb = vertex_color_value;
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = menu_pos.x;
    vert.y = menu_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = menu_pos.x + menu_pos.w;
    vert.y = menu_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = menu_pos.x;
    vert.y = menu_pos.y + menu_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = menu_pos.x + menu_pos.w;
    vert.y = menu_pos.y + menu_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));	


}


//-----------------------------------------------------------------------------
//

__inline void dc_draw_menu_text(void)
{
	pvr_vertex_t vert;

	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       

	// le text
	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 128, 128, txr_menutext, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = menutext_pos.x;
    vert.y = menutext_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = menutext_pos.x + menutext_pos.w;
    vert.y = menutext_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = menutext_pos.x;
    vert.y = menutext_pos.y + menutext_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = menutext_pos.x + menutext_pos.w;
    vert.y = menutext_pos.y + menutext_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}


//-----------------------------------------------------------------------------
//

__inline void dc_draw_menu_frame(void)
{
	   
#ifdef DEBUG
	vid_border_color(255, 0, 0);
	pvr_wait_ready();
	vid_border_color(0, 255, 0);	
#else
	pvr_wait_ready();			
#endif
			
	pvr_scene_begin();

	dc_draw_emu_bg();	
	dc_draw_menu_panel();	

	draw_status();
	dc_draw_menu_text();

	pvr_list_finish();
	pvr_scene_finish();
}

__inline void dc_draw_status_frame(void)
{
#ifdef DEBUG
	vid_border_color(255, 0, 0);
	pvr_wait_ready();
	vid_border_color(0, 255, 0);	
#else
	pvr_wait_ready();			
#endif
			
	pvr_scene_begin();

	// list en TR ou op selon GG ou sms
	dc_draw_emu_bg();	

	draw_status();

	pvr_list_finish();
	pvr_scene_finish();
}

//------------------------------------------------------------------------------
// Draw game

void dc_drawframe(void)
{
	pvr_vertex_t vert;

	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       


#ifdef DEBUG
	vid_border_color(255, 0, 0);
	pvr_wait_ready();
	vid_border_color(0, 255, 0);	
#else
	pvr_wait_ready();
#endif
			
	pvr_scene_begin();

	if(cart.type==TYPE_SMS)
	{	pvr_poly_cxt_txr (&poly, PVR_LIST_TR_POLY,PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED,256, 256, txr_emu, emu_cfg.bilinear ? PVR_FILTER_BILINEAR : PVR_FILTER_NONE);		
		pvr_poly_compile (&dc_pvr_emu_poly_hdr, &poly);
		pvr_list_begin(PVR_LIST_TR_POLY);
	}
	else
	{	pvr_poly_cxt_txr (&poly, PVR_LIST_OP_POLY,PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED,256, 256, txr_emu, emu_cfg.bilinear ? PVR_FILTER_BILINEAR : PVR_FILTER_NONE);		
		pvr_poly_compile (&dc_pvr_emu_poly_hdr, &poly);
		pvr_list_begin(PVR_LIST_OP_POLY);
	}

	pvr_prim(&dc_pvr_emu_poly_hdr, sizeof(pvr_poly_hdr_t));

	vert.flags = PVR_CMD_VERTEX;
	vert.x = screen_pos.x;
    vert.y = screen_pos.y;
    vert.z = 1.0f;
	vert.u = 0.0f;
	vert.v = 0.0f;
	vert.argb = vertex_color_value;
	vert.oargb = 0;	
	pvr_prim(&vert, sizeof(vert));

	vert.x = screen_pos.x + screen_pos.w;
    vert.y = screen_pos.y;
	vert.v = 0.0f;
	vert.z = 1;
    vert.u = 1.0f;   
	pvr_prim(&vert, sizeof(vert));

	vert.x = screen_pos.x;
    vert.y = screen_pos.y + screen_pos.h;
    vert.z = 1;
    vert.u = 0.0f;
	vert.v = 1.0f;
	pvr_prim(&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.x = screen_pos.x + screen_pos.w;
    vert.y = screen_pos.y + screen_pos.h;
    vert.z = 1;
    vert.u = 1.0f;
    vert.v = 1.0f;
	pvr_prim(&vert, sizeof(vert));	
 
	if(rom_flag & fLIGHT_PHASER) 
    {
        
		dc_draw_crosshairs();
	}

	pvr_list_finish();
	pvr_scene_finish();

#ifdef DEBUG
	vid_border_color(255, 255, 0);
#endif

}

//------------------------------------------------------------------------------
// DC inputs

void dc_getinput(void)
{
	int		p, u;
	cont_cond_t cond1,cond2;
	uint32	func;

	input.pad[0]=input.pad[1]=0;
	input.system=0;

	// get joypad addr
	if(!pads_adr[0] || !pads_adr[1])
	{
		for (p=0; p<2; p++)
		{
			for (u=0; u<MAPLE_UNIT_COUNT; u++) 
			{
				func = maple_device_func(p, u);
				if (func & MAPLE_FUNC_CONTROLLER)
				{	pads_adr[p]= maple_addr(p, u);					
				}
			}
		}
	}

	if(pads_adr[0])
	{	
		if (cont_get_cond(pads_adr[0], &cond1))
		{	pads_adr[0]=0;
			return;
		}
		
// <heliophobe> 
// Handler to get input for the paddle. 
// Sort of an ugly define that doesn't belong here but it got the just done.
		#define CLAMP(v, l,h) if(v < l) v=l; else if (v > h) v=h;
		input.analog_x = cond1.joyx; 
		input.analog_y = cond1.joyy; 
		if(rom_flag & fABSOLUTE_PADDLE) {
		  input.paddle = input.analog_x << 8;   //Update - scaled this up by 256 for analog
		}
		else {
  	    	input.paddle += ((input.analog_x - 128) << 8 )  / (20 - emu_cfg.paddle_sensivity); //Update - scaled this up by 256 for analog
 	    }
        CLAMP(input.paddle, 0, 65535);
		input.lightgun_x += ((input.analog_x - 128) << 8 )  / (20 - emu_cfg.paddle_sensivity); //Update - scaled this up by 256 for analog
		input.lightgun_y += ((input.analog_y - 128) << 8 )  / (20 - emu_cfg.paddle_sensivity); //Update - scaled this up by 256 for analog
		CLAMP(input.lightgun_x,0,65535);
		CLAMP(input.lightgun_y,0,49151);		
// </heliophobe>	


		if( cond1.ltrig && cond1.rtrig)
		{	
			if(emu_autobooted==0)
				gui_state=ROMS_SCREEN;					
		}

		if (!(cond1.buttons & CONT_DPAD_UP))
		{	input.pad[0] |= INPUT_UP;}
		
		if (!(cond1.buttons & CONT_DPAD_DOWN))
		{	input.pad[0] |= INPUT_DOWN;}
		
		if (!(cond1.buttons & CONT_DPAD_LEFT))
		{	input.pad[0] |= INPUT_LEFT;}
		
		if (!(cond1.buttons & CONT_DPAD_RIGHT))
		{	input.pad[0] |= INPUT_RIGHT;}
		
		if(emu_cfg.invert_buttons==0)
		{
			if (!(cond1.buttons & CONT_B))
			{	input.pad[0] |= INPUT_BUTTON1;}
			
			if (!(cond1.buttons & CONT_A))
			{	input.pad[0] |= INPUT_BUTTON2;}
		}
		else
		{
			if (!(cond1.buttons & CONT_A))
			{	input.pad[0] |= INPUT_BUTTON1;}
			
			if (!(cond1.buttons & CONT_B))
			{	input.pad[0] |= INPUT_BUTTON2;}		
		}

		if ( !(cond1.buttons & CONT_START) && !cond1.rtrig && !cond1.ltrig )
		{	 
			if(cart.type==TYPE_SMS)
				input.system |=INPUT_PAUSE;
			else
				input.system |=INPUT_START;
		}			
		
		if( !(cond1.buttons & CONT_X) )
		{
            if(rapidfire_level==0)
            {   input.pad[0] |= INPUT_BUTTON1 | INPUT_BUTTON2;}
            else
            {                                   
                if(rapidfire_flipflop==0)
                {
                    if(emu_cfg.invert_buttons==0)
             		{                   
                    	input.pad[0] &= ~INPUT_BUTTON2;}
                    else
                    {   input.pad[0] &= ~INPUT_BUTTON1; }                
                }
                else                 
                {   if(emu_cfg.invert_buttons==0)
             		{	               
                        input.pad[0] |= INPUT_BUTTON2;}
                    else
                    {   input.pad[0] |= INPUT_BUTTON1; }                
                }                
            }                    
		}
        
        if(rapidfire_level!=0)
        {
            if( !(cond1.buttons & CONT_Y) )
    		{
                if(rapidfire_flipflop==0)
                {
                    if(emu_cfg.invert_buttons==0)
             		{                   
                    	input.pad[0] &= ~INPUT_BUTTON1;}
                    else
                    {   input.pad[0] &= ~INPUT_BUTTON2; }                
                }
                else                 
                {   if(emu_cfg.invert_buttons==0)
             		{	               
                        input.pad[0] |= INPUT_BUTTON1;}
                    else
                    {   input.pad[0] |= INPUT_BUTTON2; }                
                }                
    		}    		            
        }	
        	

		if ( check_buttons(cond1.buttons,CONT_START)==KEY_ASSERTED && (cond1.ltrig>0 || cond1.rtrig>0))		    
		{
			emu_pause^=1;
			emu_menu_pos=0;
			display_emu_menu_screen(0);			
			pcm_clear_buffer();
		}	
		
		if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
			&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
		{	(*(void(**)())0x8c0000e0)(1);	
		}				
		
		
	}

	if(pads_adr[1])
	{			
		if (cont_get_cond(pads_adr[1], &cond2))
		{	pads_adr[1]=0;
			return;
		}	
		
		if (!(cond2.buttons & CONT_DPAD_UP))
		{	input.pad[1] |= INPUT_UP;}
		
		if (!(cond2.buttons & CONT_DPAD_DOWN))
		{	input.pad[1] |= INPUT_DOWN;}
		
		if (!(cond2.buttons & CONT_DPAD_LEFT))
		{	input.pad[1] |= INPUT_LEFT;}
		
		if (!(cond2.buttons & CONT_DPAD_RIGHT))
		{	input.pad[1] |= INPUT_RIGHT;}
		
		if(emu_cfg.invert_buttons==0)
		{
			if (!(cond2.buttons & CONT_B))
			{	input.pad[1] |= INPUT_BUTTON1;}
			
			if (!(cond2.buttons & CONT_A))
			{	input.pad[1] |= INPUT_BUTTON2;}
		}
		else
		{
			if (!(cond2.buttons & CONT_A))
			{	input.pad[1] |= INPUT_BUTTON1;}
			
			if (!(cond2.buttons & CONT_B))
			{	input.pad[1] |= INPUT_BUTTON2;}		
		}
				
		if ( !(cond2.buttons & CONT_START) && !cond2.rtrig && !cond2.ltrig )
		{	 
			if(cart.type==TYPE_SMS)
				input.system |=INPUT_PAUSE;
			else
				input.system |=INPUT_START;
		}			

		if( !(cond2.buttons & CONT_X) )
		{
			if(rapidfire_level==0)
            {   input.pad[1] |= INPUT_BUTTON1 | INPUT_BUTTON2;}
            else
            {   
                if(rapidfire_flipflop==0)
                {
                    if(emu_cfg.invert_buttons==0)
             		{                   
                    	input.pad[1] &= ~INPUT_BUTTON2;}
                    else
                    {   input.pad[1] &= ~INPUT_BUTTON1; }                
                }
                else                 
                {   if(emu_cfg.invert_buttons==0)
             		{	               
                        input.pad[1] |= INPUT_BUTTON2;}
                    else
                    {   input.pad[1] |= INPUT_BUTTON1; }                
                }                
            }             
		}
		
		if(rapidfire_level!=0)
        {
            if( !(cond2.buttons & CONT_Y) )
    		{
                if(rapidfire_flipflop==0)
                {
                    if(emu_cfg.invert_buttons==0)
             		{                   
                    	input.pad[1] &= ~INPUT_BUTTON1;}
                    else
                    {   input.pad[1] &= ~INPUT_BUTTON2; }                
                }
                else                 
                {   if(emu_cfg.invert_buttons==0)
             		{	               
                        input.pad[1] |= INPUT_BUTTON1;}
                    else
                    {   input.pad[1] |= INPUT_BUTTON2; }                
                }                
    		}    		                		    		
        }			
			
	}
	
	if(rapidfire_level>0)
	{	if((rapidfire_cnt % rapidfire_table[rapidfire_level])==0)
        rapidfire_flipflop^=1;
                
       	rapidfire_cnt++; 
    }


}

//-----------------------------------------------------------------------------
// Copy frame 


/* n must be multiple of 64 */
void dc_sq_cpy(void *dest, void *src, int n)
{

  uint32 *sq;
  uint32 *d, *s;
  
  d = (uint32 *)(0xe0000000 | (((uint32)dest) & 0x03ffffe0));
  s = (uint32 *)(src);
  
  
  *((volatile unsigned int*)0xFF000038) = ((((uint32)dest)>>26)<<2)&0x1c;
  *((volatile unsigned int*)0xFF00003C) = ((((uint32)dest)>>26)<<2)&0x1c;
  
  n >>= 6;
  while (n--) 
  {
    // sq0 
    sq = d;
    *sq++ = *s++; *sq++ = *s++;
    *sq++ = *s++; *sq++ = *s++;
    *sq++ = *s++; *sq++ = *s++;
    *sq++ = *s++; *sq++ = *s++;
    asm("pref @%0" : : "r" (d));
    d += 8;
    
    // sq1 
    sq = d;
    *sq++ = *s++; *sq++ = *s++;
    *sq++ = *s++; *sq++ = *s++;
    *sq++ = *s++; *sq++ = *s++;
    *sq++ = *s++; *sq++ = *s++;
    asm("pref @%0" : : "r" (d));
    d += 8;
  }

  *((uint32 *)(0xe0000000)) = 0;
  *((uint32 *)(0xe0000020)) = 0;

}

void dc_txr_dma_cpy(void *dest, void *src, int n)
{
	long i = n/32;
	char	*ptr=dest;


	do 
	{
		asm("ocbp @%0" : : "r" (ptr));
		ptr += 32;
	} while (-- i);

		pvr_txr_load_dma(dest, src, n,1);

}


inline void dc_copyframe(void)
{
	int count= cart.type == TYPE_SMS ? 0xc00 : 0x900;	
	unsigned int *p=(unsigned int*) emu_vbuffer;
	unsigned int *ptsq=(unsigned int*) ((((unsigned int) txr_emu) & 0x03ffffe0)| 0xe0000000);
			
	*((volatile unsigned int*)0xFF000038) = (((unsigned int)txr_emu>>26)<<2)&0x1c;
	*((volatile unsigned int*)0xFF00003C) = (((unsigned int)txr_emu>>26)<<2)&0x1c;
				
	while (count-- >0)
	{  
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
		*(ptsq++)=*(p++);
					
		asm("pref @%0" : : "r" (ptsq-1));
	}	
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

void system_load_sram(void)
{
	memset(sms.sram,0,0x8000);
	memset(old_sram,0,0x8000);
	sram_load_from_vmu (NULL);	
}


//-----------------------------------------------------------------------------
// load a rom

int load_rom(char *filename)
{
    unsigned int fd;
    int size;
	int i=0;
	int start=0;
	    
    fd = fs_open(filename, O_RDONLY);
	if(fd==0)
	{	fs_close ( fd );
		return 0;
	}

    size = fs_total(fd);
			    	
	/* Don't load games smaller than 8k */
	if(size < 0x2000) return (0);
   
	/* Take care of image header, if present */
    if((size / 512) & 1)
    {	size -= 512;
        start=512;
    }
    cart.pages = (size / 0x4000);
   	
	fs_seek(fd,start,0);
	if(fs_read(fd, &cart.rom[0], size)!=size)
	{	fs_close ( fd );
		return 0;	
	}

	fs_close (fd);
		    
    /* Figure out game image type */    
	if(stricmp(my_strrchr(filename, '.'),".gg")==0)
		cart.type=TYPE_GG;	
	else
		cart.type=TYPE_SMS;	

	// compute CRC
	rom_crc=crc32(0,&cart.rom[0],size);

	rom_flag=0;
	while(sms_db[i].name!=NULL && rom_flag==0)
	{		
		if(sms_db[i].crc==	rom_crc)
		{	
            rom_flag = sms_db[i].flag;			
		}
		i++;
	}

	system_fix_rom();
	    
    return (1);
}

//------------------------------------------------------------------------------
// Start Emulator

int start_emulator(void)
{
	// init du bitmap
	memset(&bitmap, 0, sizeof(t_bitmap));
    bitmap.width  = 256;
    bitmap.height = 256;
    bitmap.depth  = 16;
    bitmap.pitch  = (bitmap.width * (bitmap.depth >> 3));
    bitmap.data   = (unsigned char *) (emu_vbuffer);

	//	
	sms_tv = cart.type==TYPE_SMS ? emu_cfg.sms_tv : TV_NTSC;			

	if(rom_flag!=0 && cart.type==TYPE_SMS && emu_cfg.sms_tv==TV_NTSC)
	{
		if(rom_flag & fFORCE_PAL) 
		{	sms_tv = TV_NTSC_ENHANCED;}
	}
	
	system_init(SAMPLE_RATE,sms_tv);

	sms.country = emu_cfg.country;
	sms_emulate_voice =  (sms_tv ==TV_NTSC_ENHANCED) ? 0 : 1;	
    sms.use_fm = 0;
	snd.enabled= 1;
    snd.callback = NULL;

	loaded_tv_mode=emu_cfg.sms_tv;
	
	// load sram
	system_load_sram();
	
	return 1;
}

//------------------------------------------------------------------------------
//
__inline void  update_overscan(void)
{
	int idx=BACKDROP_COLOR;
	int r,g,b;
	
	r=bitmap.pal.color[idx][0];
	g=bitmap.pal.color[idx][1];
	b=bitmap.pal.color[idx][2];	

	unsigned int color = r<<16 |  g<<8 | b;
	// overscan
	*((volatile unsigned int*)0xa05f8040) = color;

	// Backdrop
	pvr_state.bg_color=color;	
	

}

//------------------------------------------------------------------------------
//

void check_and_save_sram()
{
	// save to vum 
	if(rom_flag & fNO_SRAM) 
	{	sms.save=0;}


	if (sms.save==1) 
	{	
		int i=0;
		int check=0;

		while(i<0x8000 && check==0)
		{
		

			if(old_sram[i]!=sms.sram[i])
			{	check=1;
			
			}
			i++;
		}

		if(check==1)
		{	
		   if(emu_cfg.auto_save==1)                
		   {
    		
    			display_emu_menu_statustext("Now saving SRAM to VMU");
    			loop_display_status(20);
    
    			switch(sram_save_to_vmu (NULL))
    			{
    				case NO_VMU_FOUND:
    				{	display_emu_menu_statustext("No VMU");
    				}break;
    
    				case NOT_ENOUGHT_BLOCKS:
    				{	display_emu_menu_statustext("Not enought free blocks (18)");
    				}break;
    
    				case SAVE_FAILED:
    				{	display_emu_menu_statustext("Save failed");
    				}break;
    
    				case 0:
    				{	display_emu_menu_statustext("SRAM saved");
    					
    				}break;
    			}
    			loop_display_status(60);					
    			display_emu_menu_statustext("");
			}
		}

	}

}



//------------------------------------------------------------------------------
// Main entry

void dc_emu_start()
{
	int set_pal=0;
	int size =  cart.type==TYPE_GG ? 256*144*2 :  256*192*2;

	if(emu_join==0)
	{	fastmemset(emu_vbuffer,0x0,256*256*2);		
		fastmemset(txr_emu,0,256*256*2);
		loaded_tv_mode=emu_cfg.sms_tv;
	}
	
	if(cart.type==TYPE_SMS)
	{		
		if(loaded_tv_mode==TV_PAL)
		{
			vid_init(DM_640x480_PAL_IL, PM_RGB565);			
			set_pal=1;
		}		
	}	
	get_screen_pos();

	//--< Fade In >--
	vertex_color=0.0f;	
	while(vertex_color<(1.0f - 0.025f))
	{	vertex_color+=0.025f;
		vertex_color_value=PVR_PACK_COLOR(0.5, vertex_color, vertex_color, vertex_color);
		dc_drawframe();
	}
	vertex_color=1.0f;
	vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
	
	//-- start emulator --	
	if(emu_join==0)
	{	start_emulator();}		

	memcpy(old_sram,sms.sram,0x8000);


	// init des coords du menu	
	menu_pos.w = 256;
	menu_pos.h = 256;
	menu_pos.x = ((640 / 2) - (menu_pos.w/2));
	menu_pos.y =  ((480 / 2) - (menu_pos.h/2));	

	menutext_pos.w=256;
	menutext_pos.h=256;
	menutext_pos.x=222;
	menutext_pos.y=160-16-8;
	
	status_pos.x=10;
	status_pos.y=480-60;
	status_pos.w = 512;
	status_pos.h = 32;
	emu_menu_pos=0;
	emu_pause=0;

	display_emu_menu_screen(emu_menu_pos);
		
	// init du son
	pcm_init();	
	
		
	while(gui_state==EMU_SCREEN)
	{			
		dc_getinput();	

		if(emu_pause==0)
		{
			render_pcm_ntsc();
			sms_frame(0);
			dcache_flush_range(emu_vbuffer,size);
			pvr_txr_load_dma(emu_vbuffer, txr_emu, size,1);
		
			
#ifndef DEBUG
			if(cart.type==TYPE_SMS && emu_cfg.paint_backdrop) update_overscan();
	
#endif
			dc_drawframe();			
		}
		else
		{
			manage_emu_menu();
			dc_draw_menu_frame();		
		}	

	}
	pcm_clear_buffer();
	pcm_close();
	
	check_and_save_sram();

	
	//---< rest Overscan color >---
	*((volatile unsigned int*)0xa05f8040) = 0;
	pvr_state.bg_color=0;

	//---< Fade Out >--
	while(vertex_color>0.025f)
	{	vertex_color-=0.025f;
		vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
		dc_drawframe();
	}
	vertex_color_value=0;

	if(cart.type==TYPE_SMS && set_pal==1)
		vid_init(DM_640x480_NTSC_IL, PM_RGB565);

}


