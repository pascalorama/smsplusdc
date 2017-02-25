#ifndef __DC_EMU_H__
#define __DC_EMU_H__

extern	int	emu_join;
extern	int sram_crc;

extern	int	rom_flag;

extern	uint16 emu_vbuffer[256 * 256];

extern	pvr_ptr_t txr_emu;
extern  pvr_ptr_t txr_menu;

extern	void dc_emu_start(void);
extern	void dc_emu_init(void);
extern	void dc_drawframe(void);
extern	void dc_copyframe(void);
extern	void dc_sq_cpy(void *dest, void *src, int n);
extern	void dc_draw_menu_frame(void);
extern	void dc_txr_dma_cpy(void *dest, void *src, int n);

extern	int load_rom(char *filename);

extern	void check_and_save_sram();


#endif
