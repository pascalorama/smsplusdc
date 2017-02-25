#include "dc_shared.h"

t_screenpos bookmark_pos;
t_screenpos bookmark_text_pos;
t_file_entries fav_files[MAX_FAV_ENTRIES]; 
t_file_entries old_fav_files[MAX_FAV_ENTRIES]; 

int bookmark_index=MAX_FAV_ENTRIES-1;
int last_bookmark_index=MAX_FAV_ENTRIES-1;

#define EMPTY_SLOT  "<EMPTY SLOT>"

pvr_ptr_t   txr_bookmark_text;
pvr_ptr_t   txr_lock;
uint16 bookmark_text_vbuffer[256 * 256] __attribute__ ((aligned (32)));

////////////////////////////////////////////////////////////////////////////////
//

void copy_bookmark(void)
{
    int i=0;   
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {
        strcpy( old_fav_files[i].filename,fav_files[i].filename);
        old_fav_files[i].flag=fav_files[i].flag;
    }
}

int check_update_bookmark()
{
    int i=0;   
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {
        if(stricmp( old_fav_files[i].filename,fav_files[i].filename)==0)
        {   if( (old_fav_files[i].flag & FILEENTRY_LOCKED) !=
                (fav_files[i].flag & FILEENTRY_LOCKED))
            {   return 0;}
        }
        else
        {
            if( (old_fav_files[i].flag & FILEENTRY_LOCKED) !=
                (fav_files[i].flag & FILEENTRY_LOCKED))
            {   return 0;}
        }        
    }
    
    return 1;
}

void check_bookmark_saving()
{
    if(check_update_bookmark()==0)
    {
        vmu_save_bookmark();
    }
}

////////////////////////////////////////////////////////////////////////////////
//

void allocate_bookmark(void)
{
   	fastmemset(bookmark_text_vbuffer,0,0);	
	txr_bookmark_text = pvr_mem_malloc (256 * 256 * 2);			    			
	
    txr_lock = pvr_mem_malloc (16 * 16 * 2);
    LoadPcx2Buffer("/rd/lock.pcx",txr_lock);
}

////////////////////////////////////////////////////////////////////////////////
//

void bookmark_format(int slot,char *filename)
{
    char t[256]={""};
    char *f;
    
    f=get_filename(filename);    
    strupper(f);
    
    strcpy(t,f);
	strcpy(my_strrchr(t, '.'), "");	
	strcpy(fav_files[slot].filename,filename);
	if(strlen(t)>32)
	{	strncpy(fav_files[slot].displayname,t,32);}
	else
	{   strcpy(fav_files[slot].displayname,t);}     

	if(stricmp(my_strrchr(fav_files[slot].filename, '.'), ".gg") == 0)
	{	fav_files[slot].flag|=FILELIST_FILE | FILELIST_GG;}
	else
	{	fav_files[slot].flag|=FILELIST_FILE | FILELIST_SMS;	}   
}

