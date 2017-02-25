
#ifndef _VMU_H_
#define _VMU_H_

#define NO_VMU_FOUND		(1)
#define NOT_ENOUGHT_BLOCKS	(2)
#define SAVE_FAILED			(3)
#define NO_FILE				(4)
#define LOAD_FAILED			(5)

#define STATE_ID            "SMS+"
#define STATE_VERSION       1

typedef struct
{
    char id[5];
    uint16  version;
}t_state_header;

typedef struct
{
    byte dummy[0x2000];
    byte ram[0x2000];
    byte sram[0x8000];
    byte fcr[4];
    byte paused;
    byte save;
    byte country;
    byte port_3F;
    byte port_F2;
    byte use_fm;
    byte irq;
    byte psg_mask;
	
	word lines;

}t_sms_1;

void vmu_draw_icon(int id);

extern	int sram_save_to_vmu (maple_device_t *dev);
extern	int sram_load_from_vmu (maple_device_t *dev);

extern	int save_state(maple_device_t *dev);
extern	int load_state(maple_device_t *dev);

extern	int load_config(maple_device_t *dev);
extern	int save_config(maple_device_t *dev);

extern  int vmu_save_bookmark();
extern  int vmu_load_bookmark();


extern int VMU_Load_Compressed(char *filename,unsigned char*buff,int buff_size);
extern int VMU_GetSavableOne(char *name, maple_device_t ** devout,int nbBlock);
extern int VMU_Save_Compressed(char* desc,char *filename,const uint8* buff,int buff_size,int minblocks);

extern int VMU_Load(char *filename,unsigned char*buff,int buff_size);
extern int VMU_Save(char *desc,char *filename,const uint8* buff,int buff_size,int minblocks);

extern int VMU_CheckFileExists(char *filename,maple_device_t ** devout);
extern int VMU_GetFreeBlock(maple_device_t *dev);

#endif
