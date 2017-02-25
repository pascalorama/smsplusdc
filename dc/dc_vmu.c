/* 
   2001 Takayama Fumihiko <tekezo@catv296.ne.jp>
*/

#include "stdlib.h"
//#include "dc_utils.h"
#include "dc_vmu.h"

const char *progname="SMSPLUS";

/* ============================================================ */
static void
create_filename(char *dst, const char *src)
{
  int i;
  int src_len = strlen (src);
  
  for (i = 0; i < 12; ++i) 
  {
    if (i < src_len)
      dst[i] = src[i];
    else
      dst[i] = ' ';
  }
}


static void
create_desc_short(char *dst, const char *src)
{
  int i;
  int src_len = strlen (src);
  
  for (i = 0; i < 16; ++i) 
  {
    if (i < src_len)
      dst[i] = src[i];
    else 
      dst[i] = ' ';
  }
}


static void
create_desc_long(char *dst, const char *src)
{
  int i;
  int src_len = strlen (src);
  
  for (i = 0; i < 32; ++i) 
  {
    if (i < src_len)
      dst[i] = src[i];
    else
      dst[i] = ' ';
  }
}


/* ============================================================ */
static int
get_root_block_info (root_block_info *rbi, maple_device_t *dev)
{
  uint16 buf16[512 / 2];
  int rv;
  
  rv = vmu_block_read (dev, 255, (uint8 *)buf16);
  
  if (rv != MAPLE_EOK) return rv;
  
  rbi->fatblock  = buf16[0x46 / 2];
  rbi->dirblock  = buf16[0x4a / 2];
  rbi->dirlength = buf16[0x4c / 2];
  
  return MAPLE_EOK;
}


static int
get_free_dirent_info (file_dirent_info *fdi, maple_device_t *dev)
{
  root_block_info rbi;
  int16 dirblock;
  int n;
  int rv;
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return rv;
  
  dirblock = rbi.dirblock;
  for (n = rbi.dirlength; n > 0; --n) 
  {
    dirent_vmu ent[16];
    int i;
    
    rv = vmu_block_read(dev, dirblock, (uint8 *)ent);
    if (rv != MAPLE_EOK) return rv;
    
    for (i = 0; i < 16; ++i) 
    {
      if (ent[i].filetype == 0x00)
      {
        fdi->dirblock = dirblock;
        fdi->direntry = i;
        return MAPLE_EOK;
      }
    }
    --dirblock;
  }
  return MAPLE_EFAIL;
}

static int get_file_dirent_info (file_dirent_info *fdi, maple_device_t *dev, const char *filename)
{
  char fname[12];
  root_block_info rbi;
  int16 dirblock;
  int n;
  int rv;
  
  create_filename(fname, filename);
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return rv;
  
  dirblock = rbi.dirblock;
  for (n = rbi.dirlength; n > 0; --n) 
  {
    dirent_vmu ent[16];
    int i;

    rv = vmu_block_read(dev, dirblock, (uint8 *)ent);
    if (rv != MAPLE_EOK) return rv;
    
    for (i = 0; i < 16; ++i) 
    {
      if (ent[i].filetype == 0x33 &&
          !strncmp(ent[i].filename, fname, 12))
      {
        file_hdr_vmu fhv;
        
        rv = vmu_block_read (dev, ent[i].firstblk, (uint8 *)(&fhv));
        if (rv != MAPLE_EOK) return rv;
        
        if (!strcmp (fhv.app_id, progname))
        {
          fdi->dirblock = dirblock;
          fdi->direntry = i;
          return MAPLE_EOK;
        }
      }
    }
    --dirblock;
  }
  return MAPLE_EFAIL;
}


/*  This rouine came from Marcus's website */
/*  http://mc.pp.se/dc/vms/fileheader.html */
static uint16 
calc_crc(const unsigned char *buf, int size)
{
  int i, c, n = 0;
  for (i = 0; i < size; i++)
  {
    n ^= (buf[i] << 8);
    for (c = 0; c < 8; c++)
      if (n & 0x8000)
	n = (n << 1) ^ 4129;
      else
	n = (n << 1);
  }
  return (n & 0xffff);
}


