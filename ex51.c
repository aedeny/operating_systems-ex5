#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>

#define QUIT_KEY 'q'
#define CHANGE_ORIENTATION_KEY 'w'
#define MOVE_LEFT_KEY 'a'
#define MOVE_RIGHT_KEY 'd'
#define MOVE_DOWN_KEY 's'

char get_char() {
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
  return (buf);
}

int execute_game(int pipe[2]) {
  pid_t pid = fork();
  if (pid > 0) {
    // Father
    close(pipe[0]);
    return pid;
  } else {
    // Child
    close(pipe[1]);
    dup2(pipe[0], STDIN_FILENO);
    char *argv[] = {"./draw.out", NULL};
    execvp(argv[0], argv);
  }
}

int main() {
  int my_pipe[2];
  pipe(my_pipe);

  int child_pid = execute_game(my_pipe);
  char pressed_key;
  while (1) {
    pressed_key = get_char();
    write(my_pipe[1], &pressed_key, sizeof(char));
    kill(child_pid, SIGUSR2);
    if (pressed_key == QUIT_KEY) {
      break;
    }
  }
  close(my_pipe[1]);
}