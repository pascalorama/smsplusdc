
#include "shared.h"

/* Background drawing function */
void (*render_bg)(int line);

/* Pointer to output buffer */
byte *linebuf;

/* Internal buffer for drawing non 8-bit displays */
byte internal_buffer[0x100];

/* Precalculated pixel table */
word pixel[PALETTE_SIZE];

/* Pattern cache */
byte cache[0x20000];

/* Dirty pattern info */
byte vram_dirty[0x200];
byte is_vram_dirty;

/* Pixel look-up table */
byte lut[0x10000];

/* pixel lookup */
int sms_rgb_conv[4]={0,85,170,255};
int gg_rgb_conv[8] ={0,36,73,109,146,182,219,255};

/* Attribute expansion table */
dword atex[4] =
{
    0x00000000,
    0x10101010,
    0x20202020,
    0x30303030,
};

/* Display sizes */
int vp_vstart;
int vp_vend;
int vp_hstart;
int vp_hend;

int line_mask;
int line_inc;
int vp_height_index=0;

/* */
void update_cachen(int i);

/* Macros to access memory 32-bits at a time (from MAME's drawgfx.c) */

static __inline__ dword read_dword(void *address)
{
    if ((dword)address & 3)
	{
        return ( *((byte *)address) +
                (*((byte *)address+1) << 8)  +
                (*((byte *)address+2) << 16) +
                (*((byte *)address+3) << 24) );

	}
	else
        return *(dword *)address;
}

static __inline__ void write_dword(void *address, dword data)
{
	if( ((dword)address & 1)==0)
	{
		*((word *)address) =    data ;
        *((word *)address+1) = (data >> 16);
		return;
	}

    if ((dword)address & 3)
	{
            *((byte *)address) =    data;
            *((byte *)address+1) = (data >> 8);
            *((byte *)address+2) = (data >> 16);
            *((byte *)address+3) = (data >> 24);
		return;
  	}
  	else
        *(dword *)address = data;
}


/****************************************************************************/


/* Initialize the rendering data */
void render_init(void)
{
    int bx, sx, b, s, bp, bf, sf, c;

    /* Generate 64k of data for the look up table */
    for(bx = 0; bx < 0x100; bx += 1)
    {
        for(sx = 0; sx < 0x100; sx += 1)
        {
            /* Background pixel */
            b  = (bx & 0x0F);

            /* Background priority */
            bp = (bx & 0x20) ? 1 : 0;

            /* Full background pixel + priority + sprite marker */
            bf = (bx & 0x7F);

            /* Sprite pixel */
            s  = (sx & 0x0F);

            /* Full sprite pixel, w/ palette and marker bits added */
            sf = (sx & 0x0F) | 0x10 | 0x40;

            /* Overwriting a sprite pixel ? */
            if(bx & 0x40)
            {
                /* Return the input */
                c = bf;
            }
            else
            {
                /* Work out priority and transparency for both pixels */
                c = bp ? b ? bf : s ? sf : bf : s ? sf : bf;
            }

            /* Store result */
            lut[(bx << 8) | (sx)] = c;
        }
    }

    render_reset();
}


/* Reset the rendering data */
void render_reset(void)
{
    int i;

    /* Clear display bitmap */
    memset(bitmap.data, 0, bitmap.pitch * bitmap.height);

    /* Clear palette */
    for(i = 0; i < PALETTE_SIZE; i += 1)
    {
        palette_sync(i);
    }

    /* Invalidate pattern cache */
    is_vram_dirty = 1;
    memset(vram_dirty, 1, 0x200);
    memset(cache, 0, sizeof(cache));

    /* Set up viewport size */
    if(IS_GG)
    {
        vp_vstart = 24;
        vp_vend   = 168;
        vp_hstart = 6;
        vp_hend   = 26;
    }
    else
    {
        vp_vstart = 0;
        vp_vend   = 192;
        vp_hstart = 0;
        vp_hend   = 32;
    }

    /* Pick render routine */
    render_bg = IS_GG ? render_bg_gg : render_bg_sms;
}


/* Draw a line of the display */

extern	void sremap_8_to_16(word *p1,byte *p2,word *p3);

