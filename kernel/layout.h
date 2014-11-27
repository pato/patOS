#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#define MAXROWS 25
#define MAXCOLS 80

/**
 * Assumptions: 
 * hsplits - the number of windows accross the horizontal axis (starts at 1)
 * vsplits - the number of windows accross the vertical axis (starts at 1)
 * which   - the index of the window (also starts at 1)
 *
 */

struct Layout {
  int r, c, w, h;
  Layout(int vsplits, int hsplits, int vwhich, int hwhich) {
    h = windowHeight(vsplits);
    w = windowWidth(hsplits);
    r = windowRow(vsplits, vwhich);
    c = windowCol(hsplits, hwhich);
  }

  int windowWidth(int splits) {
    /* (maxcols - left and right margins - individual margins) / number of splits*/
    return (MAXCOLS - 2 - (splits - 1)) / splits;
  }

  int windowHeight(int splits) {
    /* (maxrows - top bar - bottom and top margins - individual margins) / number of splits*/
    return (MAXROWS - 3 - (splits - 1)) / splits;
  }

  int windowRow(int splits, int which) {
    /* top bar + first border + window heights + borders */
    return 2 + (which - 1) * windowHeight(splits) + (which - 1);
  }

  int windowCol(int splits, int which) {
    /* left border + window widths + borders */
    return 1 + (which - 1) * windowWidth(splits) + (which - 1);
  }
};

#endif
