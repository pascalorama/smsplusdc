#ifndef _PCX_H_
#define _PCX_H_

typedef struct
{
	char			Mfg;               /* manufacturer, always 0xa0         */
	char			Ver;               /* encoder version number (5)        */
	char			Enc;               /* encoding code, always 1           */
	char			Bpp;               /* bits per pixel, 8 in mode 0x13    */
	unsigned short	Xmin,Ymin;         /* image origin, usually 0,0         */
	unsigned short	Xmax,Ymax;         /* image dimensions                  */
	unsigned short	Hres;              /* horizontal resolution value       */
	unsigned short	Vres;              /* vertical resolution value         */
	char			Pal[48];           /* palette (not in mode 0x13)        */
	char			Reserved;          /* who knows?                        */
	char			ClrPlanes;         /* number of planes, 1 in mode 0x13  */
	unsigned short	Bpl;               /* bytes per line, 80 in mode 0x13   */
	unsigned short	plType;            /* Grey or Color palette flag        */
	unsigned short	Hscreensize;	   /* hscreen siz 						*/
	unsigned short  Vscreensize;	   /*									*/		
	char			Filler[54];        /* Zsoft wanted a 128 byte header    */

}pcx_hdr;

#endif