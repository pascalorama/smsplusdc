
#include "dc_shared.h"
#include "pcx.h"

uint8	snap_timer=0;
uint8	snap_flag=0;
int		snap_type=0;
uint8	snap_default=0;
int		snap_count=-1;
int     snap_index=0;
int		snap_slideshow=0;
char	cur_snap[256]={""};
int		old_snap_type=0;
int		enabled_slideshow=1;

#define	SNAP_LOAD_DELAY		25
#define SNAP_COUNT          10
#define SNAP_SLIDESHOW_DELAY    175

//-----------------------------------------------------------------------------
// 

uint16			snap_vbuffer[SNAP_TXR_W * SNAP_TXR_H]		__attribute__ ((aligned (32)));
uint16          snap_buffer[SNAP_COUNT][SNAP_TXR_W * SNAP_TXR_H]	__attribute__ ((aligned (32)));
pvr_ptr_t		txr_snap;
t_screenpos		snap_pos;

//-----------------------------------------------------------------------------
// 

void gui_snap_init(void)
{
	snap_pos.x=322;
	snap_pos.y=64;
	snap_pos.w=256;
	snap_pos.h=280;
	snap_pos.w=302;
	
	

	txr_snap=pvr_mem_malloc(SNAP_TXR_W*SNAP_TXR_H*2);
	fastmemcpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);	

}
/*
void gui_snap_load(void)
{
	// si j'ai changer 
	if(stricmp(cur_snap,files[list_idx].filename)!=0)
	{		
		strcpy(cur_snap,files[list_idx].filename);
		snap_timer=0;
		snap_flag=0;
		
        snap_index=0;
        snap_count=0;
		
		old_snap_type=snap_type;
	
		if((files[list_idx].flag & FILELIST_GG))
		{	snap_type=TYPE_GG;
		}
		
		if((files[list_idx].flag & FILELIST_SMS))
		{	snap_type=TYPE_SMS;
		}

		if((files[list_idx].flag & FILELIST_DIR))
		{

			if(stricmp(cur_snap,"GG")==0)
			{	snap_type=TYPE_GG;}
			else
			{	if(stricmp(cur_snap,"SMS")==0)
				{	snap_type=TYPE_SMS;}
				else
				{	snap_type=old_snap_type;}	
			}			
		}

		// si le no preview est chargé , ne pas l'effacer
		if(snap_default==0 || (old_snap_type!=snap_type))
		{	fastmemset(snap_vbuffer,0,SNAP_TXR_W*SNAP_TXR_H*2);					
			dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);			

			if(old_snap_type!=snap_type)
				snap_default=0;
		}		
				

	}
	else
	{
		if(snap_timer>SNAP_LOAD_DELAY && snap_flag==0)
		{
            // load all the snaps 
			char fname[256];
			snap_flag=1;
			snap_timer=0;			
			snap_count=0;
            
            for(snap_index=0;snap_index<SNAP_COUNT;snap_index++)
            {
                if(files[list_idx].flag & FILELIST_SMS)
    			{	sprintf(fname,"/cd/%s/sms/%s",SNAP_DIR,get_filename(cur_snap));}
    
    			if(files[list_idx].flag & FILELIST_GG)
    			{	sprintf(fname,"/cd/%s/gg/%s",SNAP_DIR,get_filename(cur_snap));}
    
    			if(files[list_idx].flag & FILELIST_DIR)
    			{	strcpy(fname,"FakeRead.pcx");
    			}    			
    			strcpy(my_strrchr(fname, '.'), "");    			
    			if(snap_index==0)
    			{   strcat(fname,".pcx");       
                }
                else
                {   strcat(fname,".");
    				sprintf(fname,"%spc%.1d",fname,snap_index);
                }                                               
                
                fastmemset(&snap_buffer[snap_index],0,SNAP_TXR_W*SNAP_TXR_H*2);	          
       			if(load_pcx(fname,&snap_buffer[snap_index])==0)
                {   break;                       
                }
                else
                {   
                    snap_count++;
                }				                
            }
            snap_index=0;
							
			if(snap_count==0)				
			{	
				// si le chargement foire	
                enabled_slideshow=0;
                						
				if(snap_default==0)
				{
					if(snap_type==TYPE_SMS)
					{	strcpy(fname,"/rd/smspreview.pcx");
					}
					else
					{	strcpy(fname,"/rd/ggpreview.pcx");
					}												
				
					if(LoadPcx2BufferNoMask(fname,snap_vbuffer)!=0)
					{	// preview chargé
						snap_default=1;													
					}
					else
					{	// ca a foiré , efface
						fastmemset(snap_vbuffer,0,SNAP_TXR_W*SNAP_TXR_H*2);	
					}
				}					
				
				dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);														
			}
			else
			{   
                // copy the first picture 
    		    dc_sq_cpy(txr_snap,&snap_buffer[0],SNAP_TXR_W*192*2);	
				snap_default=0;
				enabled_slideshow=1;
			}	
			
		}

        // counter before loading the files
		if(snap_flag==0)
		{	snap_timer++;
		}
		else
		{    		 
            if(snap_count>1)
            {
                // cycles
                snap_timer++;
                if(snap_timer>SNAP_SLIDESHOW_DELAY)
                {
                    // copie texture
                    snap_timer=0;
                    snap_index++;                
                    dc_sq_cpy(txr_snap,&snap_buffer[snap_index%snap_count],SNAP_TXR_W*192*2);	
                }               

            }  
        }
	}
}
*/
////////////////////////////////////////////////////////////////////////////////
/*
void gui_snap_load_multi(void)
{	
	// si j'ai changer 
	if(stricmp(cur_snap,files[list_idx].filename)!=0)
	{		
		strcpy(cur_snap,files[list_idx].filename);
		snap_timer=0;
		snap_flag=0;
		snap_count=-1;
		snap_slideshow=0;
		enabled_slideshow=0;
		
		old_snap_type=snap_type;
	
		if((files[list_idx].flag & FILELIST_GG))
		{	snap_type=TYPE_GG;
		}
		
		if((files[list_idx].flag & FILELIST_SMS))
		{	snap_type=TYPE_SMS;
		}

		if((files[list_idx].flag & FILELIST_DIR))
		{

			if(stricmp(cur_snap,"GG")==0)
			{	snap_type=TYPE_GG;}
			else
			{	if(stricmp(cur_snap,"SMS")==0)
				{	snap_type=TYPE_SMS;}
				else
				{	snap_type=old_snap_type;}	
			}			
		}

		// si le no preview est chargé , ne pas l'effacer
		if(snap_default==0 || (old_snap_type!=snap_type))
		{	fastmemset(snap_vbuffer,0,SNAP_TXR_W*SNAP_TXR_H*2);					
			dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);			

			if(old_snap_type!=snap_type)
				snap_default=0;
		}		
				

	}
	else
	{
		if(snap_timer>SNAP_LOAD_DELAY)
		{
			char fname[256];
			snap_flag=1;
			snap_timer=0;			

			if(files[list_idx].flag & FILELIST_SMS)
			{	sprintf(fname,"/cd/%s/sms/%s",SNAP_DIR,get_filename(cur_snap));							
			}

			if(files[list_idx].flag & FILELIST_GG)
			{	sprintf(fname,"/cd/%s/gg/%s",SNAP_DIR,get_filename(cur_snap));}

			if(files[list_idx].flag & FILELIST_DIR)
			{	strcpy(fname,"/rd/FakeRead.pcx");
			}
		
			strcpy(my_strrchr(fname, '.'), "");

			if(snap_count==-1)
			{	strcat(fname,".pcx");}
			else
			{	strcat(fname,".");
				sprintf(fname,"%s%.3d",fname,snap_count);						
			}
													
			if(load_pcx(fname,0)==0)				
			{					
				snap_slideshow=0;
				snap_default=0;

				if(snap_count==0)
				{	enabled_slideshow=0;
					snap_default=-1;
				}
				
				if(snap_count>0)
				{	
					enabled_slideshow=1;
					snap_default=-1;
					snap_count=-1;

					if(!(files[list_idx].flag & FILELIST_DIR))
					{								
						if(files[list_idx].flag & FILELIST_SMS)
						{	sprintf(fname,"/cd/%s/sms/%s",SNAP_DIR,get_filename(cur_snap));}

						if(files[list_idx].flag & FILELIST_GG)
						{	sprintf(fname,"/cd/%s/gg/%s",SNAP_DIR,get_filename(cur_snap));}

						if(files[list_idx].flag & FILELIST_DIR)
						{	strcpy(fname,"/rd/FakeRead.pcx");
						}
					
						strcpy(my_strrchr(fname, '.'), "");					
						strcat(fname,".pcx");					
						if(load_pcx(fname,0)==0)
						{	snap_default=0;
						}
						else
						{	dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);}
						
						snap_count=0;	
					}
	
				}
				
				if(files[list_idx].flag & FILELIST_DIR)
				{	enabled_slideshow=0;
					snap_default=0;					
				}
												
				// si le chargement foire							
				if(snap_default==0)
				{					
					if(snap_type==TYPE_SMS)
					{	strcpy(fname,"/rd/smspreview.pcx");
					}
					else
					{	strcpy(fname,"/rd/ggpreview.pcx");
					}												
				
					if(LoadPcx2BufferNoMask(fname,snap_vbuffer)!=0)
					{	// preview chargé
						snap_default=1;										
						dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);	
					}
					else
					{	// ca a foiré , efface
						fastmemset(snap_vbuffer,0,SNAP_TXR_W*SNAP_TXR_H*2);	
						dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);	
					}		
				}			
				
				if(snap_default==-1) snap_default==0;

				
				
			}
			else
			{	
				enabled_slideshow=1;

				if(snap_count<999)
					snap_count++;
				else
					snap_count=-1;

				snap_default=0;	
				snap_slideshow=0;
				dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);	
			}
		
															
			
		}

		if(snap_flag==0)
		{	snap_timer++;
		}
		else
		{	
			if(enabled_slideshow==1)
			{
				snap_slideshow++;
				if(snap_slideshow>150)
				{	snap_timer=SNAP_LOAD_DELAY+150;				
				}
			}
		}
	}
}
*/
//-----------------------------------------------------------------------------
// 

