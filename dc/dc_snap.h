#ifndef __DC_SNAP_H__
#define __DC_SNAP_H__

#ifndef DDD
#define	SNAP_DIR		"snap"
#else
#define	SNAP_DIR		"smsplusdcv02/snap"
#endif

#define SNAP_DIR_NAME   "snap"

#define SNAP_TXR_W	256
#define	SNAP_TXR_H	256

extern	pvr_ptr_t		txr_snap;

extern	void draw_snapshot(void);
extern	void gui_snap_init(void);
extern	void gui_snap_load(void);

//extern	int load_pcx(char* filename,int type);
extern  int load_pcx(char* filename,unsigned short *buffer);
extern	int LoadPcx2Buffer(char* filename,unsigned short *buffer);
extern	int LoadPcx2BufferNoMask(char* filename,unsigned short *buffer);


#endif
