

#include "shared.h"
#include "dc_shared.h"
#include "lcd_icons/vmu_icon_sms.h"

#include <bzlib/bzlib.h>

//-----------------------------------------------------------------------------
// Extern

extern t_sms sms;
extern t_vdp vdp;
extern SN76496 sn;

static const uint8 smsdc_icon[] = {
#include "icons/vmu_save_icon.h"
};

//-----------------------------------------------------------------------------
// Define


#define SRAM_SIZE	0x8000

char* get_filename_only(char* filename)
{
    int i=0;
	char r_file[256];
	if(!filename) return NULL;
	strcpy(r_file,filename);

	/*** exclude the directory ***/
	for (i = strlen(r_file) - 1 ; i >= 0 ; i--)
	{	if (r_file[i] == '\\' || r_file[i] == '/')
			break;
	}
	strcpy(filename, &r_file[i + 1]);
	return filename;
}

//-----------------------------------------------------------------------------
// SRAM to VMU

int sram_save_to_vmu (maple_device_t *dev)
{
    const uint8 *buf;
	uint buf_len;
	char vmu_filename[512];
	char desc_long[512];
    int ret=0;

	if(emu_cfg.auto_save==0)
	   return 0;

 	sprintf(vmu_filename,"%x%.1x",rom_crc,emu_cfg.sram_slot);
    if(sel_file!=NULL)
    {   sprintf(desc_long, "%s", sel_file);
        get_filename_only(desc_long);
    }

 	buf = &sms.sram[0];
	buf_len = SRAM_SIZE;
	
    ret=VMU_Save_Compressed(desc_long,vmu_filename,buf,buf_len,20);
	if(ret!=0)
    {
        timer_spin_sleep (1000);
    }

	return ret;
}

//-----------------------------------------------------------------------------
// VMU TO SRAM

int sram_load_from_vmu (maple_device_t *dev)
{
    uint8 buf[SRAM_SIZE];
	uint32 buf_len;
	char vmu_filename[512];
	int i;

	buf_len = SRAM_SIZE;

	// loading
	sprintf(vmu_filename,"%x%.1x",rom_crc,emu_cfg.sram_slot);
	
    i=VMU_Load_Compressed(vmu_filename,buf,buf_len);
    if(i==0)
	{  memcpy(sms.sram,buf,0x8000);
    }
    else
    {   timer_spin_sleep (1000);}

	

	return i;
}

//-----------------------------------------------------------------------------
// VMU Save State

int save_state(maple_device_t *dev)
{
	unsigned char buf[sizeof(t_state_header) + sizeof(t_vdp) + sizeof(t_sms) + sizeof(Z80_Regs) + sizeof(int) +  sizeof(SN76496)] __attribute__ ((aligned (32)));;;
	uint buf_len=sizeof(buf);
	char vmu_filename[512];
	char desc_long[512];
	int cnt=0;
    int i=0;

	t_state_header t;

 	// save context in a buffer
	strcpy(t.id,STATE_ID);
	t.version=STATE_VERSION;

	memcpy(&buf[0],&t,sizeof(t_state_header));
	cnt+=sizeof(t_state_header);

	memcpy(&buf[cnt],&vdp,sizeof(t_vdp));
	cnt+=sizeof(t_vdp);

	memcpy(&buf[cnt],&sms,sizeof(t_sms));
	cnt+=sizeof(t_sms);

	memcpy(&buf[cnt],Z80_Context,sizeof(Z80_Regs));
	cnt+=sizeof(Z80_Regs);
	memcpy(&buf[cnt],&after_EI,sizeof(int));
	cnt+=sizeof(int);
	memcpy(&buf[cnt],&sn,sizeof(SN76496));
    cnt+=sizeof(SN76496);

	sprintf(vmu_filename,"%.8xst%.1x",rom_crc,emu_cfg.state_slot);
    if(sel_file!=NULL)
    {   sprintf(desc_long, "%s", sel_file);
        get_filename_only(desc_long);
    }

    i=VMU_Save_Compressed(desc_long,vmu_filename,buf,cnt,25);
    if(i!=0)
    {   timer_spin_sleep (1000);
        return i;
    }
	return 0;
}

//-----------------------------------------------------------------------------
// VMU Save State

