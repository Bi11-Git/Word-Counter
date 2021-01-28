
#ifndef WORD_COUNTER_COUNTER_H
#define WORD_COUNTER_COUNTER_H

#include <stdio.h>
#include <dirent.h>
#include <wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define NUM_THREADS 4

//that struct contains one part of text for every thread
struct text_part {
    char *array;
    int is_not_ascii;
    int start;
    int end;
    int words_founded;
};

int count_words( char array[], int length, char filename[]);

void thread_function( struct text_part * array);

void signal_handler (int);

#endif //WORD_COUNTER_COUNTER_H
