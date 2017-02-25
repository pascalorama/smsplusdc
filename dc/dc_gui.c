
#include "dc_shared.h"

t_file_entries cd_files[MAX_FILES_ENTRIES];
t_file_entries *files=cd_files;


#define	OPT_COUNTRY	0
#define OPT_TV		1
#define OPT_PAINT_BACKDROP	2
#define OPT_INVERT			3
#define	OPT_SPRITE_LIMIT	4
#define	OPT_BILINEAR		5
#define OPT_SRAM_SLOT		6
#define OPT_STATE_SLOT		7
#define OPT_SENSITIVITY     8
#define OPT_AUTO_SAVE       9
#define OPT_SMS_ADJUST		10
#define OPT_GG_ADJUST		11

int files_count=0;
int	dirs_count=0;
int	list_idx=0;
int	list_count=0;
int fav_idx=1;
int list_selection=-1;
int list_prev_idx=0;
int emu_autobooted=0;
uint8	list_mode=0;
int		fav_count=0;
int dsp_bookmark=0;

uint8	up_timer=0;
uint8	down_timer=0;
uint8	left_timer=0;
uint8	right_timer=0;
uint8	ltrig_timer=0;
uint8	rtrig_timer=0;

#ifndef DDD
char	cur_dir[256]={""};
#else
char    cur_dir[256]={"/smsplusdcv02"};

#endif

char	sel_filepath[256]={""};
char	sel_file[256]={""};
char    lecture_rom[4][32]={"Now Loading...","Now Roboading...","It's Thinking...","Now Moroading..."};
int     lecture_msg_index=0;

#define ROOT_DIR    "/cd"

//-----------------------------------------------------------------------------
//

#define MAX_CHARS	26
#define	PAGE_SIZE	15
#define	PAGE_X		20
#define	PAGE_Y		32
#define	FONT_ZOOM	1
#define	LINE_HEIGHT	20

pvr_ptr_t	txr_bg;
pvr_ptr_t	txr_opt;
pvr_ptr_t	txr_text;
pvr_ptr_t	txr_status;
pvr_ptr_t	txr_kitty[6];

uint16 text_vbuffer[256 * 512]		__attribute__ ((aligned (32)));
uint16 status_vbuffer[256 * 16]		__attribute__ ((aligned (32)));

t_screenpos selection_bar_pos;
t_screenpos text_pos;
t_screenpos status_pos;
t_screenpos kitty_pos;

int	options_pos=0;
int kitty_frame=0;
int kitty_cnt=0;

//-----------------------------------------------------------------------------
//
void fastmemcpy(uint32 *dest, uint32 *src, int bytes)
{
    bytes = bytes / 4;
        while (bytes-- > 0)
        {   *dest++ = *src++;}
}

void fastmemset(void *dest,int value, int bytes)
{
	uint32 *d= (uint32*) dest;
	
    bytes = bytes / 4;
    while (bytes-- > 0)
    {   *d++ = value;}
}


char * my_strrchr(const char * s, int c)
{
       const char *p = s + strlen(s);
       do {
           if (*p == (char)c)
               return (char *)p;
       } while (--p >= s);
       return NULL;
}

char* get_filename(char* filename)
{
	char r_file[256]={""};
	int i;
	if(!filename) return NULL;
		
	strcpy(r_file,filename);
		
	for (i = strlen(r_file) - 1 ; i >= 0 ; i--)
	{	if (r_file[i] == '\\' || r_file[i] == '/') 
			break;		
	}

	strcpy(r_file, &r_file[i + 1]);    		
	return r_file;
}

//-----------------------------------------------------------------------------
//
// Draw Background
//

void draw_mainscreen(void)
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


void draw_optionsscreen(void)
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
//

void draw_text(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256, 512, txr_text, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = text_pos.x;
    vert.y = text_pos.y;
    vert.z = 1.0;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = text_pos.x + text_pos.w;
    vert.y = text_pos.y;
    vert.z = 1.0;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = text_pos.x;
    vert.y = text_pos.y + text_pos.h;
    vert.z = 1.0;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = text_pos.x + text_pos.w;
    vert.y = text_pos.y + text_pos.h;
    vert.z = 1.0;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}

//-----------------------------------------------------------------------------
//

void draw_status(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256, 16, txr_status, PVR_FILTER_NONE);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = status_pos.x;
    vert.y = status_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = status_pos.x + status_pos.w;
    vert.y = status_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = status_pos.x;
    vert.y = status_pos.y + status_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = status_pos.x + status_pos.w;
    vert.y = status_pos.y + status_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}

//-----------------------------------------------------------------------------
//

