
#include <pthread.h>
#include "Counter.h"

int count_words( char text[] , int text_length, char filename[]) {

    int i, tmp;

    // divide the text length with the number of threads to share the text
    int text_length_per_thread = text_length/NUM_THREADS;

    //create an array of structs to store the data of threads
    struct text_part parts[NUM_THREADS];

    //for loop to initialize the array of structs
    for(i = 0; i < NUM_THREADS; i++) {

        parts[i].array = text;
        parts[i].start = i * text_length_per_thread;
        parts[i].end = parts[i].start + text_length_per_thread ;

        if(i == NUM_THREADS - 1 ) {
            parts[i].end = text_length - 1;
        }

    }


    pthread_t thread[NUM_THREADS];

    // creating threads
    for ( i=0; i<NUM_THREADS; i++ )
    {
        thread[i] = i ;
        tmp = pthread_create(&thread[i], NULL, (void *(*)(void *)) thread_function, (void *)&parts[i] );

        if(tmp != 0) {

        }
    }

    int sum_of_words = 0;

    //for loop to wait for every thread to finish and add the set of words founded
    for (int j = 0; j < NUM_THREADS; j++) {
        pthread_join(thread[j] , NULL);
        sum_of_words += parts[j].words_founded;

        if( parts[j].is_not_ascii == 1 ) {
            return 1;
        }
    }


    int fd;

    //open the out.txt file
    if ((fd=open("out.txt",O_WRONLY | O_CREAT | O_APPEND,0644 )) == -1) {

        perror("");
        exit(1);

    }

    //calculate the length of the string for the message
    int length_of_output = snprintf(NULL, 0,"<%d>,<%s>,<%d>\n", getpid(), filename, sum_of_words);

    //create the array and store the message
    char output[length_of_output + 1];
    snprintf(output, length_of_output + 1, "<%d>,<%s>,<%d>\n", getpid(), filename, sum_of_words);

    //write the message on the file
    write(fd, output, strlen(output));

    return 0;

}

void thread_function( struct text_part * array_part )
{

    array_part->is_not_ascii = 0;
    array_part->words_founded = 0;

    //for loop to cross the part of text
    for (int i = array_part->start; i < array_part->end; i++) {

        //check if a character is separator or is not ascii
        if (array_part->array[i] == '\0' || array_part->array[i] == '\t' || array_part->array[i] == '\n' || array_part->array[i] == ' ') {
            array_part->words_founded++;
        } else if( array_part->array[i] > 128 || array_part->array[i] < 0 ) {
            array_part->is_not_ascii = 1;
            pthread_exit(NULL);
        }

    }

    pthread_exit( NULL );


}

void signal_handler (int sig_num) {

    return;

}
