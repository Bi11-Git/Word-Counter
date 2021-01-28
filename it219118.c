
#include "Counter.h"

int main(int argc, char *argv[] ) {

    DIR *directory;
    struct stat st;
    char *curr_dir;

    //check if give one argument
    if( argc == 2 ) {

        curr_dir = argv[1];


        stat(curr_dir,&st);

        //check if user's argument is not a directory
        if( ! S_ISDIR(st.st_mode)) {

            printf("%s : Its not a directory!\n" ,curr_dir);
            exit(2);

        }


    }

    //check if user has not given argument
    if( argc == 1 ) {

        long length;

        //get the length of current directory path
        length = pathconf(".", _PC_PATH_MAX);
        assert(length != -1);

        // allocate memory for the string
        curr_dir = malloc(length * sizeof(*curr_dir));
        assert(curr_dir);

        // get the full name of the current path
        if (getcwd(curr_dir, length) == NULL) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

    }

    // open the directory
    directory = opendir(curr_dir);

    struct dirent *file;

    // check if the directory open properly
    if(directory == NULL) {

        printf("Directory %s cant open!\n" , argv[1]);
    }



    //while loop for all files in that directory
    while ((file = readdir(directory)) != NULL)
    {

        //check if filename is current directory or parent directory
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            continue;
        }

        //calculate the length of the string and store the absolute pathname
        int length_of_filepath = snprintf(NULL, 0, "%s/%s", curr_dir, file->d_name);
        char filepath[length_of_filepath + 1];
        snprintf(filepath, length_of_filepath + 1, "%s/%s", curr_dir, file->d_name);

        //create the struct stat because we need information about the file
        stat(filepath, &st);

        //check if is a directory 
        if (S_ISDIR(st.st_mode))
        {
            printf("%s :is a directory!\n", file->d_name);
            continue;
        }

        //check if the file is empty
        if (st.st_size == 0)
        {
            printf("%s :File is empty!\n", file->d_name);
            continue;
        }

        //open the file stream
        int file_descriptor = open(filepath, O_RDONLY);

        //check if file has open
        if (file_descriptor == -1)
        {
            printf("Cannot open file!\n");
            // exit(1);
        }

        //calculate the size of array
        int text_length = st.st_size / (int)(sizeof(char));

        //create a character array and store all the characters of the file
        char text[text_length];
        int read_descriptor = read(file_descriptor, text, text_length);

        //check if the read() function has read the characters
        if (read_descriptor == -1)
        {
            printf("%s :Error reading file!", file->d_name);
            continue;
        }

        //initialize to check the first 10 characters of the file
        int flag = 0;
        int max_check = 10;

        //if the file has less than 10 characters
        if (text_length < max_check)
        {
            max_check = text_length;
        }

        // if one of the characters is not ASCII raise the flag and print the message on the screen
        for (int i = 0; i < max_check; i++)
        {
            if (text[i] > 127 || text[i] < 0)
            {
                printf("%s :File is not ascii\n", file->d_name);
                flag = 1;
                break;
            }
        }

        //if the flag is raised then continue to the next file
        if (flag == 1)
        {
            continue;
        }

        //create a new child process
        int pid = fork();

        //check if the new process has created
        if (pid == -1)
        {
            perror("fork!");
            exit(1);
        }

        // if we are on the child process with pid 0 then call the function count_words()
        if (pid == 0)
        {

            //check if count_words() has found not ASCII characters
            if (count_words(text, text_length, file->d_name) == 1)
            {
                printf("%s :is not ascii!\n", file->d_name);
                exit(1);
            }

            exit(0);
        }
        else
        { // else if we are on the parent process then ignore the signals SIGINT and SIGTERM

            signal(SIGINT, signal_handler);
            signal(SIGTERM, signal_handler);
        }
    }

    //while loop to wait for all childrens
    while ((wait(NULL) ) > 0 );

    printf("All processes have finished!\nCheck file out.txt!\n" );


    closedir(directory);


    return 0;


}
