/*** INCLUDES ***/
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/*** DEFINES ***/
#define CTRL_KEY(k) ((k) & 0x1f)

/*** DATA ***/

// Make a copy of original termios, so wen can return the terminal state on exit
struct termios orig_termios;

/*** TERMINAL ***/

void die(const char *s) {
  // prints an error message and exits the program
  perror(s);
  exit(1); // exit status 1 indicates failure 
}

void disableRawMode() {
// tcsetattr(), tcgetattr(), and read() all return -1 on failure, and set the errno value to indicate the error.
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcgetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

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

  //  “cc = control characters”
  raw.c_cc[VMIN] = 0; // sets the minimum number of bytes of input needed before read() can return
  raw.c_cc[VTIME] = 1; // sets the maximum amount of time to wait before read() returns

  if (tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

// wait for one keypress, and return it.
char editorReadKey() {
  int nread = 0;
  char c;
  while ((nread == read(STDIN_FILENO, &c, 1 )) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

/*** OUTPUT ***/
void editorRefreshScreen() {
  // We are writing 4 bytes out to the terminal. 
  // The first byte is \x1b, which is the escape character, or 27 in decimal. The other three bytes are [2J
  write(STDOUT_FILENO, "\x1b[2J", 4);

  // H command (Cursor Position)
  write(STDOUT_FILENO, "\x1b[H",3);
}

/*** INPUT ***/

// waits for a keypress, and then handles it.
void editorProcessKeypress() {
  char c = editorReadKey();
  switch (c)
  {
  case CTRL_KEY('q'):
    exit(0);
    break;
  }
}

/*** INIT ***/

int main() {
  enableRawMode();
  
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  return 0;
}