////////////////////////////////////////////////////////////////////////////////
//
void load_bookmark()
{
    int i=0;
        
    vmu_load_bookmark();
        
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {                
        if(strlen(fav_files[i].filename)==0)
        {   strcpy(fav_files[i].filename,"");
            strcpy(fav_files[i].displayname,EMPTY_SLOT);
            fav_files[i].flag=0;
        }
        else
        {   bookmark_format(i,fav_files[i].filename);
            fav_files[i].flag=FILELIST_FILE | FILEENTRY_LOCKED; 
            bookmark_index=i;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//

void bookmark_init()
{
	bookmark_pos.w = 320;
	bookmark_pos.h = 310;
	bookmark_pos.x = ((640 / 2) - (bookmark_pos.w/2));
	bookmark_pos.y =  ((480 / 2) - (bookmark_pos.h/2));	
	
	bookmark_text_pos.w=256;
	bookmark_text_pos.h=256;
	bookmark_text_pos.x=192;
	bookmark_text_pos.y=100;
	
    up_timer=0;
    down_timer=0;
    
    copy_bookmark();
			
	render_bookmark_text();	
}

////////////////////////////////////////////////////////////////////////////////
//

int bookmark_find_slot(char* filename)
{   int i=0;        
    int j;  
       
    // test si n'est pas dedans
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {   if(stricmp(fav_files[i].filename,filename)==0)
        {   return i;
        }
    }
    
    // recherche le premier unlock
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {   j= (last_bookmark_index+ 1 + i) % MAX_FAV_ENTRIES;
               
        if(!(fav_files[j].flag & FILEENTRY_LOCKED))
        {   
            last_bookmark_index=j;
            return j;
        }
    }    
  
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
//

int bookmark_add_file(t_file_entries *file,int lock)
{    
    int i=0;

    int slot = bookmark_find_slot(file->filename);         
    if(slot!=-1)
    {   
        bookmark_index=slot;
        bookmark_format(slot,file->filename);  
        if(lock==1)
        {   fav_files[bookmark_index].flag|=FILEENTRY_LOCKED;}                  
    }           
    else
    {   for(i=0;i<MAX_FAV_ENTRIES;i++)
        {   // si déjà dans le bookmark
            if(stricmp(file->filename,fav_files[i].filename)==0)
            {   bookmark_index=i;
                if(lock==1)
                {   fav_files[bookmark_index].flag|=FILEENTRY_LOCKED;}    
            }
        }    
    }
    render_bookmark_text();
    
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
//

void bookmark_draw_lock(int x,int y)
{
    int w=16,h=16;
    
	pvr_vertex_t vert;
	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       

	// le text
	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 16,16, txr_lock, PVR_FILTER_NONE);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = x;
    vert.y = y;
    vert.z = 2;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = x+w;
    vert.y = y;
    vert.z = 2;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = x;
    vert.y = y+h;
    vert.z = 2;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = x+w;
    vert.y = y+h;
    vert.z = 2;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));

}

////////////////////////////////////////////////////////////////////////////////
//

void bookmark_draw_selection_bar(void)
{
	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;
    
    t_screenpos mypos;
    
    mypos.x=bookmark_text_pos.x-4;    
    mypos.y=bookmark_text_pos.y-2 +( 20 * bookmark_index) + 32;
    mypos.w=32*8+8;
    mypos.h=20;
    
	pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));
   		
	vert.argb = (0x80 << 24) | (0<<16) | (0<<8) | (217);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = mypos.x;
    vert.y = mypos.y;
    vert.z = 2;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = mypos.x + mypos.w;
    vert.y = mypos.y;
    vert.z = 2;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = mypos.x;
    vert.y = mypos.y+mypos.h;
    vert.z = 2;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = mypos.x + mypos.w;
    vert.y = mypos.y+mypos.h;
    vert.z = 2;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}

////////////////////////////////////////////////////////////////////////////////
//

void render_bookmark_text(void)
{
    int i=0;
    int ligne=0;
    
    fastmemset(bookmark_text_vbuffer,0,256*256*2);
    
    amiga_textout(&bookmark_text_vbuffer[0],256,"FAVORITES",LIST_COLOR);
    for(i=0;i<32*8;i++)
    {   bookmark_text_vbuffer[(20*256)+i]=LIST_COLOR;
    }
    ligne = 20;
    
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {   
        if(i==bookmark_index)
        {   amiga_textout(&bookmark_text_vbuffer[(32+(i*20))*256],256,fav_files[i].displayname,SELECTION_COLOR);}
        else
        {               
            if(stricmp(fav_files[i].filename,sel_file)==0 && strlen(sel_file)>0)           
            {   amiga_textout(&bookmark_text_vbuffer[(32+(i*20))*256],256,fav_files[i].displayname,LOADED_COLOR);}
            else
            {   amiga_textout(&bookmark_text_vbuffer[(32+(i*20))*256],256,fav_files[i].displayname,LIST_COLOR);}           
        }       
    }
    
    for(i=0;i<32*8;i++)
    {   bookmark_text_vbuffer[((32+(10*20))*256)+i]=LIST_COLOR;        
    }    
    amiga_textout(&bookmark_text_vbuffer[(32+(10*21))*256],256,"(A)LOAD (X)LOCK/UNLOCK (B)CANCEL",LIST_COLOR);
        
	fastmemcpy(txr_bookmark_text,bookmark_text_vbuffer,256*256*2);
}