void render_line(int line)
{
    /* Ensure we're within the viewport range */
    if((line < vp_vstart) || (line >= vp_vend)) return;

    /* Point to current line in output buffer */
	linebuf = (bitmap.depth == 8) ? &bitmap.data[(line * bitmap.pitch)] : &internal_buffer[0];

    /* dirty hack so no need to change state saving & loading */
    if((vdp.reg[1] & 0x10)!=0 && (vdp.reg[0] & 0x2)!=0)
    {   line_mask=0xff;
        line_inc=16;
        
         if(IS_SMS) 
         {  vp_vend=224;
            vp_height_index=1;
         }
    }
    else
    {   line_mask=224;
        line_inc=0;   
        
         if(IS_SMS) 
         {  vp_vend=192;
            vp_height_index=0;
         }
    }

    /* Blank line */
//    if( (!(vdp.reg[1] & 0x40)) || (((vdp.reg[2] & 1) == 0) && (IS_SMS)) )
    if( (!(vdp.reg[1] & 0x40)) )
    {
        memset(linebuf + (vp_hstart << 3), BACKDROP_COLOR, BMP_WIDTH);
    }
    else
    {
        /* Draw background */
        render_bg(line);

        /* Draw sprites */
        render_obj(line);

        /* Blank leftmost column of display */
        if(vdp.reg[0] & 0x20)
        {
			memset(linebuf, BACKDROP_COLOR, 8);						
        }
    }

	remap_8_to_16(line-vp_vstart);
	
	 
}


