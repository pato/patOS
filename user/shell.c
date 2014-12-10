#include "libc.h"
#include "libwin.h"

#define ARGLIMSTEP 2
#define ERR_FILE_NOT_FOUND (-1006)
#define ERR_EXEC_NOT_FOUND (-1007)

void notFound(char* cmd) {
  puts(cmd);
  puts(": command not found\n");
}

void fileNotFound(char* cmd, char* file) {
  puts(cmd);
  puts(": ");
  puts(file);
  puts(" : No such file or directory\n");
}

int isWhitespace(char c) { return c == ' '; }
int isNull(char c) { return c == '\0'; }

char** splitArgs(char* in) {
  unsigned int count = 0;
  //puts("splitArgs(");puts(in);puts(")\n");

  unsigned int arglim = ARGLIMSTEP;
  char** args = (char**)malloc(sizeof(char**) * arglim);

  unsigned int i = 0;
  while (1) {
    while (!isNull(in[i]) && isWhitespace(in[i])) i++;
    if (isNull(in[i])) break;
    unsigned int length = 0;
    while (!isNull(in[i]) && !isWhitespace(in[i])) {i++, length++;}

    if (count == (arglim - 1)) {
      arglim += ARGLIMSTEP;  
      args = (char**)realloc(args, sizeof(char**) * arglim);
    }

    args[count] = (char*)malloc(sizeof(char) * (length + 1));
    memcpy(args[count], &in[i - length], length + 1);
    args[count][length] = '\0';

    count++;
    if (isNull(in[i])) break;
  }

  args[count] = (char*)nullptr;

#if 0
  puts("args found: ");
  putdec(count);
  puts("\n");

  for (i = 0; i < count; i++) {
    puts("arg");putdec(i);puts(": ");
    puts(args[i]);
    puts("|");
    puts("\n");
  }
#endif
  return args;
}

void testSplit() {
  char* testStr0 = "";
  char* testStr1 = "echo test";
  char* testStr2 = "echo test          ";
  char* testStr3 = "        i can     handle    spaces";
  char* testStr4 = "                      ";
  char* testStr5 = "test";
  splitArgs(testStr0);
  splitArgs(testStr1);
  splitArgs(testStr2);
  splitArgs(testStr3);
  splitArgs(testStr4);
  splitArgs(testStr5);
}

int main() {
  win_req("super shell");
  while (1) {
    win_color(LIGHT_GREEN);
    puts("shell>");
    win_color(WHITE);
    puts(" ");
    char* in = gets();
    char** argv = splitArgs(in);
    if (argv[0] == nullptr) continue;
    int disown = 0;
    if (strcmp("exit", argv[0]) == 0) {
      win_color(RED);
      puts("Exiting...");
      exit(0);
    } else if (strcmp("shell", argv[0]) == 0) {
      disown = 1;
    }
    int pid = fork();
    if (pid) {
      if (disown) {
        close(pid);
        win_color(CYAN);
        puts("Child disowned\n");
        win_color(WHITE);
      } else {
        int eCode = join(pid);
        if (eCode == ERR_EXEC_NOT_FOUND) {
          win_color(RED);
          notFound(argv[0]);
          win_color(WHITE);
        } else if (eCode == ERR_FILE_NOT_FOUND) {
          win_color(RED);
          fileNotFound(argv[0], argv[1]);
          win_color(WHITE);
        }
      }
    } else {
      exit(execv(argv[0], argv));
    }
    if (in) free(in);
  }
  return 0;
}
