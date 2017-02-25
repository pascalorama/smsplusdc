
#include "dc_shared.h"

//-----------------------------------------------------------------------------

t_screenpos settings_pos;
t_screenpos games_pos;
t_screenpos credits_pos;

pvr_ptr_t	txr_title;
pvr_ptr_t	txr_settings;
pvr_ptr_t	txr_games;
pvr_ptr_t	txr_credits;

unsigned char title_pos=1;

//-----------------------------------------------------------------------------

void init_title_texture(void)
{	
	txr_settings = pvr_mem_malloc(256*256*2);
	txr_games = pvr_mem_malloc(256*256*2);
	txr_credits = pvr_mem_malloc(256*256*2);
}


void draw_games(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256, 256, txr_games, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    if(title_pos==1)
		vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
	else
		vert.argb = PVR_PACK_COLOR(vertex_color, 0.6f, 0.6f, 0.6f);
	vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = games_pos.x;
    vert.y = games_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = games_pos.x + games_pos.w;
    vert.y = games_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = games_pos.x;
    vert.y = games_pos.y + games_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = games_pos.x + games_pos.w;
    vert.y = games_pos.y + games_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));			
}

//-----------------------------------------------------------------------------

void draw_settings(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256, 256, txr_settings, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));
	
	if(title_pos==0)
		vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
	else
		vert.argb = PVR_PACK_COLOR(vertex_color, 0.6f, 0.6f, 0.6f);

    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = settings_pos.x;
    vert.y = settings_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = settings_pos.x + settings_pos.w;
    vert.y = settings_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = settings_pos.x;
    vert.y = settings_pos.y + settings_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = settings_pos.x + settings_pos.w;
    vert.y = settings_pos.y + settings_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));			
}

void draw_credits(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256, 256, txr_credits, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    if(title_pos==2)
		vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
	else
		vert.argb = PVR_PACK_COLOR(vertex_color, 0.6f, 0.6f, 0.6f);
	
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = credits_pos.x;
    vert.y = credits_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = credits_pos.x + credits_pos.w;
    vert.y = credits_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = credits_pos.x;
    vert.y = credits_pos.y + credits_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = credits_pos.x + credits_pos.w;
    vert.y = credits_pos.y + credits_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));			
}


//-----------------------------------------------------------------------------

void draw_titlescreen(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, txr_bg, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value;
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = 0;
    vert.y = 0;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = 640;
    vert.y = 0;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = 0;
    vert.y = 640;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = 640;
    vert.y = 640;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}


//-----------------------------------------------------------------------------

void dc_draw_title_frame()
{
	
	pvr_wait_ready();	
	pvr_scene_begin();

	pvr_list_begin(PVR_LIST_OP_POLY);	
	draw_titlescreen();
	pvr_list_finish();
	pvr_list_begin(PVR_LIST_TR_POLY);	
	draw_credits();
	draw_games();
	draw_settings();
	pvr_list_finish();

	pvr_scene_finish();

}

//-----------------------------------------------------------------------------

void dc_draw_credits_frame()
{
	
	pvr_wait_ready();	
	pvr_scene_begin();

	pvr_list_begin(PVR_LIST_OP_POLY);	
	draw_titlescreen();
	pvr_list_finish();
	
	pvr_scene_finish();

}

//-----------------------------------------------------------------------------

