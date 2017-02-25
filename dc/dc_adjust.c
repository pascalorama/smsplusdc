
#include "dc_shared.h"


t_screenpos cur_screen_pos;
t_screenpos max_screen_pos;

uint16		adjust_vbuffer[256 * 256] __attribute__ ((aligned (32)));
pvr_ptr_t	txr_adjust;
uint8		adjust_type=0;

//-----------------------------------------------------------------------------

void gui_adjust_init()
{
	txr_adjust = pvr_mem_malloc (256 * 256 * 2);		
	
}

//-----------------------------------------------------------------------------

__inline void dc_draw_adjust(void)
{
	pvr_vertex_t vert;

	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       

	// le text
	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256, 256, txr_adjust, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = cur_screen_pos.x;
    vert.y = cur_screen_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = cur_screen_pos.x + cur_screen_pos.w;
    vert.y = cur_screen_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = cur_screen_pos.x;
    vert.y = cur_screen_pos.y + cur_screen_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = cur_screen_pos.x + cur_screen_pos.w;
    vert.y = cur_screen_pos.y + cur_screen_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}

//-----------------------------------------------------------------------------

void dc_draw_adjust_frame()
{
	
	pvr_wait_ready();	
	pvr_scene_begin();

	pvr_list_begin(PVR_LIST_TR_POLY);	
	dc_draw_adjust();
	pvr_list_finish();

	pvr_scene_finish();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void gui_adjust_screen(void)
{
	cont_cond_t cond1;		
	uint8 mcont1 = 0;
	int ret=0;
	int cycle_screen=0;
	uint16 cListColor=LIST_COLOR;
	uint16 cSelectionColor=SELECTION_COLOR;

	// load de la texture	
	if(adjust_type==TYPE_GG)
	{	
		if(strlen(sel_file)==0 || cart.type!=TYPE_GG)
		{	LoadPcx2Buffer("/rd/adjustgg.pcx",adjust_vbuffer);}
		else
		{	fastmemcpy(adjust_vbuffer,emu_vbuffer,256*256*2);
			cSelectionColor=STATUS_COLOR;
			cListColor=STATUS_COLOR;
		}

		memcpy(&cur_screen_pos,&emu_cfg.gg_pos,sizeof(t_screenpos));
		default_gg_pos(&max_screen_pos);

		amiga_textout(&adjust_vbuffer[(16*256)+12],256,"Screen adjustment",cSelectionColor);
		amiga_textout(&adjust_vbuffer[(46*256)+4],256, "Digital adjust pos.",cListColor);
		amiga_textout(&adjust_vbuffer[(66*256)+4],256,"Analog adjust size",cListColor);
		amiga_textout(&adjust_vbuffer[(86*256)+32],256 ,"(A) Confirm",cListColor);
		amiga_textout(&adjust_vbuffer[(106*256)+32],256 ,"(B) Cancel",cListColor);
		amiga_textout(&adjust_vbuffer[(126*256)+32],256,"(X) Default",cListColor);

	}
	else
	{
		
		if(strlen(sel_file)==0 || cart.type!=TYPE_SMS)
		{	LoadPcx2Buffer("/rd/adjustsms.pcx",adjust_vbuffer);}
		else
		{	fastmemcpy(adjust_vbuffer,emu_vbuffer,256*256*2);	
			cSelectionColor=STATUS_COLOR;
			cListColor=STATUS_COLOR;
		
		}
		

		if(emu_cfg.sms_tv==TV_PAL)
		{	memcpy(&cur_screen_pos,&emu_cfg.pal_sms_pos,sizeof(t_screenpos));		
			default_pal_sms_pos(&max_screen_pos);
		}
		else
		{	memcpy(&cur_screen_pos,&emu_cfg.ntsc_sms_pos,sizeof(t_screenpos));
			default_ntsc_sms_pos(&max_screen_pos);
		}

		// print text
		amiga_textout(&adjust_vbuffer[(46*256)+64],256,"Screen adjustment",cSelectionColor);
		amiga_textout(&adjust_vbuffer[(76*256) +20],256,"Digital pad adjust position",cListColor);
		amiga_textout(&adjust_vbuffer[(96*256)+40],256,"Analog pad adjust size",cListColor);
		amiga_textout(&adjust_vbuffer[(116*256)+4] ,256,"(A)Confirm (B)Cancel (X)Default",cListColor);
	}
	fastmemcpy(txr_adjust,adjust_vbuffer,256*256*2);

	if(emu_cfg.sms_tv==TV_PAL && adjust_type==TYPE_SMS)
	{	vid_init(DM_640x480_PAL_IL, PM_RGB565);			
	}

	//--< Fade In >--
	vertex_color=0.0f;	
	while(vertex_color<(1.0f - 0.025f))
	{	vertex_color+=0.025f;
		vertex_color_value=PVR_PACK_COLOR(0.5, vertex_color, vertex_color, vertex_color);
		dc_draw_adjust_frame();
	}
	vertex_color=1.0f;
	vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);	
		
	// boucle principale
	while(gui_state==ADJUST_SCREEN)
	{
		if (!mcont1)
		{	mcont1= maple_first_controller();}	

		if(mcont1)
		{
			if(cont_get_cond(mcont1, &cond1))
			{	return;}

			//-- test down key --
			if(check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED)			
			{	gui_state=OPTIONS_SCREEN;
			}

			// digital pad			
			if( !(cond1.buttons&CONT_DPAD_UP) )
			{	
				if(cur_screen_pos.y>-1) cur_screen_pos.y--;			
			}

			if( !(cond1.buttons&CONT_DPAD_DOWN) )
			{
				cur_screen_pos.y++;			
			}

			if( !(cond1.buttons&CONT_DPAD_LEFT) )
			{
				if(cur_screen_pos.x>-1)
					cur_screen_pos.x--;			
			}

			if( !(cond1.buttons&CONT_DPAD_RIGHT) )
			{
				cur_screen_pos.x++;			
			}

			if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
				&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
			{	(*(void(**)())0x8c0000e0)(1);	
			}


			// analog pad
			if(cond1.joyx>128+64)
			{	if(cur_screen_pos.w<max_screen_pos.w)
					cur_screen_pos.w++;
			}
		
			if(cond1.joyx<128-64)
			{	
				if(cur_screen_pos.w>256);
					cur_screen_pos.w--;
			
			}

			if(cond1.joyy>128+64)
			{	if(cur_screen_pos.h<max_screen_pos.h)
					cur_screen_pos.h++;
			}
		
			if(cond1.joyy<128-64)
			{	
				if(cur_screen_pos.h>256);
					cur_screen_pos.h--;
			}
		
			// Action
			if(check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED)			
			{	gui_state=OPTIONS_SCREEN;
			}

			// Confirm
			if(check_buttons(cond1.buttons,CONT_A)==KEY_ASSERTED)			
			{	gui_state=OPTIONS_SCREEN;

				if(adjust_type==TYPE_GG)
				{	memcpy(&emu_cfg.gg_pos,&cur_screen_pos,sizeof(t_screenpos));
				}
				else
				{	if(emu_cfg.sms_tv==TV_PAL)
					{	memcpy(&emu_cfg.pal_sms_pos,&cur_screen_pos,sizeof(t_screenpos));}
					else
					{	memcpy(&emu_cfg.ntsc_sms_pos,&cur_screen_pos,sizeof(t_screenpos));}
				}
			}
			
			// Default
			if(check_buttons(cond1.buttons,CONT_X)==KEY_ASSERTED)			
			{	
				if(adjust_type==TYPE_GG)
				{	default_gg_pos(&cur_screen_pos);
				}
				else
				{	if(emu_cfg.sms_tv==TV_PAL)
					{	default_pal_sms_pos(&cur_screen_pos);}
					else
					{	default_ntsc_sms_pos(&cur_screen_pos);}
				}
			}

			// cycle size
			if(check_buttons(cond1.buttons,CONT_Y)==KEY_ASSERTED)	
			{
				if(adjust_type==TYPE_GG)
				{
					if(cycle_screen==3)
					{	default_gg_pos(&cur_screen_pos);													
					}
					else
					{	cur_screen_pos.w = 256 * (cycle_screen+1);
						cur_screen_pos.h = 256 * (cycle_screen+1);

						cur_screen_pos.x = (640/2) - ((160*(cycle_screen+1))/2);
						cur_screen_pos.y = (480/2) - ((144*(cycle_screen+1))/2);
					}
					if(cycle_screen<3)
						cycle_screen++;
					else
						cycle_screen=0;
				}
				else
				{
					if(cycle_screen==2)
					{	if(emu_cfg.sms_tv==TV_PAL)
						{	default_pal_sms_pos(&cur_screen_pos);}
						else
						{	default_ntsc_sms_pos(&cur_screen_pos);}
					}
					else
					{	cur_screen_pos.w = 256 * (cycle_screen+1);
						cur_screen_pos.h = 256 * (cycle_screen+1);

						cur_screen_pos.x = (640/2) - (cur_screen_pos.w/2);
						cur_screen_pos.y = (480/2) - ((192*(cycle_screen+1))/2);

					}
					if(cycle_screen<2)
						cycle_screen++;
					else
						cycle_screen=0;
				
				}
				
			}


		}	
		dc_draw_adjust_frame();
	}

	//---< Fade Out >--
	while(vertex_color>0.025f)
	{	vertex_color-=0.025f;
		vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
		dc_draw_adjust_frame();
	}
	vertex_color_value=0;

	if(emu_cfg.sms_tv==TV_PAL  && adjust_type==TYPE_SMS)
		vid_init(DM_640x480_NTSC_IL, PM_RGB565);

}
