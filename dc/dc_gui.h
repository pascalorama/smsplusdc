#ifndef __DC_GUI_H__
#define	__DC_GUI_H__

#define	FILELIST_FILE	0x00
#define	FILELIST_DIR	0x01
#define	FILELIST_GG		0x02
#define	FILELIST_SMS	0x04
#define FILEENTRY_LOCKED    0x08

#define MAX_FILES_ENTRIES	1500
#define	MAX_FAV_ENTRIES		10

#define KEY_ASSERTED		(1)
#define	KEY_RELEASED		(2)
#define	KEY_PUSHED			(3)
#define	KEY_DELAY			(10)

#define TRIGGER_R			(0x01)
#define TRIGGER_L			(0x02)


#define SELECTION_COLOR		ARGB1555(1,255,255,0)
#define LIST_COLOR			ARGB1555(1,255,255,255)
#define LOADED_COLOR		ARGB1555(1,255,0,0)
#define STATUS_COLOR		ARGB1555(1,0,255,0)

#define SMS_COLOR			ARGB1555(1,255,255,255)
#define GG_COLOR			ARGB1555(1,255,255,255)

typedef struct
{
	char	filename[256];
	char	displayname[256];	
	unsigned char	flag;
}t_file_entries;

extern	t_file_entries *files;
extern	t_file_entries cd_files[MAX_FILES_ENTRIES];

extern	char	sel_filepath[256];
extern	char	sel_file[256];

extern	int		files_count;
extern	int		dirs_count;
extern	int		list_idx;
extern	int		list_count;
extern	int		list_selection;
extern	int		list_prev_idx;
extern	char	cur_dir[256];
extern	int		options_pos;
extern	int		emu_autobooted;
extern	int		fav_idx;

extern	uint8	list_mode;
extern	uint8	up_timer;
extern	uint8	down_timer;
extern	uint8	left_timer;
extern	uint8	right_timer;
extern	uint8	ltrig_timer;
extern	uint8	rtrig_timer;


extern	uint16 text_vbuffer[256 * 512];
extern	uint16 status_vbuffer[256 * 16];

extern	char	sel_file[256];

extern	pvr_ptr_t	txr_status;
extern	pvr_ptr_t	txr_bg;

extern	void gui_roms_screen(void);
extern	void gui_roms_screen_init(void);
extern	void gui_options_screen(void);
extern	int gui_get_directories(char *dirn);
extern  void gui_load_rom(char* filename);

extern	void fastmemcpy(uint32 *dest, uint32 *src, int bytes);
extern	void fastmemset(void *dest,int value, int bytes);

extern	uint16 check_buttons(uint16 keyvalue,uint16 keymask);
extern	uint16 check_triggers(uint8 keyvalue,uint8 keymask);

extern	char * my_strrchr(const char * s, int c);

extern	t_screenpos text_pos;
extern	t_screenpos selection_bar_pos;
extern	t_screenpos status_pos;

extern	void draw_text(void);
extern	void draw_selection_bar(void);
extern	void draw_status(void);

extern  int dsp_bookmark;

extern	char* get_filename(char* filename);
extern  void strupper ( char * str );

#endif
