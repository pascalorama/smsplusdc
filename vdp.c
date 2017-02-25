
#include "shared.h"
#include "hvc.h"


/* VDP context */
t_vdp vdp;

int vint_index[4]={0xc0,0xe0,0xf0,0};

/*--------------------------------------------------------------------------*/

/* Initialize VDP emulation */
void vdp_init(void)
{
    vdp_reset();
}


/* Reset VDP emulation */
void vdp_reset(void)
{
    memset(&vdp, 0, sizeof(t_vdp));
    
}


/* Write data to the VDP's control port */
void vdp_ctrl_w(int data)
{
    /* Waiting for the reset of the command? */
    if(vdp.pending == 0)
    {
        /* Save data for later */
        vdp.latch = data;

        /* Set pending flag */
        vdp.pending = 1;
    }
    else
    {
        /* Clear pending flag */
        vdp.pending = 0;

        /* VDP register write */
        if((data & 0xF0) == 0x80)
        {
            int r = (data & 0x0F);
            int d = vdp.latch;
			
            /* Store register data */
            vdp.reg[r] = d;

            /* Update table addresses */
            vdp.ntab = (vdp.reg[2] << 10) & 0x3800;
            vdp.satb = (vdp.reg[5] << 7) & 0x3F00;

            vdp.addr = vdp.code = 0;
            
            /* bigger height mode 224 */
            if( (vdp.reg[1] & 0x10)!=0 && (vdp.reg[0] & 0x2)!=0)
            {   vdp.ntab=0x700 +(((vdp.reg[2]>>2 )& 0x3)*0x1000);               
            }
                        						
        }
        else
        {
            /* Extract code bits */
            vdp.code = (data >> 6) & 3;

            /* Make address */
            vdp.addr = (data << 8 | vdp.latch);

            /* Read VRAM for code 0x00 */
            if(vdp.code == 0x00)
            {
                /* Load buffer with current VRAM byte */
                vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)];

                /* Bump address */
                vdp.addr += 1;
            }
        }
    }
}


/* Read the status flags */
int vdp_ctrl_r(void)
{
    /* Save the status flags */
    byte temp = vdp.status;

    /* Clear pending flag */
    vdp.pending = 0;

    /* Clear pending interrupt and sprite collision flags */
    vdp.status &= ~(0x80 | 0x40 | 0x20);

    /* Lower the IRQ line */
    if(sms.irq == 1)
    {
        sms.irq = 0;
        z80_set_irq_line(0, CLEAR_LINE);
    }

    /* Return the old status flags */
    return (temp);
}


/* Write data to the VDP's data port */
void vdp_data_w(int data)
{
    int index;

    /* Clear the pending flag */
    vdp.pending = 0;

    switch(vdp.code)
    {
        case 0: /* VRAM write */
        case 1: /* VRAM write */
        case 2: /* VRAM write */

            /* Get current address in VRAM */
            index = (vdp.addr & 0x3FFF);

            /* Only update if data is new */
            if(data != vdp.vram[index])
            {
                /* Store VRAM byte */
                vdp.vram[index] = data;

                /* Mark patterns as dirty */
                vram_dirty[(index >> 5)] = 1;
				is_vram_dirty = 1;

            }
            break;

        case 3: /* CRAM write */
            if(cart.type == TYPE_GG)
            {
                index = (vdp.addr & 0x3F);
                if(data != vdp.cram[index])
                {
					vdp.cram[index] = data;
                    index = (vdp.addr >> 1) & 0x1F;
                    palette_sync(index);					
                }
            }
            else
            {
                index = (vdp.addr & 0x1F);
                if(data != vdp.cram[index])
                {
                    vdp.cram[index] = data;
                    palette_sync(index);				
                }
            }
            break;
    }

    /* Bump the VRAM address */
    vdp.addr += 1;
}


/* Read data from the VDP's data port */
int vdp_data_r(void)
{
    byte temp = 0;

    /* Clear the pending flag */
    vdp.pending = 0;

    switch(vdp.code)
    {
        case 0: /* VRAM read */
        case 1: /* VRAM read */
        case 2: /* VRAM read */

            /* Return the buffered value */
            temp = vdp.buffer;

            /* Get data from the current VRAM address */
            vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)];
            break;

        case 3: /* Undefined */
            /* Return 'no value' data */
            temp = 0xFF;
            break;
    }

    /* Bump address register */
    vdp.addr += 1;
    return (temp);
}


/* Process frame events */
void vdp_run(void)
{
    int int1 = vint_index[vp_height_index];
    int int_limit = vint_index[vp_height_index+1];
    

    if(vdp.line <= int1)
    {
        if(vdp.line == 0)
        {
            vdp.left = vdp.reg[10];
        }

        if(vdp.left == 0)
        {
            vdp.left = vdp.reg[10];
            vdp.status |= 0x40;
        }
        else
        {
            vdp.left -= 1;
        }

        if((vdp.status & 0x40) && (vdp.reg[0] & 0x10))
        {
            sms.irq = 1;
            z80_set_irq_line(0, ASSERT_LINE);
        }
    }
    else
    {
		if(vdp.line == (int1 + 1))
        {	vdp.status |= 0x80;}


        vdp.left = vdp.reg[10];

        if((vdp.line < int_limit) && (vdp.status & 0x80) && (vdp.reg[1] & 0x20))
        {
            sms.irq = 1;
            z80_set_irq_line(0, ASSERT_LINE);
        }
    }
}


byte vdp_vcounter_r(void)
{
    if(sms_tv ==TV_NTSC)
        return  vc_table[vp_height_index][(vdp.line & 0x1FF)];
	else		
		return vc_table[4+vp_height_index][(vdp.line & 0x1FF)]; 

}


byte vdp_hcounter_r(void)
{
   int pixel = (((z80_ICount % CYCLES_PER_LINE) / 4) * 3) * 2;
   return hc_table[0][(pixel >> 1) & 0x1FF];
	
}

