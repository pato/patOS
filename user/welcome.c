#include "libc.h"
#include "libwin.h"


int main(int argc, char** argv) {
  win_req((char*)"Welcome");
  puts("\n\n\n\n\n");
  win_color(RED);
  puts("             Welcome to");
  puts("\n");
  win_color(GREEN);
  puts(" mmmmm           m     mmmm   mmmm");
  puts("\n");
  puts(" #   \"#  mmm   mm#mm  m\"  \"m #\"   \"");
  puts("\n");
  puts(" #mmm#\" \"   #    #    #    # \"#mmm");
  puts("\n");
  puts(" #      m\"\"\"#    #    #    #     \"#");
  puts("\n");
  puts(" #      \"mm\"#    \"mm   #mm#  \"mmm#\"");
  puts("\n");
  puts("\n");
  win_color(RED);
  puts("             NextGen OS");
  puts("\n");
  puts("\n");
  win_color(WHITE);
  puts("   Press enter to return to shell");
  gets();
  return 0;
}
