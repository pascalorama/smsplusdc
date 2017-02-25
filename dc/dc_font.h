
#ifndef _UIFONT_H_
#define _UIFONT_H_

//extern void dc_font_draw_char(int ascii_code , unsigned short *buf,int pitch,unsigned short color);
void my_draw_char(int ascii_code , unsigned short *buf,int pitch,unsigned short color);
void my_draw_string(unsigned short *buffer, int width, char *str,unsigned short color) ;
void amiga_textout(unsigned short *buffer, int width, char *str,unsigned short color);


#endif /* _UIFONT_H_ */