void draw_kitty(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 128, 128, txr_kitty[kitty_frame], PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = kitty_pos.x;
    vert.y = kitty_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = kitty_pos.x + kitty_pos.w;
    vert.y = kitty_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = kitty_pos.x;
    vert.y = kitty_pos.y + kitty_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = kitty_pos.x + kitty_pos.w;
    vert.y = kitty_pos.y + kitty_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));

	
	kitty_cnt++;
	if( (kitty_cnt & 0x1f)==0 )
	{
		if(kitty_frame<5)
			kitty_frame++;
		else
			kitty_frame=0;
	}
		
}

//-----------------------------------------------------------------------------
// Draw Selection Bar

void draw_selection_bar(void)
{
	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;
    
	pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));
   		
	vert.argb = (0x80 << 24) | (0<<16) | (0<<8) | (217);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = selection_bar_pos.x;
    vert.y = selection_bar_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = selection_bar_pos.x + selection_bar_pos.w;
    vert.y = selection_bar_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = selection_bar_pos.x;
    vert.y = selection_bar_pos.y+selection_bar_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = selection_bar_pos.x + selection_bar_pos.w;
    vert.y = selection_bar_pos.y+selection_bar_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));

}

//-----------------------------------------------------------------------------

void draw_mainscreen_frame(void)
{
    pvr_wait_ready();
    pvr_scene_begin();
    
    pvr_list_begin(PVR_LIST_OP_POLY);
    draw_mainscreen();
	pvr_list_finish();
	
	pvr_list_begin(PVR_LIST_TR_POLY);
		
	draw_selection_bar();		
	draw_snapshot();	
		
	draw_status();
	draw_text();
			
    if(dsp_bookmark==1)
    {   draw_bookmark();}
    
    draw_kitty();
    
	  
    
	pvr_list_finish();

	pvr_scene_finish();
    
}

//-----------------------------------------------------------------------------
// Initialize Texture stuff and pictures loading

void gui_roms_screen_init(void)
{
	uint16 kitty_vbuffer[128 * 128]		__attribute__ ((aligned (32)));
	int i=0;
	
	fav_idx=1;
	files=cd_files;
	
	txr_bg = pvr_mem_malloc(512*512*2);
	txr_opt = pvr_mem_malloc(512*512*2);   

	txr_text=pvr_mem_malloc(256*512*2);
	fastmemset(text_vbuffer,0,256*256*2);
	fastmemcpy(txr_text,text_vbuffer,256*512*2);			

	txr_status=pvr_mem_malloc(256*16*2);
	fastmemset(status_vbuffer,0,256*16*2);
	fastmemcpy(txr_status,status_vbuffer,256*16*2);	
	
	// anim du chat
	for(i=0;i<6;i++)
	{	txr_kitty[i] = pvr_mem_malloc(128*128*2);}
	
	LoadPcx2Buffer("/rd/chat01.pcx",kitty_vbuffer);
	fastmemcpy(txr_kitty[0],kitty_vbuffer,128*128*2);	
	LoadPcx2Buffer("/rd/chat02.pcx",kitty_vbuffer);
	fastmemcpy(txr_kitty[1],kitty_vbuffer,128*128*2);	
	LoadPcx2Buffer("/rd/chat03.pcx",kitty_vbuffer);
	fastmemcpy(txr_kitty[2],kitty_vbuffer,128*128*2);	
	LoadPcx2Buffer("/rd/chat04.pcx",kitty_vbuffer);
	fastmemcpy(txr_kitty[3],kitty_vbuffer,128*128*2);	
	LoadPcx2Buffer("/rd/chat05.pcx",kitty_vbuffer);
	fastmemcpy(txr_kitty[4],kitty_vbuffer,128*128*2);	
	LoadPcx2Buffer("/rd/chat06.pcx",kitty_vbuffer);
	fastmemcpy(txr_kitty[5],kitty_vbuffer,128*128*2);	

	gui_snap_init();	
}



void strupper ( char * str )
{
	int i;
		
	for (i=0;i<strlen(str);i++)
		str[i] = toupper ( str[i] );	
}

static void parentDir ( char * dir )
{
    int len = strlen ( dir )-1;
    int i;
      
    if (dir[len]=='/')
      len--;
    if (len<=0)
      return;

    for (i=len;i>=0;i--)
    {
        if (dir[i]=='/')
          break;
    }
  
    dir[i] = '\0';
}