void gui_snap_load(void)
{
	// si j'ai changer 
	if(stricmp(cur_snap,files[list_idx].filename)!=0)
	{		
		strcpy(cur_snap,files[list_idx].filename);
		snap_timer=0;
		snap_flag=0;
		
		old_snap_type=snap_type;
	
		if((files[list_idx].flag & FILELIST_GG))
		{	snap_type=TYPE_GG;
		}
		
		if((files[list_idx].flag & FILELIST_SMS))
		{	snap_type=TYPE_SMS;
		}

		if((files[list_idx].flag & FILELIST_DIR))
		{

			if(stricmp(cur_snap,"GG")==0)
			{	snap_type=TYPE_GG;}
			else
			{	if(stricmp(cur_snap,"SMS")==0)
				{	snap_type=TYPE_SMS;}
				else
				{	snap_type=old_snap_type;}	
			}			
		}

		// si le no preview est chargé , ne pas l'effacer
		if(snap_default==0 || (old_snap_type!=snap_type))
		{	fastmemset(snap_vbuffer,0,SNAP_TXR_W*SNAP_TXR_H*2);					
			dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);			

			if(old_snap_type!=snap_type)
				snap_default=0;
		}		
				

	}
	else
	{
		if(snap_timer>SNAP_LOAD_DELAY)
		{
			char fname[256];
			snap_flag=1;
			snap_timer=0;			

			if(files[list_idx].flag & FILELIST_SMS)
			{	sprintf(fname,"/cd/%s/sms/%s",SNAP_DIR,get_filename(cur_snap));}

			if(files[list_idx].flag & FILELIST_GG)
			{	sprintf(fname,"/cd/%s/gg/%s",SNAP_DIR,get_filename(cur_snap));}

			if(files[list_idx].flag & FILELIST_DIR)
			{	strcpy(fname,"FakeRead.pcx");
			}
		
			strcpy(my_strrchr(fname, '.'), "");
			strcat(fname,".pcx");
							
			if(load_pcx(fname,snap_vbuffer)==0)				
			{	

#ifdef DDD
    	       // try snap dir
       
           		if(files[list_idx].flag & FILELIST_SMS)
	       		{	sprintf(fname,"/cd/%s/sms/%s",SNAP_DIR_NAME,get_filename(cur_snap));}

		      	if(files[list_idx].flag & FILELIST_GG)
    			{	sprintf(fname,"/cd/%s/gg/%s",SNAP_DIR_NAME,get_filename(cur_snap));}
    			
    			strcpy(my_strrchr(fname, '.'), "");
        		strcat(fname,".pcx");
        		
        		if(load_pcx(fname,snap_vbuffer)==0)				
    			{	
        		
#endif    
    				// si le chargement foire							
    				if(snap_default==0)
    				{
    					if(snap_type==TYPE_SMS)
    					{	strcpy(fname,"/rd/smspreview.pcx");
    					}
    					else
    					{	strcpy(fname,"/rd/ggpreview.pcx");
    					}												
    				
    					if(LoadPcx2BufferNoMask(fname,snap_vbuffer)!=0)
    					{	// preview chargé
    						snap_default=1;													
    					}
    					else
    					{	// ca a foiré , efface
    						fastmemset(snap_vbuffer,0,SNAP_TXR_W*SNAP_TXR_H*2);	
    					}
    				}						
#ifdef DDD
                }
                else
    			{
	       			snap_default=0;
		      	}				
#endif		
				
			}
			else
			{
				snap_default=0;
			}
		

			dc_sq_cpy(txr_snap,snap_vbuffer,SNAP_TXR_W*192*2);														

		}

		if(snap_flag==0)
			snap_timer++;
	}
}