int load_state(maple_device_t *dev)
{
	unsigned char buf[512*200] 	__attribute__ ((aligned (32)));
	unsigned char *pbuf=buf;
	uint buf_len=sizeof(buf);
	char vmu_filename[512];
	char desc_long[512];
	int cnt=0;
    int i;

	cnt=0;

	// loading
	sprintf(vmu_filename,"%.8xst%.1x",rom_crc,emu_cfg.state_slot);

    i=VMU_Load_Compressed(vmu_filename,&buf[0],buf_len);
    if( i==0)
    {
        // buffer loaded
		t_state_header *t=(t_state_header*) pbuf;

			if(stricmp(t->id,STATE_ID)!=0)
			{
                // old state version
                memcpy(&vdp,pbuf,sizeof(t_vdp));
    			pbuf+=sizeof(t_vdp);

    			memcpy(&sms,pbuf,sizeof(t_sms_1));
    			pbuf+=sizeof(t_sms_1);

    			memcpy(Z80_Context,pbuf,sizeof(Z80_Regs));
    			pbuf+=sizeof(Z80_Regs);
    			memcpy(&after_EI,pbuf,sizeof(int));
    			pbuf+=sizeof(int);
    			memcpy(&sn,pbuf,sizeof(SN76496));

            }
            else
            {
                // skip header
                pbuf+=sizeof(t_state_header);

                memcpy(&vdp,pbuf,sizeof(t_vdp));
    			pbuf+=sizeof(t_vdp);

    			memcpy(&sms,pbuf,sizeof(t_sms));
    			pbuf+=sizeof(t_sms);

    			memcpy(Z80_Context,pbuf,sizeof(Z80_Regs));
    			pbuf+=sizeof(Z80_Regs);
    			memcpy(&after_EI,pbuf,sizeof(int));
    			pbuf+=sizeof(int);
    			memcpy(&sn,pbuf,sizeof(SN76496));

            }
			system_load_state(NULL);
            return 0;
    }

	return i;
}


//-----------------------------------------------------------------------------
// Save Config to VMU

int save_config(maple_device_t *dev)
{
	const uint8 *buf;
	uint buf_len;
	char vmu_filename[512];
	char desc_long[512];
	int i;

	buf = &emu_cfg;
	buf_len = sizeof(t_config);

    i=VMU_Save_Compressed("Config data","SMS.SYS",buf,buf_len,3);

	return 0;
}

//-----------------------------------------------------------------------------
// load Config from vmu

int load_config(maple_device_t *test)
{
	char vmu_filename[512];
    uint8 vmu_buf[512*200] __attribute__ ((aligned (32)));
	int i;
	t_config t;

	i=0;
	
	

 	// loading
    i=VMU_Load_Compressed("SMS.SYS",&vmu_buf[0],sizeof(t_config));
   
    if(i==0)
    {
       t_config1 *p =(t_config1*) vmu_buf;

        switch(p->version)
        {
            case 1:
            {   
                        memcpy(&emu_cfg,vmu_buf,sizeof(t_config1));
            }break;

            default:
            {   
                        memcpy(&emu_cfg,vmu_buf,sizeof(t_config));
            }
        }

        emu_cfg.version=CONFIG_VERSION;
    }

	return i;
}


////////////////////////////////////////////////////////////////////////////////
//
int vmu_load_bookmark()
{
	unsigned char buf[512*200] 	__attribute__ ((aligned (32)));
    char   my_files[MAX_FAV_ENTRIES][256];
	unsigned char *pbuf=buf;
	int buf_len=sizeof(buf);
	char vmu_filename[512];
	char desc_long[512];
	int cnt=0;
	int i=0;
	
  //  printf("load bookmark\n");

	for(i=0;i<MAX_FAV_ENTRIES;i++)
	{  strcpy(my_files[i],"");
    }

	// loading
	sprintf(vmu_filename,"SMSPLUS.FV0");
    cnt=VMU_Load_Compressed(vmu_filename,buf,buf_len);

    if(cnt==0)
    {
       

        // buffer loaded
	   memcpy(&my_files,pbuf,MAX_FAV_ENTRIES*256);
	   //loaded
		for(i=0;i<MAX_FAV_ENTRIES;i++)
        {   strcpy(fav_files[i].filename,my_files[i]);
  //          printf(" fichier %s\n",fav_files[i].filename);
        }
		return 0;
    }
		
    return 255;
}

