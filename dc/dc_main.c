


#include "shared.h"
#include "dc_shared.h"

KOS_INIT_ROMDISK(romdisk);

//=============================================================================

static pvr_init_params_t pvr_params = 
{
  /* Enable opaque and translucent polygons with size 16 */
  { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
  
  /* Vertex buffer size 512K */
  512*1024
};

int load_bios()
{
	unsigned int fd;
    int size;
	int start=0;
	    
    fd = fs_open("/cd/bios_ue.sms", O_RDONLY);
	if(fd==0)
	{	fs_close ( fd );
		return 0;
	}
    size = fs_total(fd);

	if(fs_read(fd, &bios_sms.rom[0], size)!=size)
	{	fs_close ( fd );
		return 0;	
	}

	return 1;
}

//=============================================================================

int gui_state = MAIN_SCREEN;

int vertex_color_value = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f); 
float vertex_color=1.0; 

//=============================================================================
//-----------------------------------------------------------------------------
// Main entry

int main(int argc,char** argv)
{
	int contloop=1;

	// init all stuff here
	pvr_init_defaults();
		
	// allocate the emu and gui stuff
	allocate_bookmark();
	dc_emu_init();	
	gui_roms_screen_init();
	init_title_texture();
	gui_adjust_init();

	default_config();
	load_config(NULL);	
    load_bookmark();		

	vmu_draw_icon(0);
		
	//
	if(load_rom("/cd/boot.sms"))
	{
		gui_state=EMU_SCREEN;
		emu_join=0;	
		emu_autobooted=1;
		strcpy(sel_file,"boot.sms");
	}
	else
	{
		if(load_rom("/cd/boot.gg"))
		{
			gui_state=EMU_SCREEN;
			emu_join=0;	
			emu_autobooted=1;
			strcpy(sel_file,"boot.gg");
		}
	}

	while(contloop)
	{
		switch(gui_state)
		{

			case CREDITS_SCREEN:
			{
				gui_credits_screen();
			}break;

			case MAIN_SCREEN:
			{
				gui_title_screen();
			}break;

			case ADJUST_SCREEN:
			{
				gui_adjust_screen();
			}break;

			case ROMS_SCREEN:
			{
							
				gui_roms_screen();

			}break;

			case OPTIONS_SCREEN:
			{   
			   	
				gui_options_screen();
			}break;

			case EMU_SCREEN:
			{
				dc_emu_start();
			}break;	
			
			// debug
			
#ifndef DEBUG
			case OUT_SCREEN:
			{	contloop=0;
			}break;
#endif
		}
	
	
	}



	return 0;
}