//-----------------------------------------------------------------------------
// 

void draw_snapshot(void)
{	
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

	if(snap_type==TYPE_SMS)
	{	snap_pos.h=280;
		snap_pos.w=302;
	}
	else
	{	snap_pos.h=374;
		snap_pos.w=484;
	}

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, SNAP_TXR_W, SNAP_TXR_H, txr_snap, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = vertex_color_value=PVR_PACK_COLOR(vertex_color, vertex_color, vertex_color, vertex_color);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;
    
    vert.x = snap_pos.x;
    vert.y = snap_pos.y;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = snap_pos.x + snap_pos.w;
    vert.y = snap_pos.y;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 0.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = snap_pos.x;
    vert.y = snap_pos.y + snap_pos.h;
    vert.z = 1;
    vert.u = 0.0;
    vert.v = 1.0;
    pvr_prim(&vert, sizeof(vert));
    
    vert.x = snap_pos.x + snap_pos.w;
    vert.y = snap_pos.y + snap_pos.h;
    vert.z = 1;
    vert.u = 1.0;
    vert.v = 1.0;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}

//-----------------------------------------------------------------------------
// I got here a troubles with free quite weird :\

int load_pcx(char* filename,unsigned short *buffer)
{
	unsigned int	fd;
	pcx_hdr			*pcx_header;
	unsigned char	pcx_file[0x10000];
	unsigned char   *pcx;
	int	j;
	unsigned char	byte;
	int w,h;
	int i,numbytes,runlen,cnt,size;
	uint16	pcx_pal[256];
	uint8	data[256*256];

	cnt=0;

	//-- check if a pcx --
	//if (stricmp(strrchr(filename, '.'), ".pcx") != 0)
	//{	return 0;}

	//-- open file --
	fd=fs_open(filename, O_RDONLY);
	if(fd==0)
	{	return 0;}

	//-- Get the length of the file --
	size=fs_total(fd);	
	if(size>0x10000) return 0;		

	//-- read header --
	pcx= (unsigned char*) pcx_file;
	if(!pcx)
	{	fs_close ( fd );
		return 0;
	}	

	if(fs_read(fd,pcx, size)!=size)
	{	fs_close ( fd );
		return(0);	
	}

	fs_close ( fd );

	pcx_header = (pcx_hdr*) (pcx);

	//-- check of attributes --
	if(pcx_header->Bpp!=8 || pcx_header->ClrPlanes>1)
	{	return 0;}

	w=pcx_header->Xmax+1;
	h=pcx_header->Ymax+1;

	if(w>256 || h>192) 
	{	return 0;}

	//-- allocate the memory --
	numbytes= (w * h);		

	size=numbytes;

	//-- skip header --
	pcx= pcx+ sizeof(pcx_hdr);

	// RLE decoding
	do
	{
		byte = *pcx++;

		if ((byte & 0xc0) == 0xc0)
		{
			runlen = (byte & 0x3f);
			byte = *pcx++;

			while(runlen--)
				data[cnt++] = byte;
		}
		else
			data[cnt++] = byte;
	}
	while (cnt < numbytes);

	//-- Load the palette --
	pcx++;

	//-- load the pal --
	for (i=0; i<256; i++)
	{	pcx_pal[i]= ARGB1555(1,pcx[i*3+0],pcx[i*3+1],pcx[i*3+2]);
	}

	// convertit
	for(i=0;i<h;i++)
	{	
		for(j=0;j<w;j++)
			buffer[(i*256)+j]=pcx_pal[data[(i*w)+j]];
	}

	//-- close the pcx --
	return 1;

}