int gui_get_directories(char *dirn)
{
	file_t fh;
	char dname[256]={ROOT_DIR};
	dirent_t * entry;
	int numdir=0;	
	strcat (dname, dirn);			
	files_count=0;
	dirs_count=0;
	list_count=0;	
	memset(&cd_files[0],0,MAX_FILES_ENTRIES * sizeof(t_file_entries));
	
	if (cdrom_reinit() ==ERR_OK)
	{	
		if ((fh=fs_open(dname, O_DIR|O_RDONLY))==0)
			return -1;	

		// read all direcotries entry
		entry=fs_readdir(fh);
		while(entry!=NULL)
		{
			if(entry->size==-1)
			{	
				if( stricmp(entry->name,SNAP_DIR_NAME)!=0 &&
                    stricmp(entry->name,"src")!=0
                    )
				{						
					strcpy(cd_files[list_count].filename,entry->name);											
					strcpy(cd_files[list_count].displayname,entry->name);

					cd_files[dirs_count].flag=FILELIST_DIR;
					dirs_count++;
					list_count++;
				
					if(list_count>MAX_FILES_ENTRIES-1)
					{	fs_close ( fh );
						return 0;
					}
				}
			}	
			entry=fs_readdir(fh);
		}	
		fs_close ( fh );

		if ((fh=fs_open(dname, O_DIR|O_RDONLY))==0)
			return -1;

		entry=fs_readdir(fh);
		while(entry!=NULL)
		{
			if(entry->size!=-1)
			{	
				if( stricmp(my_strrchr(entry->name, '.'), ".gg") == 0 || stricmp(my_strrchr(entry->name, '.'), ".sms") == 0)
				{	
					char t[256]={""};										
					sprintf(cd_files[list_count].filename,"%s/%s",dname,entry->name);
										
					strcpy(t,entry->name);
					strcpy(my_strrchr(t, '.'), "");					

					if(strlen(t)>32)
					{		
						strncpy(cd_files[list_count].displayname,t,32);
					}
					else
					{
						strcpy(cd_files[list_count].displayname,t);
					}																						
										
					if(stricmp(my_strrchr(entry->name, '.'), ".gg") == 0)
					{	cd_files[list_count].flag=FILELIST_FILE | FILELIST_GG;						
					}
					else
					{	cd_files[list_count].flag=FILELIST_FILE | FILELIST_SMS;						
					}
					
					list_count++;
					files_count++;

					if(list_count>MAX_FILES_ENTRIES-1)
					{	fs_close ( fh );
						return 0;
					}
				}
			}	
			entry=fs_readdir(fh);
		}	
		fs_close ( fh );
	}
		
	return list_count;
}


//-----------------------------------------------------------------------------
//
// Display Roms Selection
//

void gui_display_roms_page(int page)
{
	int i=0;
	int y=0;
	int page_num=0;
	int start_pos=0;
	int end_pos=0;
	char txt[256];
	
	y=0;

	fastmemset(text_vbuffer,0x0,256*512*2);
		
	if(list_count>0)
	{		
		if(list_idx>list_count-1) list_idx=list_count-1;
		if(list_idx<0) list_idx=0;
										
		page_num=(int)(page/ PAGE_SIZE);												
		start_pos=page_num * PAGE_SIZE;
						
		if( start_pos+ PAGE_SIZE>list_count-1)
			end_pos=list_count-1;
		else
			end_pos= (start_pos+PAGE_SIZE)-1;				
									
		for(i=start_pos;i<=end_pos ;i++)
		{
			if(i!=list_idx)
			{									
				if( stricmp(files[i].filename,sel_file)==0)								
				{	
					// affiche la rom chargée
					if(files[i].flag & FILELIST_DIR)
					{	sprintf(txt,"[%s]",files[i].displayname);}
					else
					{	sprintf(txt,"%s",files[i].displayname);}								
					strupper(txt);
					amiga_textout(&text_vbuffer[(y*256)],256,txt,LOADED_COLOR);
				}
				else
				{	
					// affiche une rom normale
					if(files[i].flag & FILELIST_DIR)
					{	sprintf(txt,"[%s]",files[i].displayname);	
						strupper(txt);
						amiga_textout(&text_vbuffer[(y*256)],256,txt,LIST_COLOR);		
					}
					else
					{	sprintf(txt,"%s",files[i].displayname);
						strupper(txt);

						if(files[i].flag & FILELIST_GG)
						{
							amiga_textout(&text_vbuffer[(y*256)],256,txt,GG_COLOR);								
						}
						else
						{	amiga_textout(&text_vbuffer[(y*256)],256,txt,SMS_COLOR);								
						}					
					}										
				}
			}
			else
			{	
				// affiche la sélection courante
				if(files[i].flag & FILELIST_DIR)
				{	sprintf(txt,"[%s]",files[i].displayname);}
				else
				{	sprintf(txt,"%s",files[i].displayname);}
				strupper(txt);
				amiga_textout(&text_vbuffer[(y*256)],256,txt,SELECTION_COLOR);
				selection_bar_pos.y=32+(y*FONT_ZOOM);								
			}		
			y=y+LINE_HEIGHT;
		}
	}
	else
	{	
		selection_bar_pos.y=32;

		if(strlen(cur_dir)==0)
		{
			amiga_textout(&text_vbuffer[0+64],256,"INSERT ROMS CD",SELECTION_COLOR);
			amiga_textout(&text_vbuffer[(LINE_HEIGHT*256)+40],256,"and press (B) button",SELECTION_COLOR);		
		}
		else
		{
			amiga_textout(&text_vbuffer[0+64],256,"EMPTY DIRECTORY",SELECTION_COLOR);			
		}
	}

	fastmemcpy(txr_text,text_vbuffer,256*512*2);

}

