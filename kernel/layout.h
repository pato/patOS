#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#define MAXROWS 25
#define MAXCOLS 80

struct Layout {
  int r, c, w, h;
  Layout(int splits, int which) {
    w = windowWidth(splits);
    h = windowHeight(splits);
    r = windowRow(splits, which);
    c = windowCol(splits, which);
  }

  int windowWidth(int splits) {
    return MAXCOLS - 2 - (splits - 1); //TODO: starthere: fix this mess
  }

  int windowHeight(int splits) {
    return MAXROWS - 3 - (splits - 1);
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
