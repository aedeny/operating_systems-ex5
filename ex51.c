#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define QUIT_KEY 'q'
#define CHANGE_ORIENTATION_KEY 'w'
#define MOVE_LEFT_KEY 'a'
#define MOVE_RIGHT_KEY 'd'
#define MOVE_DOWN_KEY 's'
#define ERROR_MSG "Error"

enum boolean {
  FAIL = -1,
  SUCCESS
};

void writeError(char *message) {
  write(STDERR_FILENO, message, sizeof(message));
}

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
  pid_t pid;
  if ((pid = fork()) == -1) {
    writeError(ERROR_MSG);
    return FAIL;
  }

  if (pid > 0) {
    // Father
    close(pipe[0]);
    return pid;
  } else {
    // Child
    if (close(pipe[1]) == -1) {
      writeError(ERROR_MSG);
      return FAIL;
    }
    if(dup2(pipe[0], STDIN_FILENO) == -1){
      writeError(ERROR_MSG);
      return FAIL;
    }
    char *argv[] = {"./draw.out", NULL};
    execvp(argv[0], argv);
    return FAIL;
  }
}

int main() {
  int my_pipe[2];
  if (pipe(my_pipe) == -1) {
    write(STDERR_FILENO, ERROR_MSG, sizeof(ERROR_MSG));
    exit(1);
  }

  int child_pid;
  if ((child_pid= execute_game(my_pipe)==FAIL){
    close(my_pipe[0]);
    close(my_pipe[1]);
    writeError(ERROR_MSG);
    exit(1);
  }
  // TODO add checks
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
  kill(child_pid, SIGKILL);
}