////////////////////////////////////////////////////////////////////////////////
//
void draw_bookmark_text(void)
{
	pvr_vertex_t vert;

	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       

	// le text
	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 256,256, txr_bookmark_text, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = bookmark_text_pos.x;
    vert.y = bookmark_text_pos.y;
    vert.z = 2;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = bookmark_text_pos.x + bookmark_text_pos.w;
    vert.y = bookmark_text_pos.y;
    vert.z = 2;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = bookmark_text_pos.x;
    vert.y = bookmark_text_pos.y + bookmark_text_pos.h;
    vert.z = 2;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = bookmark_text_pos.x + bookmark_text_pos.w;
    vert.y = bookmark_text_pos.y + bookmark_text_pos.h;
    vert.z = 2;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
    
}

////////////////////////////////////////////////////////////////////////////////
//

void draw_bookmark(void)
{
    int x,i;

	pvr_vertex_t vert;
	pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;       

	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, 128, 128, txr_menu, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

	vert.argb = vertex_color_value;
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = bookmark_pos.x;
    vert.y = bookmark_pos.y;
    vert.z = 2.0;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = bookmark_pos.x + bookmark_pos.w;
    vert.y = bookmark_pos.y;
    vert.z = 2.0;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = bookmark_pos.x;
    vert.y = bookmark_pos.y + bookmark_pos.h;
    vert.z = 2.0;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = bookmark_pos.x + bookmark_pos.w;
    vert.y = bookmark_pos.y + bookmark_pos.h;
    vert.z = 2.0;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));	
    
    draw_bookmark_text();
    bookmark_draw_selection_bar();
    
    x=bookmark_text_pos.x-20;
    for(i=0;i<MAX_FAV_ENTRIES;i++)
    {   if(fav_files[i].flag &  FILEENTRY_LOCKED)
        {   bookmark_draw_lock(x,bookmark_text_pos.y-2 +( 20 * i) + 32);}
    }

}

///////////////////////////////////////////////////////////////////////////////
//
void bookmark_run(void)
{
    cont_cond_t cond1;		
	uint8 mcont1 = 0;
	uint16 ret=0;
	int max_pos=9;
	
	if (!mcont1)
    {	mcont1= maple_first_controller();}	

	if(mcont1)
	{
        /* Check for start on the controller */
	    if (cont_get_cond(mcont1, &cond1))
		{	return;}
		
		// ferme le bookmark
		if (check_buttons(cond1.buttons,CONT_B)==KEY_ASSERTED ||
            check_buttons(cond1.buttons,CONT_Y)==KEY_ASSERTED)
   		{  dsp_bookmark=0;
   		   check_bookmark_saving();
   		}
   		
   		// flip flop du lock
   		if (check_buttons(cond1.buttons,CONT_X)==KEY_ASSERTED)
   		{  
   		   if(fav_files[bookmark_index].flag & FILEENTRY_LOCKED)
   		   {   fav_files[bookmark_index].flag &= ~FILEENTRY_LOCKED;}
   		   else
   		   {
                if(strlen(fav_files[bookmark_index].filename)>0)
                    fav_files[bookmark_index].flag |= FILEENTRY_LOCKED;
                
            }
   		   
   		}
   		
   		// haut & bas
		ret=check_buttons(cond1.buttons,CONT_DPAD_DOWN);
		if( (ret==KEY_ASSERTED) || (down_timer>KEY_DELAY))
		{	
			if(bookmark_index<max_pos)bookmark_index++;
			render_bookmark_text();
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
		{	if(bookmark_index>0)bookmark_index--;
            render_bookmark_text();
			up_timer=0;				
		}
		else
		{   if(ret==KEY_PUSHED)
				up_timer++;			
			else
				up_timer=0;					
		}
		
		// charge la rom
		if (check_buttons(cond1.buttons,CONT_A)==KEY_ASSERTED)
   		{              
           if(strlen(fav_files[bookmark_index].filename)>0) 
       	   {    fav_files[bookmark_index].flag |= FILEENTRY_LOCKED;     	   
                gui_load_rom(fav_files[bookmark_index].filename);
                
                dsp_bookmark=0;
                // enrefistre
                check_bookmark_saving();                
       	   }
   		}
   		
   		
    }
    
}