void gui_title_screen(void)
{
	cont_cond_t cond1;		
	uint8 mcont1 = 0;
	int ret=0;
	uint16 t[256 * 256]		__attribute__ ((aligned (32)));

	int settings_cnt=0;
	int games_cnt=0;
	int credits_cnt=0;

	int frame_timer=0;
	char filename[256];
	
	// load
    png_to_texture("/rd/title.png", txr_bg,PNG_NO_ALPHA);
	
	LoadPcx2Buffer("/rd/credits0000.pcx",t);
	fastmemcpy((uint32*) txr_credits,(uint32*) t,256*256*2);

	LoadPcx2Buffer("/rd/games0000.pcx",t);
	fastmemcpy((uint32*) txr_games,(uint32*) t,256*256*2);

	LoadPcx2Buffer("/rd/soucoupe0000.pcx",t);
	fastmemcpy((uint32*) txr_settings,(uint32*) t,256*256*2);
	
	settings_pos.x=12;
	settings_pos.y=250;
	settings_pos.w=320;
	settings_pos.h=320;

	credits_pos.x=450;
	credits_pos.y=268;
	credits_pos.w=320;
	credits_pos.h=320;

	games_pos.x=205;
	games_pos.y=278;
	games_pos.w=320;
	games_pos.h=320;
	
	//--< Fade In >--
	vertex_color=0.0f;	
	while(vertex_color<(1.0f - 0.025f))
	{	vertex_color+=0.025f;
		vertex_color_value=PVR_PACK_COLOR(0.5, vertex_color, vertex_color, vertex_color);
		dc_draw_title_frame();
	}
	vertex_color=1.0f;
	vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);	
		
	// boucle principale
	while(gui_state==MAIN_SCREEN)
	{
		if (!mcont1)
		{	mcont1= maple_first_controller();}	

		if(mcont1)
		{
			if(cont_get_cond(mcont1, &cond1))
			{	return;}

			if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
				&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
			{	(*(void(**)())0x8c0000e0)(1);	
			}


			if(check_buttons(cond1.buttons,CONT_DPAD_RIGHT)==KEY_ASSERTED)			
			{	if(title_pos<2)title_pos++;				
			}

			if(check_buttons(cond1.buttons,CONT_DPAD_LEFT)==KEY_ASSERTED)			
			{	if(title_pos>0)title_pos--;				
			}			

			if(check_buttons(cond1.buttons,CONT_A)==KEY_ASSERTED )			
			{	

				switch(title_pos)
				{
					case 0:
					{	gui_state=OPTIONS_SCREEN;
                        memcpy(&emu_cfg_o,&emu_cfg,sizeof(t_config));
						options_pos=0;
					}break;

					case 1:
					{	gui_state=ROMS_SCREEN;
					}break;	
					
					case 2:
					{	gui_state=CREDITS_SCREEN;
					}break;
				}								
			}
			// Debug sortie a la console	
#ifdef DEBUG
			if (check_buttons(cond1.buttons,CONT_Y)==KEY_ASSERTED)
			{	gui_state=OUT_SCREEN;				
			}
#endif
		
		}	

		frame_timer++;
		if(frame_timer >7)
		{	
			frame_timer=0;	

			switch(title_pos)
			{
				case 2:
				{	credits_cnt++;
					
				
					if(credits_cnt<37)
					{	credits_cnt++;}
					else
					{	credits_cnt=0;}
					
					sprintf(filename,"/rd/credits00%.2d.pcx",credits_cnt);
					if(LoadPcx2Buffer(filename,t))
						fastmemcpy((uint32*) txr_credits,(uint32*) t,256*256*2);
					
				}break;

				case 1:
				{
					if(games_cnt<25)
					{	games_cnt++;}
					else
					{	games_cnt=0;}		
					sprintf(filename,"/rd/games00%.2d.pcx",games_cnt);
					if(LoadPcx2Buffer(filename,t))
						fastmemcpy((uint32*) txr_games,(uint32*) t,256*256*2);
					
				}break;

				case 0:
				{	settings_cnt++;
					if(settings_cnt<37)
					{	settings_cnt++;}
					else
					{	settings_cnt=0;}
					
					sprintf(filename,"/rd/soucoupe00%.2d.pcx",settings_cnt);
					if(LoadPcx2Buffer(filename,t))
						fastmemcpy((uint32*) txr_settings,(uint32*) t,256*256*2);
										
				}break;			
			}								
			
		}


		dc_draw_title_frame();
	}

	//---< Fade Out >--
	while(vertex_color>0.025f)
	{	vertex_color-=0.025f;
		vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
		dc_draw_title_frame();
	}
	vertex_color_value=0;

}

//-----------------------------------------------------------------------------

void gui_credits_screen(void)
{
	cont_cond_t cond1;		
	uint8 mcont1 = 0;
	int ret=0;

	png_to_texture("/rd/credits.png", txr_bg,PNG_NO_ALPHA);

	//--< Fade In >--
	vertex_color=0.0f;	
	while(vertex_color<(1.0f - 0.025f))
	{	vertex_color+=0.025f;
		vertex_color_value=PVR_PACK_COLOR(0.5, vertex_color, vertex_color, vertex_color);
		dc_draw_credits_frame();
	}
	vertex_color=1.0f;
	vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);	
		
	// boucle principale
	while(gui_state==CREDITS_SCREEN)
	{
		if (!mcont1)
		{	mcont1= maple_first_controller();}	

		if(mcont1)
		{
			if(cont_get_cond(mcont1, &cond1))
			{	return;}

			if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
				&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
			{	(*(void(**)())0x8c0000e0)(1);	
			}

			if(check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED)			
			{	gui_state=MAIN_SCREEN;
			}
		}

		dc_draw_credits_frame();
	}

	//---< Fade Out >--
	while(vertex_color>0.025f)
	{	vertex_color-=0.025f;
		vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
		dc_draw_credits_frame();
	}
	vertex_color_value=0;
}
