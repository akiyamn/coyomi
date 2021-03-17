#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <time.h>
#include <unistd.h> 
#include <limits.h>
#include <sys/wait.h>
#include <ncurses.h>

#define DAYS_IN_WEEK 7
#define DAY_NAME_SIZE 50
#define COMMAND_SIZE 50
#define MAIN_TEXT_SIZE 5000
#define ASCII_DIGIT_START 48
#define SECONDS_IN_DAY 86400
#define LAYOUT_X_RATIO 0.25
#define MAIN_PADDING 2

// Will hard code these for now...
#define TERMINAL "st"
#define EDITOR "vim"
#define ENTRY_PATH "/home/yui/Documents/coyomi"

typedef struct win_dims_t {
    int ysize;
    int xsize;
    int ypos;
    int xpos;
} win_dims_t;


#endif
