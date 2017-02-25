#ifndef __CONFIG_H__
#define	__CONFIG_H__

typedef struct
{
	int x,y,w,h;
}t_screenpos;

extern t_screenpos screen_pos;

#define fFORCE_PAL	(0x1)
#define f3D_GLASSES	(0x2)
#define fPGA_HACK	(0x4)
#define fNO_SRAM	(0x8)

#define fLIGHT_PHASER	(0x10)
#define fABSOLUTE_PADDLE (0x20)
#define fCODE_MASTER    (0x40)

#define CONFIG_VERSION  2

typedef struct {		
	unsigned int crc;
	char *name;
	int	 flag;
} t_sms_crc;

typedef struct
{
	uint8	sprite_limit;
	uint8	sms_tv;
	uint8	bilinear;
	uint8	country;

	uint8	sram_slot;
	uint8	state_slot;
	uint8	invert_buttons;
	uint8	paint_backdrop;

	uint8	version;

	t_screenpos	pal_sms_pos;
	t_screenpos ntsc_sms_pos;
	t_screenpos gg_pos;
		
}t_config1;

typedef struct
{
	uint8	sprite_limit;
	uint8	sms_tv;
	uint8	bilinear;
	uint8	country;

	uint8	sram_slot;
	uint8	state_slot;
	uint8	invert_buttons;
	uint8	paint_backdrop;

	uint8	version;

   	uint8   auto_save;
    uint8   paddle_sensivity;
    uint8   fill;

	t_screenpos	pal_sms_pos;
	t_screenpos ntsc_sms_pos;
	t_screenpos gg_pos;
    

}t_config;



extern	t_config	emu_cfg;
extern	t_config	emu_cfg_o;
extern	t_sms_crc sms_db[];

//-----------------------------------------------------------------------------
//
extern	void default_config(void);
extern	void get_screen_pos(void);
extern	int check_config();

extern	void default_pal_sms_pos(t_screenpos *t);
extern	void default_ntsc_sms_pos(t_screenpos *t);
extern	void default_gg_pos(t_screenpos *t);

#endif
