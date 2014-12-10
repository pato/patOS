#ifndef _LIBWIN_H_
#define _LIBWIN_H_

extern long win_req(char* name);
extern long win_color(int color);
extern long win_draw(char*** buf);
extern long win_cursor(int r, int c);
extern long win_clear();
extern long win_size(int* w, int* h);

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHT_GRAY 7
#define DARK_GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA  13
#define LIGHT_BROWN 14
#define WHITE 15
#endif