// Adapted from Allegro library, credits goes to Shawn

int LoadPcx2Buffer(char* filename,unsigned short *buffer)
{
	unsigned int	fd;
	pcx_hdr			*pcx_header;
	unsigned char	pcx_file[0x10000];
	unsigned char   *pcx;
	int	j;
	unsigned char	byte;
	int w,h;
	int i,runlen,cnt,size;
	uint16	pcx_pal[256];		
	uint8	*data=NULL;
	int		x,xx,y,po;
	int     bytes_per_line=0;
	unsigned char bpp ;

	cnt=0;

	//-- check if a pcx --
	if (stricmp(my_strrchr(filename, '.'), ".pcx") != 0)
	{	return 0;}

	//-- open file --
	fd=fs_open(filename, O_RDONLY);
	if(fd==0)
	{	return 0;}

	//-- Get the length of the file --
	size=fs_total(fd);	
	if(size>0x10000) return 0;		

	//-- read header --
	pcx= (unsigned char*) pcx_file;
	if(!pcx)
	{	fs_close ( fd );
		return 0;
	}	

	if(fs_read(fd,pcx, size)!=size)
	{	fs_close ( fd );
		return(0);	
	}

	fs_close ( fd );

	pcx_header = (pcx_hdr*) (pcx);
	
	w=pcx_header->Xmax+1;
	h=pcx_header->Ymax+1;
	bytes_per_line=pcx_header->Bpl;
	bpp== pcx_header->ClrPlanes *8 ;
	
	// allocate memory
	//-- check of attributes --
	if(pcx_header->Bpp!=8 || !(pcx_header->ClrPlanes==1 || pcx_header->ClrPlanes==3)  )
	{	return 0;}	
	
	if(pcx_header->ClrPlanes==1)
		data=malloc(w*h*pcx_header->ClrPlanes);
	
	
	//-- skip header --
	pcx= pcx+ sizeof(pcx_hdr);

	
	// RLE decoding
    for(y=0;y<h;y++)
    {   x=xx=0;
        po = 0;
        while (x < (bytes_per_line*pcx_header->ClrPlanes))
        {
            byte = *pcx++;
            if ((byte & 0xC0) == 0xC0)
            {   runlen = (byte & 0x3F);
                byte = *pcx++;
            }
            else
            {   runlen=1;
            }

            if(pcx_header->ClrPlanes==1)
            {   while (runlen--)
                {   if (x < w)
                        data[(y*w)+x] = byte;
                    x++;
	            }
            }
            else
            {
                while (runlen--)
	 	        {
                    // red
	    	        if (xx < w)
                    {
                        switch(po)
                        {
                            case 0:
                            {   buffer[(y*w)+(xx)] = ((byte>>3) & 0x1f)<<10;
                            }break;

                            case 1:
                            {   buffer[(y*w)+(xx)] |=  (byte>>3<<5);
                            }break;

                            case 2:
                            {   buffer[(y*w)+(xx)] |= ((byte>>3) & 0x1f);

								if(buffer[(y*w)+(xx)]!= 0x7c1f)
								{	buffer[(y*w)+(xx)] |=0x8000;									
								}

                            }break;
                        }
                    }

        	        x++;
        	        if (x == bytes_per_line)
	                {
                        xx = 0;
            		  	po = 1;
        	        }
	                else
        	        {
                        if (x == bytes_per_line*2)
            	      	{   xx = 0;
		  	    	        po = 2;
        	            }
	                    else
		  	                xx++;
                    }
                }
	        }
        }
    }


	if(pcx_header->ClrPlanes ==1)
	{
		//-- Load the palette --
		pcx++;

		//-- load the pal --
		for (i=0; i<256; i++)
		{	pcx_pal[i]= ARGB1555(1,pcx[i*3+0],pcx[i*3+1],pcx[i*3+2]);
		}

		// convertit
		for(i=0;i<h;i++)
		{	
			for(j=0;j<w;j++)
				buffer[(i*w)+j]= data[(i*w)+j]!=0 ? pcx_pal[data[(i*w)+j]] : 0;
		}
	}

	if(data)
		free(data);

	//-- close the pcx --
	return 1;

}