static int
allocate_blocks_on_fat (maple_device_t *dev, uint8 blocks) 
{
  int i;
  uint8 free_mem[200];
  int free_blocks;
  root_block_info rbi;
  uint16 fat_buf16[512 / 2];
  uint16 this_block = 0;
  uint8 max_block = blocks;
  int firstblk;
  int rv;
  
  if (!blocks) return -1;
  
  free_blocks = ndc_vmu_check_free_blocks (free_mem, dev);
  if (free_blocks < 0 || free_blocks < blocks) 
    return -1;
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return -1;
  rv = vmu_block_read(dev, rbi.fatblock, (uint8 *)fat_buf16);
  if (rv != MAPLE_EOK) return -1;
  
  firstblk = -1;
  for (i = 0; i < 200; ++i)
  {
    if (free_mem[199-i] == 1) 
    {
      if (blocks == max_block) 
      {
        /* first file block */
	this_block = firstblk = 199 - i;
      }
      else
      {
        /* need the next one */
	fat_buf16[this_block] = 199 - i;
	this_block = 199 - i;
      }
      
      --blocks;
      
      /* last file block */
      if (blocks == 0) 
      { 
        fat_buf16[this_block] = 0xfffa;
        break;
      }
    }
  }
  
  rv = vmu_block_write (dev, rbi.fatblock, (uint8 *)fat_buf16);
  if (rv != MAPLE_EOK) return -1;
  
  return firstblk;
}


/* ============================================================ */
int
ndc_vmu_read_blocks (maple_device_t *dev, uint16 firstblk, uint8 *buffer, uint8 blocks)
{
  int i, nread;
  root_block_info rbi;
  uint16 fat_buf16[512 / 2];
  int rv;
  
  if (!dev) return -1;
  if (!blocks) return -1;
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return -1;
  rv = vmu_block_read (dev, rbi.fatblock, (uint8 *)fat_buf16);
  if (rv != MAPLE_EOK) return -1;
  
  nread = 0;
  for (i = 0; i < blocks; ++i)
  {
    vmu_block_read (dev, firstblk, buffer);
    buffer += 512;
    ++nread;
    
    firstblk = fat_buf16[firstblk];
    if (firstblk == 0xfffa)
      break;
    if (firstblk == 0xfffc)
      return -1;
  }
  return nread;
}


int
ndc_vmu_write_blocks (maple_device_t *dev, uint16 firstblk, uint8 *buffer, uint8 blocks)
{
  int i, nwrite;
  root_block_info rbi;
  uint16 fat_buf16[512 / 2];
  int rv;
  
  if (!dev) return -1;
  if (!blocks) return -1;
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return -1;
  rv = vmu_block_read (dev, rbi.fatblock, (uint8 *)fat_buf16);
  if (rv != MAPLE_EOK) return -1;
  
  nwrite = 0;
  for (i = 0; i < blocks; ++i)
  {
    vmu_block_write (dev, firstblk, buffer);
    buffer += 512;
    ++nwrite;
    
    firstblk = fat_buf16[firstblk];
    if (firstblk == 0xfffa)
      break;
    if (firstblk == 0xfffc)
      return -1;
  }
  return nwrite;
}


int
ndc_vmu_get_dirent (dirent_vmu *vmu, maple_device_t *dev, const char* filename)
{
  file_dirent_info fdi;
  dirent_vmu ent[16];
  int rv;
  
  if (!dev) return -1;
  
  rv = get_file_dirent_info (&fdi, dev, filename);
  if (rv != MAPLE_EOK) return -1;
  
  rv = vmu_block_read (dev, fdi.dirblock, (uint8 *)ent);
  if (rv != MAPLE_EOK) return -1;
  
  if (vmu)
    memcpy(vmu, ent + fdi.direntry, sizeof(dirent_vmu));
  return ent[fdi.direntry].firstblk;
}