//-----------------------------------------------------------------------------
//
// check the key mask
//

uint16 check_buttons(uint16 keyvalue,uint16 keymask)
{
	static uint16 key_pressed=0;
	uint16 key= key_pressed & keymask;

	if(key==0 && !(keyvalue & keymask))
	{	key_pressed |= keymask;		
		return KEY_ASSERTED;
	}
	
	if(key!=0 && (keyvalue & keymask))
	{	key_pressed &= ~keymask;
		return KEY_RELEASED;
	}

	if(key!=0 && !(keyvalue & keymask))
	{	return KEY_PUSHED;}

	return 0;
}

uint16 check_triggers(uint8 keyvalue,uint8 keymask)
{
	static uint8 trigs_pressed=0;
	uint8 key= trigs_pressed & keymask;

	if(key==0 && (keyvalue >0))	
	{	trigs_pressed |= keymask;		
		return KEY_ASSERTED;
	}
	
	if(key!=0 && (keyvalue ==0))
	{	trigs_pressed &= ~keymask;
		return KEY_RELEASED;
	}

	if(key!=0 && (keyvalue >0))
	{	return KEY_PUSHED;}

	return 0;
}

//-----------------------------------------------------------------------------
//
// Options Screen
//

void draw_optionsscreen_frame(void)
{
    pvr_wait_ready();
    pvr_scene_begin();
    
    pvr_list_begin(PVR_LIST_OP_POLY);
    draw_optionsscreen();
	pvr_list_finish();
	
	pvr_list_begin(PVR_LIST_TR_POLY);	
	draw_selection_bar();	
	draw_text();	
	pvr_list_finish();

	pvr_scene_finish();
    
}


void display_options_screen(int pos)
{
	char txt[256];
	
	fastmemset(text_vbuffer,0,256*512*2);	

	// country
	sprintf(txt,"Country                 %s", emu_cfg.country==TYPE_DOMESTIC ? "Domestic" : "Overseas");
	amiga_textout(&text_vbuffer[0] ,256,txt,pos==OPT_COUNTRY? SELECTION_COLOR:LIST_COLOR);

	// TV
	sprintf(txt,"MasterSystem tv type        %s", emu_cfg.sms_tv==TV_PAL ? " Pal" : "Ntsc");
	amiga_textout(&text_vbuffer[256*20] ,256,txt,pos==OPT_TV? SELECTION_COLOR:LIST_COLOR);

	// Backdrop
	sprintf(txt,"Paint backdrop               %s", emu_cfg.paint_backdrop==0 ? " No" : "Yes");
	amiga_textout(&text_vbuffer[256*40] ,256,txt,pos==OPT_PAINT_BACKDROP ? SELECTION_COLOR:LIST_COLOR);	

	// invert
	sprintf(txt,"Inverted buttons             %s", emu_cfg.invert_buttons==0 ? " No" : "Yes");
	amiga_textout(&text_vbuffer[256*60] ,256,txt,pos==OPT_INVERT? SELECTION_COLOR:LIST_COLOR);
		

	// sprites
	sprintf(txt,"Sprites flickering           %s",emu_cfg.sprite_limit==0 ? " No" : "Yes");
	amiga_textout(&text_vbuffer[256*80] ,256,txt,pos==OPT_SPRITE_LIMIT? SELECTION_COLOR:LIST_COLOR);

	// Bilinear
	sprintf(txt,"Bilinear filtering           %s",emu_cfg.bilinear==0 ? " No" : "Yes");
	amiga_textout(&text_vbuffer[256*100] ,256,txt,pos==OPT_BILINEAR? SELECTION_COLOR:LIST_COLOR);

	// sram slot
	sprintf(txt,"Sram slot                      %.1d",emu_cfg.sram_slot);
	amiga_textout(&text_vbuffer[256*120] ,256,txt,pos==OPT_SRAM_SLOT? SELECTION_COLOR:LIST_COLOR);

	// state
	sprintf(txt,"Save state slot                %.1d",emu_cfg.state_slot);
	amiga_textout(&text_vbuffer[256*140] ,256,txt,pos==OPT_STATE_SLOT? SELECTION_COLOR:LIST_COLOR);	
	
	sprintf(txt,"Paddle sensitivity             %.1d",emu_cfg.paddle_sensivity);
	amiga_textout(&text_vbuffer[256*160],256,txt,pos==OPT_SENSITIVITY? SELECTION_COLOR:LIST_COLOR);
	
   	sprintf(txt,"Auto save SRAM               %s",emu_cfg.auto_save==0 ? " No" : "Yes");
	amiga_textout(&text_vbuffer[256*180],256,txt,pos==OPT_AUTO_SAVE? SELECTION_COLOR:LIST_COLOR);
			
	amiga_textout(&text_vbuffer[256*200],256,"Adjust MasterSystem screen ",pos==OPT_SMS_ADJUST? SELECTION_COLOR:LIST_COLOR);
	amiga_textout(&text_vbuffer[256*220],256,"Adjust Gamegear screen ",pos==OPT_GG_ADJUST? SELECTION_COLOR:LIST_COLOR);
	
	//selection_bar_pos.y = (text_pos.y-2) + (pos*28)-2;	
	selection_bar_pos.y = (text_pos.y-2) + (pos*selection_bar_pos.h)-2;	
	fastmemcpy(txr_text,text_vbuffer,256*512*2);
}


