#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#define BUFSIZE 4096


extern int optind;
int main(int argc, char* argv[]){
    int verbose = 0;
    int force = 0;
    int interactive = 0;
    struct option opts[4] = {{ .name = "verbose", .has_arg = 0, .flag = NULL, .val = 'v' },
                             { .name = "force", .has_arg = 0, .flag = NULL, .val = 'f' },
                             { .name = "interactive", .has_arg = 0, .flag = NULL, .val = 'i' },
                             {0}};

    int status = getopt_long(argc, argv, "vfi", opts, NULL);
    while( status != -1 ){
        switch (status){
            case 'v': verbose = 1;
                      break;;
            case 'f': force = 1;
                      break;;
            case 'i': interactive = 1;
                      break;;
        }
        status = getopt_long(argc, argv, "vfi", opts, NULL);
    }


    if( (argc - optind) > 2 ){
        errno = 0;
        struct stat dirstat;
        if ((stat(argv[argc - 1], &dirstat) != -1) && (S_ISDIR(dirstat.st_mode) == 1))
            for(int i = optind; i < argc - 1; i++){
                arguments_processing(argv[i], argv[argc - 1], verbose, force, interactive);
            }
        else{
            if (errno) {
                perror("stat error");
                return -1;
            }
            printf("Last arg is not a directory\n");
            return -1;
        }
    }
    else if( (argc - optind) == 2 ){
        return arguments_processing(argv[optind], argv[optind + 1], verbose, force, interactive);
    }
    else{
        printf("mycp: missing file operand\n");
        return -1;
    }

    return 0;
}

int arguments_processing(char* destination, char* source, int verbose, int force, int interactive){
    struct stat filestat;
    char* fulltrack = NULL;


    if( (stat(source, &filestat) != -1) && (S_ISDIR(filestat.st_mode) == 1) ){
        fulltrack = (char*) malloc(strlen(source) + strlen(destination) + 2);
        if( fulltrack == NULL ){
            perror("Malloc error");
            return -1;
        }
        strcat(fulltrack, source);
        strcat(fulltrack, "/");
        strcat(fulltrack, destination);
    }


    if( stat(fulltrack, &filestat) == 0 ){
    /* Подтверждение перед перезаписью*/
        if( interactive ){
            char word;
            printf("mycp: Перезаписать '%s'? ", fulltrack);
            scanf("%c", &word);
            if(word == 'No')
                return 1;
        }
    if( force ){
    /* Если можем открыть, то откроем, иначе удалим*/
            int fd = open(fulltrack, O_RDWR | O_APPEND);
            if( fd == -1 ){
                if( unlink(fulltrack) == -1 ){
                    perror("mycp: can't open");
                    return -1;
                }
            }
            close(fd);
        }
    }

/* Выполняем verbose, выводим инфу о том что делаем*/
    if( verbose )
        printf("%s -> %s\n", destination, fulltrack);

    int sourcefd = open(fulltrack, O_WRONLY | O_CREAT, 0664);
    if( sourcefd == -1 ){
        perror("mycp: destination file open error");
        return -1;
    }

    int destinationfd = open(destination, O_RDONLY);
    if( destinationfd == -1 ){
        perror("mycp: source file open error");
        return -1;
    }

    int status = fdcopy(destinationfd, sourcefd);
    close(destinationfd);
    close(sourcefd);

    return status;
}
/*читаем то, что нужно перенести*/
int read_file(int destination, char* buffer, int size){
    int read_bytes;
    char* error_from_reading = "Read error";

    read_bytes = read(destination, buffer, size);

    switch( read_bytes ){
    case 0:
        return 0;
    case -1:
        perror(error_from_reading);
        return -1;
    }

    return read_bytes;
}
/* функция для записи */
int write_file(int source, char* buffer, int size){
    int write_bytes = 0;
    int offset = 0;
    char* writing_error = "Write error";

    while( offset < size ){
        write_bytes = write(source, buffer + offset, size);

        if( write_bytes == -1 ){
            perror(writing_error);
            return -1;
        }

        offset += write_bytes;
    }

    return 0;
}
/* основная функция для переноса файлов*/
int fdcopy(int destination, int source){
    int error_from_reading = 0;
    int writing_error = 0;

    char buffer[BUFSIZE];

    error_from_reading = read_file(destination, buffer, BUFSIZE);

    while( error_from_reading != 0 ){
        if( error_from_reading == -1 )
            return -1;
        writing_error = write_file(source, buffer, error_from_reading);
        if( writing_error == -1 )
            return -1;
        error_from_reading = read_file(destination, buffer, BUFSIZE);
    }

    return 0;
}