////////////////////////////////////////////////////////////////////////////////
// save bookmark

int vmu_save_bookmark()
{
	const uint8 *buf;
	uint buf_len;
	char vmu_filename[512];
	char desc_long[512];
	int i;
	char   my_files[MAX_FAV_ENTRIES][256];
	
	// copie des fichier
	for(i=0;i<MAX_FAV_ENTRIES;i++)
	{   if(fav_files[i].flag & FILEENTRY_LOCKED)
        {   strcpy(my_files[i],fav_files[i].filename);
        }
        else
        {   strcpy(my_files[i],"");}              
    }

	sprintf(vmu_filename,"SMSPLUS.FV0");
	sprintf(desc_long, "FAVORITES");

	buf = &my_files[0];
	buf_len = MAX_FAV_ENTRIES*256;

    i=VMU_Save_Compressed(desc_long,vmu_filename,buf,buf_len,5);

    if(i!=0)
    {   timer_spin_sleep (1000);}

	return i;
}

////////////////////////////////////////////////////////////////////////////////
//

int VMU_Load(char *filename,unsigned char*buff,int buff_size)
{
    int size=0;
   	int v=0;

    // Open the VMU directory
	file_t vd = fs_open("/vmu", O_RDONLY | O_DIR);
	if (vd == FILEHND_INVALID)
    {	return NO_VMU_FOUND;	}

	// Look through each attached VMU for an FoF system file
	dirent_t * d;
	file_t f = FILEHND_INVALID;
	while ( (d = fs_readdir(vd)) )
    {   char name[256];
        sprintf(name, "/vmu/%s/%s", d->name,filename);
        f = fs_open(name, O_RDONLY);
		if (f != FILEHND_INVALID)
		{   break;}
	}
	fs_close(vd);

	if (f == FILEHND_INVALID)
    {   return LOAD_FAILED	;}

    // Ok, we found one -- skip the header and load the data from it
	fs_seek(f, 128 + 512, SEEK_SET);

	size=fs_total(f);
	if(size<128+512)
	   return 0;
    else
        size -= 512+128;

	// load the buffer
    if(fs_read(f,&buff[0],size)!=size)
    {   return LOAD_FAILED;  }


    fs_close(f);

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
int VMU_Save(char* desc,char *filename,const uint8* buff,int buff_size,int minblocks)
{
    int i=0;

    //  VMU header
	vmu_pkg_t	pkg;
	uint8		*pkg_out;
	int	        pkg_size;
	uint16     *vmu_icon=(uint16*)smsdc_icon;

    // Find a VMU to save on
	maple_device_t * dev;
	if (!VMU_GetSavableOne(NULL, &dev,minblocks))
    {   return NO_VMU_FOUND;
	}

    // compute a block size
    memset(&pkg, 0, sizeof(pkg));
	strcpy(pkg.desc_short, "SMS+ System");
	strcpy(pkg.desc_long, desc);
	strcpy(pkg.app_id, "SMS Plus");
	pkg.icon_cnt = 1;
	pkg.icon_anim_speed = 0x0;
	pkg.eyecatch_type = VMUPKG_EC_NONE;
	pkg.eyecatch_data = NULL;
	pkg.data_len = buff_size;
	pkg.data =buff;

    // write palette
    for(i=0;i<16;i++)
    {   pkg.icon_pal[i] = vmu_icon[i];}

    pkg.icon_data = malloc(512);
	for (i=0; i<16*32; i++)
    {	pkg.icon_data[i] =smsdc_icon[32+i];}

    // Build the full VMU package
	vmu_pkg_build(&pkg, &pkg_out, &pkg_size);

	// Commit the changes
	if (vmufs_write(dev, filename, pkg_out, pkg_size, VMUFS_OVERWRITE) < 0)
    {   return SAVE_FAILED;
	}
	free(pkg_out);

	if(pkg.icon_data)
	   free(pkg.icon_data);

    return 0;
}


//-----------------------------------------------------------------------------
// standard VMU routine from Dan Potter Feet of fury 

int VMU_Save_Compressed(char *desc,char *filename,const uint8* buff,int buff_size,int minblocks)
{
    uint8 compressed_buf[512*200] 	__attribute__ ((aligned (32)));
    int compressed_size=sizeof(compressed_buf);
    int* i_buff = (int*)compressed_buf;
    int i=0;
    
    //  VMU header
	vmu_pkg_t	pkg;
	uint8		*pkg_out __attribute__ ((aligned (32)));
	int	        pkg_size;
    int         nbBlocks=0;
	uint16     *vmu_icon=(uint16*)smsdc_icon;
    maple_device_t *dev=NULL;
    int         remaining_blocks=0;
    int         saved_blocks=0;
    uint8       icon[512];
    int         freeblk=0;

    //BZ2_bzBuffToBuffCompress(&compressed_buf[4],&compressed_size,buff,buff_size,1,0,0);

    if(compress(&compressed_buf[4],&compressed_size,buff,buff_size)!=Z_OK)
    {
        return SAVE_FAILED;
    }
	i_buff[0]=compressed_size;
    nbBlocks=compressed_size/512;
    
    // pad the size
    if( ((compressed_size + 512 +4 + 128 ) % 512)!=0)
    {   compressed_size = ((compressed_size +512 + 4 + 128 ) / 512 + 1)*512;
        nbBlocks=compressed_size/512;
        compressed_size-=(128 + 512);
    }   
     
   
    // check first if the file exist
    saved_blocks=VMU_CheckFileExists(filename,&dev);
    timer_spin_sleep (200);                  
        
    if(saved_blocks==0)
    {   // Find a VMU to save on
        if (VMU_GetSavableOne(NULL, &dev,nbBlocks)==0)
        {        return NOT_ENOUGHT_BLOCKS;
    	}
    }
    else
    {
        if(nbBlocks>saved_blocks)
        {   if (VMU_GetSavableOne(NULL, &dev,(nbBlocks-saved_blocks))==0)
            {   return NOT_ENOUGHT_BLOCKS; }
            timer_spin_sleep (150);
        }
        else
        {
            if(nbBlocks<saved_blocks)
            {   if (VMU_GetSavableOne(NULL, &dev,nbBlocks)==0)
                {   return NOT_ENOUGHT_BLOCKS; }
                timer_spin_sleep (150);
            }

        }
    }

    if((dev)==NULL)
    {
        return NO_VMU_FOUND;
    }

    

    memset(&pkg, 0, sizeof(pkg));
	strcpy(pkg.desc_short, "SMS+ System");
	strcpy(pkg.desc_long, desc);
	strcpy(pkg.app_id, "SMS Plus");
	pkg.icon_cnt = 1;
	pkg.icon_anim_speed = 0x0;
	pkg.eyecatch_type = VMUPKG_EC_NONE;
	pkg.eyecatch_data = NULL;
	pkg.data_len = compressed_size;
	pkg.data =compressed_buf;

    // write palette
    for(i=0;i<16;i++)
    {   pkg.icon_pal[i] = vmu_icon[i];}
    
    pkg.icon_data = &icon[0];
	for (i=0; i<16*32; i++) 
    {	pkg.icon_data[i] =smsdc_icon[32+i];}

    // Build the full VMU package
	vmu_pkg_build(&pkg, &pkg_out, &pkg_size);

	// Commit the changes
	if (vmufs_write(dev, filename, pkg_out, pkg_size, VMUFS_OVERWRITE) < 0)
    {   return SAVE_FAILED;
	}

	free(pkg_out);
	

    return 0;
} 

//-----------------------------------------------------------------------------

int VMU_Load_Compressed(char *filename,unsigned char*buff,int buff_size)
{
     uint8 vmu_compressed_buf[512*200] 	__attribute__ ((aligned (32)));
    int size=0;
   	int* i_buff=(int*)vmu_compressed_buf;
    uint8* vmu=(uint8*)vmu_compressed_buf[4];
  	int compressed_size; 	
  	int v=0;
    vmu_pkg_t pkg;

    // Open the VMU directory
	file_t vd = fs_open("/vmu", O_RDONLY | O_DIR);
	if (vd == FILEHND_INVALID) 
    {	return NO_VMU_FOUND;	}
	
	// Look through each attached VMU for an FoF system file
	dirent_t * d;
	file_t f = FILEHND_INVALID;
	while ( (d = fs_readdir(vd)) ) 
    {   char name[256]; 
        sprintf(name, "/vmu/%s/%s", d->name,filename);
        f = fs_open(name, O_RDONLY);
		if (f != FILEHND_INVALID)
		{   break;}
	}
	fs_close(vd);
	
	if (f == FILEHND_INVALID) 
    {  // printf("pas trouvé\n");

        return LOAD_FAILED	;}
    
    // Ok, we found one -- skip the header and load the data from it
	fs_seek(f, 128 + 512, SEEK_SET);
	
	size=fs_total(f);

   	if(size<128+512)
	   return 0;
    else
        size -= 512+128;
    
	// load the buffer
    if(fs_read(f,&vmu_compressed_buf[0],size)!=size)
    {
   //     printf("erreur read\n");
        return LOAD_FAILED;
    }
    
    //BZ2_bzBuffToBuffDecompress(&buff[0],&buff_size,&vmu_compressed_buf[4],(int)i_buff[0],0,0);
    if(uncompress(&buff[0],&buff_size,&vmu_compressed_buf[4],(int)i_buff[0])!=Z_OK)
    {
     //  printf("erreur decompression\n");
       return LOAD_FAILED;}


    fs_close(f);

    return 0;
}

//-----------------------------------------------------------------------------
// standard VMU

int VMU_CheckFileExists(char *filename,maple_device_t ** devout)
{
    char* vmu[256];
    int size=0;
    int p,u;

    for (p=0; p<MAPLE_PORT_COUNT; p++)
    {
		for (u=0; u<MAPLE_UNIT_COUNT; u++)
        {
            maple_device_t * dev = maple_enum_dev(p, u);

            if (dev && (dev->info.functions & MAPLE_FUNC_MEMCARD))
            {
                file_t f = FILEHND_INVALID;
            	sprintf(vmu,"/vmu/%c%d/%s",(97+dev->port),dev->unit,filename);

                f = fs_open(vmu, O_RDONLY);
        		if (f != FILEHND_INVALID)
    	       	{
                    //found the file
                    *devout = dev;
                    size=fs_total(f);
                    fs_close(f);
                    return (size/512);
                }
                fs_close(f);
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
//

int VMU_GetFreeBlock(maple_device_t *dev)
{
    uint16 buf16[255];
   	int free_blocks=0,i=0;

   	if (dev && (dev->info.functions & MAPLE_FUNC_MEMCARD))
    {
        free_blocks=0;

        if(vmu_block_read(dev,255,(uint8*)buf16)==MAPLE_EOK)
        {
            if(vmu_block_read(dev,buf16[0x46/2],(uint8*)buf16)==MAPLE_EOK)
    		{
                for(i=0;i<200;++i)
                {   if(buf16[i]==0xfffc)
                      free_blocks++;
            	}
                return free_blocks;
                
            }
        }

    }
    return 0;
}

//-----------------------------------------------------------------------------
// standard VMU routine from Dan Potter Feet of fury 

int VMU_GetSavableOne(char *name, maple_device_t ** devout,int nbBlock) 
{   
    int p=0,u=0;
    uint16 buf16[256];
   	int free_blocks=0,i=0;
   	
   	for (p=0; p<MAPLE_PORT_COUNT; p++)
    {
		for (u=0; u<MAPLE_UNIT_COUNT; u++)
        {
			maple_device_t *dev = maple_enum_dev(p, u);
			if (dev && (dev->info.functions & MAPLE_FUNC_MEMCARD))
            {
                free_blocks=0;
                              
                if(vmu_block_read(dev,255,(uint8*)buf16)==MAPLE_EOK)
                {
                    if(vmu_block_read(dev,buf16[0x46/2],(uint8*)buf16)==MAPLE_EOK)
            		{
                        for(i=0;i<200;++i)
                        {   if(buf16[i]==0xfffc)
                              free_blocks++;
                    	}                  	                       
                                                     	                                                                                                                                                         	                    	
                        if(free_blocks>=nbBlock)
                        {
                           *devout = dev;
                            return 1;
                        }
                    }
                }
			}
		}
	}
	return 0;
}



//-----------------------------------------------------------------------------
// Draw icon on lcd

void vmu_draw_icon(int id)
{
	vmu_set_icon(vmu_icon_sms);
}
