/* 
   2001 Takayama Fumihiko <tekezo@catv296.ne.jp>
*/

#ifndef _DC_VMU_H
#define _DC_VMU_H

#include "..\shared.h"

/* 
   The dirent_vmu and file_hdr_vmu types came from 
   vmuload01.0 (file dciwrap.c) by Dan Potter
   Original code can be found on this site:
   http://dcdev.allusion.net/softprj/
*/
 
/* VMU Directory Entry */
typedef struct {
	uint8	filetype;	/* 0x00 = no file; 0x33 = data; 0xcc = a game */
	uint8	copyprotect;	/* 0x00 = copyable; 0xff = copy protected */
	uint16	firstblk;	/* location of the first block in the file */
	char	filename[12];	/* there is no null terminator */
	uint8	cent;		/* these are all file creation date stuff, in BCD format */
	uint8	year;
	uint8	month;
	uint8	day;
	uint8	hour;
	uint8	min;
	uint8	sec;
	uint8	dow;		/* day of week (0 = monday, etc) */
	uint16	filesize;	/* size of the file in blocks */
	uint16	hdroff;		/* offset of header, in blocks from start of file */
	char	dummys[4];	/* unused */
} dirent_vmu;

/* ROM menu header */
typedef struct {
	char	desc_short[16];
	char	desc_long[32];
	char	app_id[16];
	uint16	icon_cnt;
        uint16  animation_speed;
	uint16	eyecatch_type;
	uint16	crc;
	uint32	file_bytes;
	char	reserved[20];
	uint16	palette[16];
	uint8	icon_bitmap[384];
} file_hdr_vmu;


typedef struct {
  int16 fatblock;
  int16 dirblock;
  int16 dirlength;
} root_block_info;


typedef struct {
  int16 dirblock;
  int16 direntry;
} file_dirent_info;


#ifdef __cplusplus
extern "C" {
#endif

/* low level interface */
int ndc_vmu_read_blocks (maple_device_t *dev, uint16 firstblk, uint8 *buffer, uint8 blocks);
int ndc_vmu_write_blocks (maple_device_t *dev, uint16 firstblk, uint8 *buffer, uint8 blocks);

int ndc_vmu_get_dirent (dirent_vmu *vmu, maple_device_t *dev, const char *filename);
int ndc_vmu_getall_dirent (dirent_vmu *entries, int *num_entries, maple_device_t *dev);

int ndc_vmu_check_free_blocks (uint8 *free_mem, maple_device_t *dev);
void ndc_vmu_do_crc (uint8 *buffer, uint16 bytes);
void ndc_vmu_create_vmu_header(uint8 *header,
                               const char *desc_short,
                               const char *desc_long,
                               uint16 filesize,
                               const uint8 *icon);

int ndc_vmu_allocate_file (maple_device_t *dev, const char *filename, uint8 blocks);
int ndc_vmu_remove_file(maple_device_t *dev, const char *filename);


/* high level interface */
int ndc_vmu_save(uint8 *src, uint32 src_len, maple_device_t *dev,
                 const char *filename,
                 const char *desc_short,
                 const char *desc_long, 
                 const uint8 *icon);
int ndc_vmu_load(uint8 *dst, uint32 *dst_len, maple_device_t *dev, const char *filename);

#ifdef __cplusplus
}
#endif

#endif