void gui_options_screen(void)
{
	uint8 mcont1 = 0;
	cont_cond_t cond1;	
	uint16	ret;
	int		max_pos=11;	
	
	text_pos.x=90;
	text_pos.y=80;	
	text_pos.w=448;
    //text_pos.h=704;
    text_pos.h=768;

  	selection_bar_pos.x=80;
	selection_bar_pos.y=text_pos.y-2;

	selection_bar_pos.w= 466;	
	selection_bar_pos.h= 30;
		
//	memcpy(&emu_cfg_o,&emu_cfg,sizeof(t_config));
	
//	printf("AVANT:\n");
//	printf("emu_cfg_o\n x %d y %d w %d h %d\n",emu_cfg_o.ntsc_sms_pos.x,emu_cfg_o.ntsc_sms_pos.y,emu_cfg_o.ntsc_sms_pos.w,emu_cfg_o.ntsc_sms_pos.h);
//	printf("emu_cfg\n x %d y %d w %d h %d\n",emu_cfg.ntsc_sms_pos.x,emu_cfg.ntsc_sms_pos.y,emu_cfg.ntsc_sms_pos.w,emu_cfg.ntsc_sms_pos.h);
	

	display_options_screen(options_pos);

	up_timer=0;
	down_timer=0;
	left_timer=0;
	right_timer=0;
	adjust_type=-1;

	// reload the picture
	png_to_texture("/rd/options.png", txr_bg,PNG_NO_ALPHA);

	//--< Fade in >---
	vertex_color=0.0f;
	
	while(vertex_color<(1.0f - 0.025f))
	{	vertex_color+=0.025f;
		vertex_color_value=PVR_PACK_COLOR(0.5, vertex_color, vertex_color, vertex_color);
		pvr_wait_ready();
		pvr_scene_begin();
    
		pvr_list_begin(PVR_LIST_OP_POLY);
		draw_optionsscreen();
		pvr_list_finish();
	
		pvr_list_begin(PVR_LIST_TR_POLY);		
		draw_text();				
		pvr_list_finish();

		pvr_scene_finish();
	}


	while(gui_state==OPTIONS_SCREEN)
	{
		draw_optionsscreen_frame();
		
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
				if(options_pos<max_pos)options_pos++;
				display_options_screen(options_pos);
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
			{	if(options_pos>0)options_pos--;
				display_options_screen(options_pos);								
				up_timer=0;				
			}
			else
			{
				if(ret==KEY_PUSHED)
					up_timer++;			
				else
					up_timer=0;					
			}

			ret=check_buttons(cond1.buttons,CONT_DPAD_LEFT);
			if( ret==KEY_ASSERTED || left_timer>50)
			{	
				// manage options
				switch(options_pos)
				{
					case OPT_COUNTRY:
					{	emu_cfg.country^=1;
					}break;

					case OPT_INVERT:
					{	emu_cfg.invert_buttons^=1;
					}break;

					case OPT_TV:
					{	emu_cfg.sms_tv^=1;
					}break;

					case OPT_SPRITE_LIMIT:
					{	emu_cfg.sprite_limit^=1;						
					}break;

					case OPT_PAINT_BACKDROP:
					{	emu_cfg.paint_backdrop^=1;						
					}break;

					case OPT_BILINEAR:
					{	emu_cfg.bilinear^=1;						
					}break;

					case OPT_SRAM_SLOT	:
					{	if(emu_cfg.sram_slot>0) emu_cfg.sram_slot--;							
					}break;

					case OPT_STATE_SLOT:
					{	if(emu_cfg.state_slot>0) emu_cfg.state_slot--;							
					}break;
					
					case OPT_SENSITIVITY:
					{	if(emu_cfg.paddle_sensivity>0) emu_cfg.paddle_sensivity--;							
					}break;
     
                    case OPT_AUTO_SAVE:
					{   emu_cfg.auto_save^=1;
                        display_options_screen(options_pos);	
                    }break;
				}

				display_options_screen(options_pos);								
				left_timer=0;
			}
			else
			{
				if(ret==KEY_PUSHED)
					left_timer++;			
				else
					left_timer=0;		
			}

			ret=check_buttons(cond1.buttons,CONT_DPAD_RIGHT);
			if(ret==KEY_ASSERTED || right_timer>50)
			{	
				// manage options
				switch(options_pos)
				{
					case OPT_COUNTRY:
					{	emu_cfg.country^=1;
					}break;

					case OPT_INVERT:
					{	emu_cfg.invert_buttons^=1;
					}break;

					case OPT_TV:
					{	emu_cfg.sms_tv^=1;
					}break;

					case OPT_SPRITE_LIMIT:
					{	emu_cfg.sprite_limit^=1;						
					}break;

					case OPT_PAINT_BACKDROP:
					{	emu_cfg.paint_backdrop^=1;						
					}break;

					case OPT_BILINEAR	:
					{	emu_cfg.bilinear^=1;						
					}break;

					case OPT_SRAM_SLOT	:
					{	if(emu_cfg.sram_slot<9) emu_cfg.sram_slot++;							
					}break;

					case OPT_STATE_SLOT	:
					{	if(emu_cfg.state_slot<9) emu_cfg.state_slot++;							
					}break;	
					
					case OPT_SENSITIVITY:
					{	if(emu_cfg.paddle_sensivity<9) emu_cfg.paddle_sensivity++;							
					}break;
     
                    case OPT_AUTO_SAVE:
					{   emu_cfg.auto_save^=1;
                        display_options_screen(options_pos);	
                    }break;
				}				

				display_options_screen(options_pos);								
				right_timer=0;
			}			
			else
			{	if(ret==KEY_PUSHED)
					right_timer++;			
				else
					right_timer=0;	
			}

			if ( check_buttons(cond1.buttons,CONT_A)==KEY_ASSERTED)
			{
				switch(options_pos)
				{
					case OPT_GG_ADJUST:
					{	adjust_type=TYPE_GG;
						gui_state=ADJUST_SCREEN;
					}break;

					case OPT_SMS_ADJUST:
					{	adjust_type=TYPE_SMS;
						gui_state=ADJUST_SCREEN;						
					}break;

					case OPT_COUNTRY:
					{	emu_cfg.country^=1;
						display_options_screen(options_pos);	
					}break;

					case OPT_INVERT:
					{	emu_cfg.invert_buttons^=1;
						display_options_screen(options_pos);	
					}break;

					case OPT_TV:
					{	emu_cfg.sms_tv^=1;
						display_options_screen(options_pos);	
					}break;

					case OPT_SPRITE_LIMIT:
					{	emu_cfg.sprite_limit^=1;				
						display_options_screen(options_pos);	
					}break;

					case OPT_PAINT_BACKDROP:
					{	emu_cfg.paint_backdrop^=1;				
						display_options_screen(options_pos);	
					}break;

					case OPT_BILINEAR	:
					{	emu_cfg.bilinear^=1;					
						display_options_screen(options_pos);	
					}break;	
					
					case OPT_AUTO_SAVE:
					{   emu_cfg.auto_save^=1;
                        display_options_screen(options_pos);	
                    }
					
						
				}			
			}
		
			
			if (check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED)
			{
				if(emu_autobooted==1)
				{	gui_state=EMU_SCREEN;	
					emu_join=1;
				}
				else
					gui_state=MAIN_SCREEN;	
			
			}	

			//check si on a un reset
			if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
				&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
			{	(*(void(**)())0x8c0000e0)(1);	
			}					
		
		}		
	}

		// fade out
	while(vertex_color>0.025f)
	{
		vertex_color-=0.025f;
		vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
		
		pvr_wait_ready();
		pvr_scene_begin();
    
		pvr_list_begin(PVR_LIST_OP_POLY);
		draw_optionsscreen();
		pvr_list_finish();
	
		pvr_list_begin(PVR_LIST_TR_POLY);
		draw_text();		
		pvr_list_finish();

		pvr_scene_finish();
	}
	vertex_color_value=0;
	
	fastmemset(text_vbuffer,0,256*256*2);
	fastmemcpy(txr_text,text_vbuffer,256*256*2);
	
