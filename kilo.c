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

  // IXON turns off Ctrl-S and Ctrl-Q signals. ICRNL fixes Ctrl-M reading as return char
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  raw.c_oflag &= ~(OPOST);

   raw.c_cflag |= (CS8);

  // ICANON turns off canonical mode. quits after pressing q
  // ISIG turns off Ctrl-C and Ctrl-Z signals, IEXTEN Ctrl-V
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();
  
  while (1) {
    char c = '\0';
    read(STDOUT_FILENO, &c, 1);
    // iscntrl() tests whether a character is a control character, (enter, esc, ...).
    if (iscntrl(c)) {
      printf("%d\r\n",c);
    } else {
      // %d tells printf to format the byte as a decimal number (its ASCII code), and %c tells it to write out the byte directly, as a character.
      printf("%d ('%c')\r\n",c,c);
    }
    if (c == 'q') break;
  }
  return 0;
}
