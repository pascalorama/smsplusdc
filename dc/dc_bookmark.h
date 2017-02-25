
#ifndef _DC_BOOKMARK_H_
#define _DC_BOOKMARK_H_

extern t_file_entries fav_files[MAX_FAV_ENTRIES]; 

extern void load_bookmark();
extern void bookmark_init();
extern void draw_bookmark(void);
extern  void bookmark_run(void);
extern  void render_bookmark_text(void);
extern  void allocate_bookmark(void);

extern  int bookmark_find_slot();

#endif
