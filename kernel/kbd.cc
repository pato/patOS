#include "kbd.h"
#include "machine.h"
#include "debug.h"
#include "windowmanager.h"

int shift = 0;

InputStream<char> *Keyboard::is;
BB<char> *Keyboard::bb;

void Keyboard::init() {
    bb = new BB<char>(100);
    is = bb;
}

static uint32_t kbd_get(void);

/* interrupt handler */
void Keyboard::handler(void) {
    uint32_t ch = kbd_get();
    if (ch > 0)
      bb->put((char)ch);
}

/* internal functions */
static char ul(char x) {
    if (shift) {
      switch (x) {
        case 'a' ... 'z' : return x - ('a' - 'A');
        case '\'' : return '"';
        case '\\' : return '|';
        case '`'  : return '~';
        case '-'  : return '_';
        case '='  : return '+';
        case '.'  : return '>';
        case ','  : return '<';
        case '/'  : return '?';
        case ';'  : return ':';
        case ']'  : return '}';
        case '['  : return '{';
        case '0'  : return ')';
        case '1'  : return '!';
        case '2'  : return '@';
        case '3'  : return '#';
        case '4'  : return '$';
        case '5'  : return '%';
        case '6'  : return '^';
        case '7'  : return '&';
        case '8'  : return '*';
        case '9'  : return '(';
        default: return 'X';
      }
    } else {
        return x;
    }
}

/* get the next character from the hardware */
static uint32_t kbd_get(void) {
    while ((inb(0x64) & 1) == 0);
    uint8_t b = inb(0x60);
    switch (b) {
    case 0x02 ... 0x0a : return(ul('0' + b - 1)); // 1-9
    case 0x0b : return('0');

    case 0x0e : return(8); // backspace
    case 0x1c : return(13); // new line
    case 0x39 : return(32); // space

    case 0x10 : return(ul('q'));
    case 0x11 : return(ul('w'));
    case 0x12 : return(ul('e'));
    case 0x13 : return(ul('r'));
    case 0x14 : return(ul('t'));
    case 0x15 : return(ul('y'));
    case 0x16 : return(ul('u'));
    case 0x17 : return(ul('i'));
    case 0x18 : return(ul('o'));
    case 0x19 : return(ul('p'));
    case 0x1e : return(ul('a'));
    case 0x1f : return(ul('s'));
    case 0x20 : return(ul('d'));
    case 0x21 : return(ul('f'));
    case 0x22 : return(ul('g'));
    case 0x23 : return(ul('h'));
    case 0x24 : return(ul('j'));
    case 0x25 : return(ul('k'));
    case 0x26 : return(ul('l'));
    case 0x2c : return(ul('z'));
    case 0x2d : return(ul('x'));
    case 0x2e : return(ul('c'));
    case 0x2f : return(ul('v'));
    case 0x30 : return(ul('b'));
    case 0x31 : return(ul('n'));
    case 0x32 : return(ul('m'));

    case 0x28 : return(ul('\''));
    case 0x29 : return(ul('`'));
    case 0x0C : return(ul('-'));
    case 0x0D : return(ul('='));
    case 0x33 : return(ul(','));
    case 0x34 : return(ul('.'));
    case 0x35 : return(ul('/'));
    case 0x27 : return(ul(';'));
    case 0x2B : return(ul('\\'));
    case 0x1A : return(ul('['));
    case 0x1B : return(ul(']'));

    case 0x2a: case 0x36: shift = 1; return 0;
    case 0xaa: case 0xb6: shift = 0; return 0;

    /* Used for shifting focus */
    case 0x3B: WindowManager::wm->shiftFocus(1); return 0; // F1
    case 0x3C: WindowManager::wm->shiftFocus(2); return 0; // F2
    case 0x3D: WindowManager::wm->shiftFocus(3); return 0; // F3
    case 0x3E: WindowManager::wm->shiftFocus(4); return 0; // F4
    case 0x3F: WindowManager::wm->shiftFocus(5); return 0; // F5
    case 0x40: WindowManager::wm->shiftFocus(6); return 0; // F6

    case 0x37: /* printscreen */
      WindowManager::wm->currentWindow()->redrawTextBuf();
      return 0;
    default: return 0;
    }
 
}