int
ndc_vmu_getall_dirent (dirent_vmu *entries, int *num_entries, maple_device_t *dev)
{
  root_block_info rbi;
  int16 dirblock;
  int n;
  int nwrite;
  int rv;
  
  if (!dev) return -1;
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return -1;
  
  dirblock = rbi.dirblock;
  nwrite = 0;
  for (n = rbi.dirlength; n > 0; --n) 
  {
    dirent_vmu ent[16];
    
    rv = vmu_block_read(dev, dirblock, (uint8 *)ent);
    if (rv != MAPLE_EOK) return -1;
    
    if (nwrite < *num_entries)
    {
      int blocks;
      if (*num_entries - nwrite > 16)
        blocks = 16;
      else
        blocks = *num_entries - nwrite;
      
      memcpy (entries, ent, sizeof(dirent_vmu) * blocks);
      entries += blocks;
      nwrite += blocks;
    }
    
    --dirblock;
  }
  *num_entries = nwrite;
  
  return 0;
}


int
ndc_vmu_check_free_blocks (uint8 *free_mem, maple_device_t *dev)
{
  root_block_info rbi;
  uint16 fat_buf16[512 / 2];
  int free_blocks;
  int i;
  int rv;
  
  if (!dev) return -1;
  
  rv = get_root_block_info(&rbi, dev);
  if (rv != MAPLE_EOK) return -1;
  rv = vmu_block_read (dev, rbi.fatblock, (uint8 *)fat_buf16);
  if (rv != MAPLE_EOK) return -1;
  
  if (free_mem)
    memset(free_mem, 0, 200);
  free_blocks = 0;
  for (i = 0; i < 200; ++i) 
  {
    if (fat_buf16[i] == 0xfffc) 
    {
      free_blocks++;
      if (free_mem)
        free_mem[i] = 1;
    }
  }
  
  return free_blocks;
}


void
ndc_vmu_do_crc (uint8 *buffer, uint16 bytes) 
{
  uint16 crc = calc_crc (buffer, bytes);
  file_hdr_vmu *hdr = (file_hdr_vmu *)buffer;
  
  hdr->crc = crc;
}


void 
ndc_vmu_create_vmu_header(uint8 *header, const char *desc_short, 
                          const char *desc_long, uint16 filesize,
                          const uint8 *icon) 
{
  file_hdr_vmu *hdr = (file_hdr_vmu *)header;
  
  memset(header, 0, 640);
  create_desc_short (hdr->desc_short, desc_short);
  create_desc_long (hdr->desc_long, desc_long);
  strcpy (hdr->app_id, progname);
  
  hdr->icon_cnt = 1;
  hdr->file_bytes = filesize;
  
  if (icon)
    memcpy(hdr->palette, icon, 32 + 512);
  else
  {
    memset(hdr->palette, 0, 32 + 512);
  }
}


int
ndc_vmu_allocate_file (maple_device_t *dev, const char *filename, uint8 blocks)
{
  int firstblk;
  int free_blocks;
  
  if (!dev) return -1;
  if (!blocks) return -1;
  
  free_blocks = ndc_vmu_check_free_blocks (NULL, dev);
  if (free_blocks < 0 || free_blocks < blocks)
    return -1;
  
  firstblk = allocate_blocks_on_fat (dev, blocks);
  if (firstblk < 0)
    return -1;
  
  {
    /* update directory entry */
    dirent_vmu dir_entry;
    file_dirent_info fdi;
    dirent_vmu ent[16];
    
    memset(&dir_entry, 0, sizeof(dir_entry));
    dir_entry.filetype = 0x33;
    dir_entry.copyprotect = 0x00;
    dir_entry.firstblk = firstblk;
    create_filename (dir_entry.filename, filename);
    dir_entry.filesize = blocks;
    dir_entry.hdroff = 0;
    
    if (get_free_dirent_info (&fdi, dev) != MAPLE_EOK) return -1;
    if (vmu_block_read (dev, fdi.dirblock, (uint8 *)ent) != MAPLE_EOK) return -1;
    
    memcpy (ent + fdi.direntry, &dir_entry, sizeof(dirent_vmu));
    if (vmu_block_write (dev, fdi.dirblock, (uint8 *)ent) != MAPLE_EOK) return -1;
  }
  
  return firstblk;
}


