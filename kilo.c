#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

// Make a copy of original termios, so wen can return the terminal state on exit
struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);

  // Runs when the program exits
  atexit(disableRawMode);

  struct termios raw = orig_termios;

  // ICANON turns off canonical mode. quits after pressing q
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();
  char c;
  while (read(STDOUT_FILENO, &c, 1) == 1 && c != 'q') {
    if (iscntrl(c)) {
      printf("%d\n",c);
    } else {
      // %d tells printf to format the byte as a decimal number (its ASCII code), and %c tells it to write out the byte directly, as a character.
      printf("%d ('%c')\n",c,c);
    }
  }
  return 0;
}