//	printf("APRES:\n");
//	printf("emu_cfg_o\n x %d y %d w %d h %d\n",emu_cfg_o.ntsc_sms_pos.x,emu_cfg_o.ntsc_sms_pos.y,emu_cfg_o.ntsc_sms_pos.w,emu_cfg_o.ntsc_sms_pos.h);
//	printf("emu_cfg\n x %d y %d w %d h %d\n",emu_cfg.ntsc_sms_pos.x,emu_cfg.ntsc_sms_pos.y,emu_cfg.ntsc_sms_pos.w,emu_cfg.ntsc_sms_pos.h);
	

	if(check_config()==1 ) save_config(NULL);
}

////////////////////////////////////////////////////////////////////////////////
//

void gui_load_rom(char* filename)
{
    // affiche status
    fastmemset(status_vbuffer,0,256*16*2);					
  	amiga_textout(&status_vbuffer[0],256,&lecture_rom[lecture_msg_index & 0x3][0],SELECTION_COLOR);
    lecture_msg_index++;
  	
  	fastmemcpy(txr_status,status_vbuffer,256*16*2);	
 	draw_mainscreen_frame();
	draw_mainscreen_frame();						
	strcpy(sel_filepath,filename);
	strcpy(sel_file,filename);

   	if(load_rom(sel_filepath))
  	{
 		gui_state=EMU_SCREEN;
   		emu_join=0;
   		list_selection=list_idx;
	}
   	else
  	{	
 		strcpy(sel_file,"");
		strcpy(sel_filepath,"");
	
      	fastmemset(status_vbuffer,0,256*16*2);
		amiga_textout(&status_vbuffer[0],256,"Unable to load ROM",SELECTION_COLOR);
   		fastmemcpy(txr_status,status_vbuffer,256*16*2);	
  		draw_mainscreen_frame();					
 	}
}

