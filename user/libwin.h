#ifndef _LIBWIN_H_
#define _LIBWIN_H_

extern long win_req(const char* name);
extern long win_color16(int color);
extern long win_color256(int r, int g, int b);
extern long win_draw(char*** buf);
extern long win_cursor(int r, int c);
extern long win_clear();
extern long win_size(int* w, int* h);

#endif