/* Draw the Master System background */
void render_bg_sms(int line)
{
    int locked = 0;
    //int v_line = (line + vdp.reg[9]) % 224;
    int v_line =(line + vdp.reg[9] + line_inc) % line_mask;
    int v_row  = (v_line & 7) << 3;
    int hscroll = ((vdp.reg[0] & 0x40) && (line < 0x10)) ? 0 : (0x100 - vdp.reg[8]);
    int column = vp_hstart;
    word attr;
    word *nt = (word *)&vdp.vram[vdp.ntab + ((v_line >> 3) << 6)];
    int nt_scroll = (hscroll >> 3);
    int shift = (hscroll & 7);
    dword atex_mask;
    dword *cache_ptr;
    dword *linebuf_ptr = (dword *)&linebuf[0 - shift];
	int tile_num;
	
    /* Draw first column (clipped) */
    if(shift)
    {
        int x, c, a;

        attr = nt[(column + nt_scroll) & 0x1F];

		tile_num=attr & 0x1ff;
		if(vram_dirty[tile_num]) update_cachen(tile_num);

        a = (attr >> 7) & 0x30;

        for(x = shift; x < 8; x += 1)
        {
            c = cache[((attr & 0x7FF) << 6) | (v_row) | (x)];
            linebuf[(0 - shift) + (x) ] = ((c) | (a));
        }

        column += 1;
    }

    /* Draw a line of the background */
    for(; column < vp_hend; column++)
    {
        /* Stop vertical scrolling for leftmost eight columns */
        if((vdp.reg[0] & 0x80) && (!locked) && (column >= 24))
        {
            locked = 1;
            v_row = (line & 7) << 3;
            nt = (word *)&vdp.vram[((vdp.reg[2] << 10) & 0x3800) + ((line >> 3) << 6)];
        }

        /* Get name table attribute word */
        attr = nt[(column + nt_scroll) & 0x1F];

		tile_num=attr & 0x1ff;
		if(vram_dirty[tile_num]) update_cachen(tile_num);
		
        /* Expand priority and palette bits */
        atex_mask = atex[(attr >> 11) & 3];
		
        /* Point to a line of pattern data in cache */
        cache_ptr = (dword *)&cache[((attr & 0x7FF) << 6) | (v_row)];			
        
        /* Copy the left half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1)] , read_dword( &cache_ptr[0] ) | (atex_mask));

        /* Copy the right half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1) | (1)], read_dword( &cache_ptr[1] ) | (atex_mask));		
    }

    /* Draw last column (clipped) */
    if(shift)
    {
        int x, c, a;

        char *p = &linebuf[(0 - shift)+(column << 3)];

        attr = nt[(column + nt_scroll) & 0x1F];

		tile_num=attr & 0x1ff;
		if(vram_dirty[tile_num]) update_cachen(tile_num);

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
        a = (attr >> 7) & 0x30;
		
        for(x = 0; x < shift; x += 1)
        {
            c = cache[((attr & 0x7FF) << 6) | (v_row) | (x)];
            p[x] = ((c) | (a));
        }
    }
}


/* Draw the Game Gear background */
void render_bg_gg(int line)
{
    int v_line = (line + vdp.reg[9] + line_inc) % line_mask;
    int v_row  = (v_line & 7) << 3;
    int hscroll = (0x100 - vdp.reg[8]);
    int column;
    word attr;
    word *nt = (word *)&vdp.vram[vdp.ntab + ((v_line >> 3) << 6)];
    int nt_scroll = (hscroll >> 3);
    dword atex_mask;
    dword *cache_ptr;
    dword *linebuf_ptr = (dword *)&linebuf[0 - (hscroll & 7)];
	int tile_num;

    /* Draw a line of the background */
    for(column = vp_hstart; column <= vp_hend; column++)
    {
        /* Get name table attribute word */
        attr = nt[(column + nt_scroll) & 0x1F];

#ifndef LSB_FIRST
        attr = (((attr & 0xFF) << 8) | ((attr & 0xFF00) >> 8));
#endif
		tile_num=attr & 0x1ff;
		if(vram_dirty[tile_num]) update_cachen(tile_num);

        /* Expand priority and palette bits */
        atex_mask = atex[(attr >> 11) & 3];

        /* Point to a line of pattern data in cache */
        cache_ptr = (dword *)&cache[((attr & 0x7FF) << 6) | (v_row)];

        /* Copy the left half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1)] , read_dword( &cache_ptr[0] ) | (atex_mask));

        /* Copy the right half, adding the attribute bits in */
        write_dword( &linebuf_ptr[(column << 1) | (1)], read_dword( &cache_ptr[1] ) | (atex_mask));
    }
}


/* Draw sprites */
void render_obj(int line)
{
    int i;
	
    /* Sprite count for current line (8 max.) */
    int count = 0;

    /* Sprite dimensions */
    int width = 8;
    int height = (vdp.reg[1] & 0x02) ? 16 : 8;

    /* Pointer to sprite attribute table */
    byte *st = (byte *)&vdp.vram[vdp.satb];

    /* Adjust dimensions for double size sprites */
    if(vdp.reg[1] & 0x01)
    {
        width *= 2;
        height *= 2;
    }

    /* Draw sprites in front-to-back order */
    for(i = 0; i < 64; i++)
    {
        /* Sprite Y position */
        int yp = st[i];

        /* End of sprite list marker? */
        if(line_inc==0)
        {   if(yp == 208) return;}
        else
        {   yp-=16;}

        /* Actual Y position is +1 */
        yp += 1;

        /* Wrap Y coordinate for sprites > 240 */
        if(yp > 240) yp -= 256;

        /* Check if sprite falls on current line */
        if((line >= yp) && (line < (yp + height)))
        {
            byte *linebuf_ptr;

            /* Width of sprite */
            int start = 0;
            int end = width;

            /* Sprite X position */
            int xp = st[0x80 + (i << 1)];

            /* Pattern name */
            int n = st[0x81 + (i << 1)];
									
            /* Bump sprite count */
            count += 1;

            /* Too many sprites on this line ? */
            if((emu_cfg.sprite_limit) && (count == 9)) return;

            /* X position shift */
            if(vdp.reg[0] & 0x08) xp -= 8;

            /* Add MSB of pattern name */
            if(vdp.reg[6] & 0x04) n |= 0x0100;

            /* Mask LSB for 8x16 sprites */
            if(vdp.reg[1] & 0x02) n &= 0x01FE;

			int tile_num=n & 0x1ff;
			if(vram_dirty[tile_num]) update_cachen(tile_num);
			tile_num++;
			if( (vdp.reg[1] & 0x02) && vram_dirty[tile_num]) update_cachen(tile_num);


            /* Point to offset in line buffer */
            linebuf_ptr = (byte *)&linebuf[xp];

            /* Clip sprites on left edge */
            if(xp < 0)
            {
                start = (0 - xp);
            }

            /* Clip sprites on right edge */
            if((xp + width) > 256)        
            {
                end = (256 - xp);
            }

            /* Draw double size sprite */
            if(vdp.reg[1] & 0x01)
            {
                int x;
                byte *cache_ptr = (byte *)&cache[(n << 6) | (((line - yp) >> 1) << 3)];

                /* Draw sprite line */
                for(x = start; x < end; x += 1)
                {
                    /* Source pixel from cache */
                    byte sp = cache_ptr[(x >> 1)];
    
                    /* Only draw opaque sprite pixels */
                    if(sp)
                    {
                        /* Background pixel from line buffer */
                        byte bg = linebuf_ptr[x];
    
                        /* Look up result */
                        linebuf_ptr[x] = lut[(bg << 8) | (sp)];
    
                        /* Set sprite collision flag */
                        if(bg & 0x40) vdp.status |= 0x20;
                    }
                }
            }
            else /* Regular size sprite (8x8 / 8x16) */
            {
                int x;
                byte *cache_ptr = (byte *)&cache[(n << 6) | ((line - yp) << 3)];

                /* Draw sprite line */
                for(x = start; x < end; x += 1)
                {
                    /* Source pixel from cache */
                    byte sp = cache_ptr[x];
    
                    /* Only draw opaque sprite pixels */
                    if(sp)
                    {
                        /* Background pixel from line buffer */
                        byte bg = linebuf_ptr[x];
    
                        /* Look up result */
                        linebuf_ptr[x] = lut[(bg << 8) | (sp)];
    
                        /* Set sprite collision flag */
                        if(bg & 0x40) vdp.status |= 0x20;
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/* Update pattern cache with modified tiles */

void update_cache(void)
{
    int i, x, y, c;
    int b0, b1, b2, b3;
    int i0, i1, i2, i3;

    if(!is_vram_dirty) return;
    is_vram_dirty = 0;

    for(i = 0; i < 0x200; i += 1)
    {
		update_cachen(i);
    } 
}

//-----------------------------------------------------------------------------
/* Update pattern cache with needed tiles */

__inline void update_cachen(int i)
{
    int x, y, c;
    int b0, b1, b2, b3;
    int i0, i1, i2, i3;     

    vram_dirty[i] = 0;

	int i5=i<<5;
	int i6=i<<6;

    for(y = 0; y < 8; y += 1)
    {
		int idx= i5 | (y << 2);

		b0 = vdp.vram[idx];
		b1 = vdp.vram[idx|1];
		b2 = vdp.vram[idx|2];
		b3 = vdp.vram[idx|3];
       
        for(x = 0; x < 8; x += 1)
        {
			int offset=7-x;
			
            i0 = (b0 >> (offset)) & 1;
            i1 = (b1 >> (offset)) & 1;
            i2 = (b2 >> (offset)) & 1;
            i3 = (b3 >> (offset)) & 1;

            c = (i3 << 3 | i2 << 2 | i1 << 1 | i0);

            cache[0x00000 | (i6) | ((y  ) << 3) | (x  )] = c;
            cache[0x08000 | (i6) | ((y  ) << 3) | (offset)] = c;
            cache[0x10000 | (i6) | ((7-y) << 3) | (x  )] = c;
            cache[0x18000 | (i6) | ((7-y) << 3) | (offset)] = c;
        }
    }

}


//-----------------------------------------------------------------------------
//

/* Update a palette entry */
void palette_sync(int index)
{
    int r, g, b;

    if(IS_GG)
    {
        r = gg_rgb_conv[((vdp.cram[(index << 1) | 0] >> 1) & 7)];
        g = gg_rgb_conv[((vdp.cram[(index << 1) | 0] >> 5) & 7)];
        b = gg_rgb_conv[((vdp.cram[(index << 1) | 1] >> 1) & 7)];

		bitmap.pal.color[index][0] = r;
		bitmap.pal.color[index][1] = g;
		bitmap.pal.color[index][2] = b;

		pixel[index] = MAKE_PIXEL565(r, g, b);
    }
    else
    {
        r = sms_rgb_conv[((vdp.cram[index] >> 0) & 3)];
        g = sms_rgb_conv[((vdp.cram[index] >> 2) & 3)];
        b = sms_rgb_conv[((vdp.cram[index] >> 4) & 3)];		

		bitmap.pal.color[index][0] = r;
		bitmap.pal.color[index][1] = g;
		bitmap.pal.color[index][2] = b;

		pixel[index] = MAKE_PIXEL1555(r, g, b);
    }
		
    
	
	
    bitmap.pal.dirty[index] = bitmap.pal.update = 1;
	
}


//-----------------------------------------------------------------------------
// remap des couleurs

__inline void remap_8_to_16(int line)
{
    int length = BMP_WIDTH>>5;
    int ofs = BMP_X_OFFSET;
    int i=length;
	dword val;
	dword *ibuffer= &internal_buffer[BMP_X_OFFSET];			
	dword colors;
    word *p = (word *)&bitmap.data[(line * bitmap.pitch)];
	
	//for(i=0;i<length;i++)
	while(i-->0)
	{
		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];

		val =(*ibuffer++);
		*p++ = pixel[val & 0x1f];
		*p++ = pixel[(val >>8) & 0x1f];
		*p++ = pixel[(val >>16) & 0x1f];
		*p++ = pixel[(val >>24) & 0x1f];	
		
		
		
		
	}
}

