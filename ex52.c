#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>

// Initial Settings
#define INIT_X 10
#define INIT_Y 1
#define BOARD_HEIGHT 20
#define BOARD_WIDTH 20

// Keys
#define QUIT_KEY 'q'
#define CHANGE_ORIENTATION_KEY 'w'
#define MOVE_LEFT_KEY 'a'
#define MOVE_RIGHT_KEY 'd'
#define MOVE_DOWN_KEY 's'

enum boolean {
  FALSE,
  TRUE
};
enum Orientation {
  HORIZONTAL,
  VERTICAL,
  NUMBER_OF_ORIENTATIONS
};

typedef struct LineShape {
  int center_x;
  int center_y;
  enum Orientation orientation;
} LineShape;

int reset_shape(LineShape *ls, int force_reset) {
  if (force_reset || (ls->orientation == HORIZONTAL
      && ls->center_y >= BOARD_HEIGHT - 1
      || ls->orientation == VERTICAL
          && ls->center_y >= BOARD_HEIGHT - 2)) {
    ls->center_x = INIT_X;
    ls->center_y = INIT_Y;
    ls->orientation = VERTICAL;
  }
}

// Global Variables
LineShape lineShape;

void print_board() {
  int i;
  int j;
  int cx = lineShape.center_x;
  int cy = lineShape.center_y;

  system("clear");
  write(STDOUT_FILENO, "\n", 1);
  for (i = 0; i < BOARD_HEIGHT; i++) {
    for (j = 0; j < BOARD_WIDTH; j++) {
      if (j == 0 || j == BOARD_WIDTH - 1 || i == BOARD_HEIGHT - 1) {
        write(STDOUT_FILENO, "*", 1);
      } else if (
          (lineShape.orientation == HORIZONTAL && i == cy && 2 > abs((j - cx)))
              || (lineShape.orientation == VERTICAL && j == cx
                  && 2 > abs(i - cy))) {
        write(STDOUT_FILENO, "-", 1);
      } else {
        write(STDOUT_FILENO, " ", 1);
      }
    }
    write(STDOUT_FILENO, "\n", 1);
  }
}

void handle_key(char key) {
  if (key == MOVE_LEFT_KEY) {
    if (lineShape.orientation == HORIZONTAL && lineShape.center_x > 2
        || lineShape.orientation == VERTICAL && lineShape.center_x > 1)
      lineShape.center_x--;
  } else if (key == MOVE_DOWN_KEY) {
    lineShape.center_y++;
  } else if (key == MOVE_RIGHT_KEY) {
    if (lineShape.orientation == HORIZONTAL
        && lineShape.center_x < BOARD_WIDTH - 3
        || lineShape.orientation == VERTICAL
            && lineShape.center_x < BOARD_WIDTH - 2)
      lineShape.center_x++;
  } else if (key == CHANGE_ORIENTATION_KEY) {
    lineShape.orientation =
        (lineShape.orientation + 1) % NUMBER_OF_ORIENTATIONS;
    if (lineShape.orientation == HORIZONTAL && lineShape.center_x >=
        BOARD_WIDTH - 3) {
      lineShape.center_x--;
    } else if (lineShape.orientation == HORIZONTAL && lineShape.center_x <= 2) {
      lineShape.center_x++;
    }
  }
  reset_shape(&lineShape, FALSE);
}

void handle_key_signal() {
  char key = 0;
  scanf("%c", &key);
  if (key == QUIT_KEY) {
    exit(0);
  }
  handle_key(key);
  print_board();
}

void handle_alarm_signal() {
  alarm(1);
  lineShape.center_y++;
  reset_shape(&lineShape, FALSE);
  print_board();
}

int main() {
  reset_shape(&lineShape, TRUE);
  signal(SIGALRM, handle_alarm_signal);
  alarm(1);
  signal(SIGUSR2, handle_key_signal);
  while (1) {
    pause();
  }
}

