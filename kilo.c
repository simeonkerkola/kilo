#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);

  // Runs when the program exits
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  tcgetattr(STDIN_FILENO, &raw);

  raw.c_lflag &= ~(ECHO);

  tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();
  char c;
  while (read(STDOUT_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}