int
ndc_vmu_remove_file(maple_device_t *dev, const char *filename) 
{
  int16 firstblk = -1;
  
  if (!dev) return -1;
  if (!filename) return -1;
  
  {
    /* remove directory entry */
    file_dirent_info fdi;
    dirent_vmu ent[16];
    
    if (get_file_dirent_info (&fdi, dev, filename) != MAPLE_EOK) return -1;
    if (vmu_block_read (dev, fdi.dirblock, (uint8 *)ent) != MAPLE_EOK) return -1;
    
    firstblk = ent[fdi.direntry].firstblk;
    
    memset(ent + fdi.direntry, 0, sizeof(dirent_vmu));
    if (vmu_block_write (dev, fdi.dirblock, (uint8 *)ent) != MAPLE_EOK) return -1;
  }
  
  {
    /* remove fat entry */
    root_block_info rbi;
    uint16 fat_buf16[512 / 2];
    int16 block = firstblk;
    
    if (get_root_block_info(&rbi, dev) != MAPLE_EOK) return -1;
    if (vmu_block_read (dev, rbi.fatblock, (uint8 *)fat_buf16) != MAPLE_EOK) return -1;
    
    while (fat_buf16[block] != 0xfffa) 
    {
      uint16 bak = fat_buf16[block];
      fat_buf16[block] = 0xfffc;
      block = bak;
    }
    fat_buf16[block] = 0xfffc;
    
    if (vmu_block_write (dev, rbi.fatblock, (uint8 *)fat_buf16) != MAPLE_EOK) return -1;
  }
  
  return 0;
}


int
ndc_vmu_save(uint8 *src, uint32 src_len, maple_device_t *dev, 
             const char *filename, 
             const char *desc_short, 
             const char *desc_long, 
             const uint8 *icon)
{
  int save_len;
  int save_blocks;
  uint8 buf[512 * 200]; /* 200 blocks */
  dirent_vmu ent;
  int firstblk;
  
  if (!dev) return -1;
  
  save_len = 1024 + src_len;
  if (save_len % 512)
    save_blocks = save_len / 512 + 1;
  else
    save_blocks = save_len / 512;
  
  memset(buf, 0, sizeof(buf));
  
  ndc_vmu_create_vmu_header (buf, desc_short, desc_long, 
                             save_blocks * 512 - 128, icon);
  memcpy(buf + 640, &src_len, sizeof(uint32));
  memcpy(buf + 1024, src, src_len);
  ndc_vmu_do_crc(buf, 512 * save_blocks);
  
  firstblk = ndc_vmu_get_dirent (&ent, dev, filename);
  
  if ((firstblk >= 0) && (ent.filesize != save_blocks))
  {
    int free_blocks = ndc_vmu_check_free_blocks (NULL, dev);
    if (free_blocks < 0)
      return -1;
    
    if (ent.filesize + free_blocks < save_blocks)
      return -1;
    
    if (ndc_vmu_remove_file(dev, filename) < 0)
      return -1;
    
    firstblk = -1;
  }
  
  if (firstblk < 0) 
  {
    int free_blocks = ndc_vmu_check_free_blocks (NULL, dev);
    
    if (free_blocks < 0 || free_blocks < save_blocks)
      return -1;
    
    firstblk = ndc_vmu_allocate_file (dev, filename, save_blocks);
    if (firstblk < 0)
      return -1;
  }
  
  if (ndc_vmu_write_blocks (dev, firstblk, buf, save_blocks) != save_blocks)
      return -1;
  
  return 0;
  
}


int
ndc_vmu_load(uint8 *dst, uint32 *dst_len, maple_device_t *dev, const char *filename)
{
  dirent_vmu ent;
  uint32 src_len; 
  uint8 buf[512 * 200]; /* 200 blocks */
  uint32 body_len;
  int firstblk;
  
  if (!dev) return -1;
  
  firstblk = ndc_vmu_get_dirent (&ent, dev, filename);
  if (firstblk < 0)
  {		return -1;
  }
  
  body_len = (ent.filesize - 2) * 512;
  
  if (*dst_len < body_len)
  {	 return -1;
  }
  
  if (ndc_vmu_read_blocks (dev, firstblk, buf, ent.filesize) != ent.filesize)
  {	 return -1;
  }
  
  memcpy (&src_len, buf + 640, sizeof(uint32));
  memcpy (dst, buf + 1024, body_len);
  
  if (src_len)
    *dst_len = src_len;
  
  return 0;
}