//-----------------------------------------------------------------------------
//
// Display Roms Selection
//

void gui_roms_screen(void)
{
	cont_cond_t cond1;		
	uint8 mcont1 = 0;
	uint16	ret;
	unsigned int fd;
		
	// re-init des variables
	selection_bar_pos.x= PAGE_X;
	selection_bar_pos.y=PAGE_Y;
	selection_bar_pos.w=300-PAGE_X;
	selection_bar_pos.h=20;

	text_pos.x=25;
	text_pos.y=PAGE_Y+3;
	text_pos.w=256*FONT_ZOOM;
	text_pos.h=512*FONT_ZOOM;

	status_pos.x=320;
	status_pos.y=285;
	status_pos.w= 256;
	status_pos.h= 16;

	//kitty_pos.x=500;
	//kitty_pos.y =344;
    kitty_pos.x=510;
	kitty_pos.y =350;
	kitty_pos.w = 160;
	kitty_pos.h =160;
	kitty_frame=0;
	kitty_cnt=0;

	up_timer=0;
	down_timer=0;
	left_timer=0;
	right_timer=0;
			
	fastmemset(text_vbuffer,0,256*256*2);
	fastmemset(status_vbuffer,0,256*16*2);

	amiga_textout(&status_vbuffer[0],256,"Select a ROM file",SELECTION_COLOR);
	fastmemcpy(txr_status,status_vbuffer,256*16*2);	

	// reload picture
	png_to_texture("/rd/romsload.png", txr_bg,PNG_NO_ALPHA);
		
	//---< loop for gui >---	
	memset(&cd_files[0],0,MAX_FILES_ENTRIES * sizeof(t_file_entries));
	files=&cd_files;

	gui_get_directories(cur_dir);	
	gui_display_roms_page(list_idx);

	//--< Fade in >---
	vertex_color=0.0f;
	
	while(vertex_color<(1.0f - 0.025f))
	{	vertex_color+=0.025f;
		vertex_color_value=PVR_PACK_COLOR(0.5, vertex_color, vertex_color, vertex_color);
		pvr_wait_ready();
		pvr_scene_begin();
    
		pvr_list_begin(PVR_LIST_OP_POLY);
		draw_mainscreen();
		pvr_list_finish();
	
		pvr_list_begin(PVR_LIST_TR_POLY);
		draw_text();		
		draw_status();
		draw_snapshot();
		draw_kitty();
		pvr_list_finish();

		pvr_scene_finish();
	}	    	
	
		
	while(gui_state==ROMS_SCREEN)
	{	
    	draw_mainscreen_frame();
        gui_snap_load();	
    	
        if(dsp_bookmark==0)
        {	
	        //draw_mainscreen_frame();	
        	

        	//-- get input --
        	if (!mcont1)
    		{	mcont1= maple_first_controller();}	

	       	if(mcont1)
	       	{
    			/* Check for start on the controller */
	       		if (cont_get_cond(mcont1, &cond1))
		      	{	return;}
			
                //-- A Button --
    			if (check_buttons(cond1.buttons,CONT_A)==KEY_ASSERTED)
	       		{	
		      		if(cond1.ltrig==0 && cond1.rtrig==0)
			     	{
                        if(files[list_idx].flag & FILELIST_DIR)
	       				{	
		      				sprintf(cur_dir,"%s/%s",cur_dir,files[list_idx].filename);
			         		list_prev_idx=list_idx;
					       	list_idx=0;
    						gui_get_directories(cur_dir);	
	       					gui_display_roms_page(list_idx);
	   					
		      			}
			     		else
				    	{	// démarre l'ému					
                            gui_load_rom(files[list_idx].filename);                     
				    	}
    				}			
	       			else
		      		{
	       	     		// add to favorite 
    					if(!(files[list_idx].flag & FILELIST_DIR))
		      			{    
		                      bookmark_add_file(&files[list_idx],1);
    	                      gui_load_rom(files[list_idx].filename);
    	                      check_bookmark_saving();
    	                      

				    	}
    				}
	       		}

    			//-- B button : parent dir --
	       		if (check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED)
		      	{	
	       			list_idx=list_prev_idx;				
	      			list_selection=-1;
		     		parentDir(cur_dir);			
    				gui_get_directories(cur_dir);	
	      			if(list_idx>list_count) list_idx=list_count;
		     		gui_display_roms_page(list_idx);			
    			
	       		}

    			//-- X button : quit screen --
	       		if (check_buttons(cond1.buttons,CONT_X)==KEY_ASSERTED)
	   	       	{	
			     	gui_state= MAIN_SCREEN;																	
    			}

	       		//-- Y button : go to bookmark --
    			if (check_buttons(cond1.buttons,CONT_Y)==KEY_ASSERTED)
	       		{	
		      		dsp_bookmark=1;
		      	    bookmark_init();
	    	        if(!(files[list_idx].flag & FILELIST_DIR) && list_count>0)
		      		{ bookmark_add_file(&files[list_idx],0);
		      		
		      		}  	      		    
    			}
						
	       		//-- test down key --
    			ret=check_buttons(cond1.buttons,CONT_DPAD_DOWN);
	       		if( (ret==KEY_ASSERTED) || (down_timer>KEY_DELAY))
		      	{	if(list_idx<list_count-1) list_idx++;	
			     	gui_display_roms_page(list_idx);
    				down_timer=0;				
	       		}
		      	else
    			{
	       			if(ret==KEY_PUSHED)
		      			down_timer++;			
			     	else
				    	down_timer=0;					
    			}
			
	       		//-- test up key --
		      	ret=check_buttons(cond1.buttons,CONT_DPAD_UP);			
    			if ( (ret==KEY_ASSERTED) || (up_timer>KEY_DELAY) )
	       		{					
		      		if(list_idx>0) list_idx--;	;		
        			gui_display_roms_page(list_idx);
		      		up_timer=0;				
    			}
	       		else
		      	{
			     	if(ret==KEY_PUSHED)
				    	up_timer++;			
    				else
	       				up_timer=0;					
		      	}
			
    			//-- 
	       		ret=check_buttons(cond1.buttons,CONT_DPAD_LEFT);
		      	if( ret==KEY_ASSERTED || left_timer>KEY_DELAY)
    			{	list_idx-=PAGE_SIZE;
	       			if(list_idx<0) list_idx=0;		
		      		gui_display_roms_page(list_idx);
    				left_timer=0;
	       		}
		      	else
    			{
	       			if(ret==KEY_PUSHED)
		      			left_timer++;			
			     	else
				    	left_timer=0;		
    			}

	       		ret=check_buttons(cond1.buttons,CONT_DPAD_RIGHT);
		      	if( ret==KEY_ASSERTED || right_timer>KEY_DELAY)
    			{	list_idx+=PAGE_SIZE;
	       			if(list_idx>list_count-1) list_idx=list_count-1;		
		      		gui_display_roms_page(list_idx);
    				right_timer=0;
	       		}			
		      	else
    			{	if(ret==KEY_PUSHED)
	       				right_timer++;			
		      		else
    					right_timer=0;	
	       		}
			
    			//-- rejoin the game --
	       		if( (cond1.ltrig && cond1.rtrig) || check_buttons(cond1.buttons,CONT_START)==KEY_ASSERTED)
		      	{	
			     	if(strlen(sel_file)>0)
    				{
	       				gui_state=EMU_SCREEN;
		      			list_idx=list_selection;
    					emu_join=1;
	       			}
		      	}									
			
    			//check si on a un reset
	       		if( !(cond1.buttons & CONT_START) && !(cond1.buttons & CONT_X) && !(cond1.buttons & CONT_Y)
		      		&& !(cond1.buttons & CONT_A)  && !(cond1.buttons & CONT_B))
    			{	(*(void(**)())0x8c0000e0)(1);	 		}			
    			
			} // mcont1				
		}
		else
        {               
            bookmark_run();
        }
	}

	gui_display_roms_page(list_idx);	

	// fade out
	while(vertex_color>0.025f)
	{
		vertex_color-=0.025f;
		vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
		
		pvr_wait_ready();
		pvr_scene_begin();
    
		pvr_list_begin(PVR_LIST_OP_POLY);
		draw_mainscreen();
		pvr_list_finish();
	
		pvr_list_begin(PVR_LIST_TR_POLY);
		draw_text();
		draw_status();
		draw_snapshot();
		draw_kitty();
		if(dsp_bookmark==1)
		{ draw_bookmark();}
		
		pvr_list_finish();

		pvr_scene_finish();
	}
	vertex_color_value=0;

}