int LoadPcx2BufferNoMask(char* filename,unsigned short *buffer)
{
	unsigned int	fd;
	pcx_hdr			*pcx_header;
	unsigned char	pcx_file[0x20000];
	unsigned char   *pcx;
	int	j;
	unsigned char	byte;
	int w,h;
	int i,runlen,cnt,size;
	uint16	pcx_pal[256];		
	uint8	*data=NULL;
	int		x,xx,y,po;
	int     bytes_per_line=0;
	unsigned char bpp ;

	cnt=0;

	//-- check if a pcx --
	if (stricmp(my_strrchr(filename, '.'), ".pcx") != 0)
	{	return 0;}

	//-- open file --
	fd=fs_open(filename, O_RDONLY);
	if(fd==0)
	{	return 0;}

	//-- Get the length of the file --
	size=fs_total(fd);	
	if(size>0x20000) return 0;		

	//-- read header --
	pcx= (unsigned char*) pcx_file;
	if(!pcx)
	{	fs_close ( fd );
		return 0;
	}	

	if(fs_read(fd,pcx, size)!=size)
	{	fs_close ( fd );
		return(0);	
	}

	fs_close ( fd );

	pcx_header = (pcx_hdr*) (pcx);
	
	w=pcx_header->Xmax+1;
	h=pcx_header->Ymax+1;
	bytes_per_line=pcx_header->Bpl;
	bpp== pcx_header->ClrPlanes *8 ;

	if(h>192 || w>256) 
		return 0;
	
	// allocate memory
	//-- check of attributes --
	if(pcx_header->Bpp!=8 || !(pcx_header->ClrPlanes==1 || pcx_header->ClrPlanes==3)  )
	{	return 0;}	
	
	if(pcx_header->ClrPlanes==1)
		data=malloc(w*h*pcx_header->ClrPlanes);
	
	
	//-- skip header --
	pcx= pcx+ sizeof(pcx_hdr);

	
	// RLE decoding
    for(y=0;y<h;y++)
    {   x=xx=0;
        po = 0;
        while (x < (bytes_per_line*pcx_header->ClrPlanes))
        {
            byte = *pcx++;
            if ((byte & 0xC0) == 0xC0)
            {   runlen = (byte & 0x3F);
                byte = *pcx++;
            }
            else
            {   runlen=1;
            }

            if(pcx_header->ClrPlanes==1)
            {   while (runlen--)
                {   if (x < w)
                        data[(y*w)+x] = byte;
                    x++;
	            }
            }
            else
            {
                while (runlen--)
	 	        {
                    // red
	    	        if (xx < w)
                    {
                        switch(po)
                        {
                            case 0:
                            {   buffer[(y*256)+(xx)] = ((byte>>3) & 0x1f)<<10;
                            }break;

                            case 1:
                            {   buffer[(y*256)+(xx)] |=  (byte>>3<<5);
                            }break;

                            case 2:
                            {   buffer[(y*256)+(xx)] |= ((byte>>3) & 0x1f);
								buffer[(y*256)+(xx)] |=0x8000;									

								/*if(buffer[(y*w)+(xx)]!= 0x7c1f)
								{	buffer[(y*w)+(xx)] |=0x8000;									
								}*/

                            }break;
                        }
                    }

        	        x++;
        	        if (x == bytes_per_line)
	                {
                        xx = 0;
            		  	po = 1;
        	        }
	                else
        	        {
                        if (x == bytes_per_line*2)
            	      	{   xx = 0;
		  	    	        po = 2;
        	            }
	                    else
		  	                xx++;
                    }
                }
	        }
        }
    }


	if(pcx_header->ClrPlanes ==1)
	{
		//-- Load the palette --
		pcx++;

		//-- load the pal --
		for (i=0; i<256; i++)
		{	pcx_pal[i]= ARGB1555(1,pcx[i*3+0],pcx[i*3+1],pcx[i*3+2]);
		}

		// convertit
		for(i=0;i<h;i++)
		{	
			for(j=0;j<w;j++)
				buffer[(i*256)+j]= pcx_pal[data[(i*w)+j]];
			//	buffer[(i*w)+j]= data[(i*w)+j]!=0 ? pcx_pal[data[(i*w)+j]] : 0;
		}
	}

	if(data)
		free(data);

	//-- close the pcx --
	return 1;

}
