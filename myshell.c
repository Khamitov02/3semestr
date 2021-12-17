#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#define INWORD 1
#define OUTWORD 0
#define max_symb 1000
#define CLOSE(fd1)              \
    if (close (fd1) < 0) {      \
        perror ("Error close"); \
        return errno;           \
    }

int main ()
{
    char command_name[max_symb];
    char *commands[max_symb];
    char *cmd[max_symb];

    while (!feof (stdin)) {
        printf ("my_shell > ");

        fgets (command_name, max_symb, stdin); //читаем символы

        command_name[strlen (command_name) - 1] = 0;

        if ((command_name == NULL) || !strcmp (command_name, "exit")) // выход
            break;

        int number_of_comnd = 0;
        commands[number_of_comnd] = strtok (command_name, "|");
         while (commands[++number_of_comnd] = strtok (NULL, "|"));
        int pass_pipe = -1;
        int pid;
        int pipefd[2];
        for (int i = 0; i < number_of_comnd; ++i) {
            if (pipe (pipefd) == -1) {
                printf ("Pipe error\n"); //выводим ошибку пайпа
                break;
            }

            pid = fork ();
            if (pid == -1) {
                printf ("Fork error\n"); //выводим ошибку форка
                break;
            }

            if (pid) {
                close (pipefd[1]);
                if (pass_pipe != -1)
                    CLOSE (pass_pipe);
                pass_pipe = pipefd[0];
            }
            else {
                if (pass_pipe != -1)
                    if (dup2 (pass_pipe, 0) == -1) {
                        perror ("error in copying from std");
                        return -1;
                    }

                if (i < (number_of_comnd - 1))
                    if (dup2 (pipefd[1], 1) == -1) {
                        perror (" error in copying to stdout");
                        return -1;
                    }

                close (pipefd[0]);
                close (pipefd[1]);
                if (pass_pipe != -1)
                    CLOSE (pass_pipe); //закрываем пайп

                int num_of_args = 0;
                cmd[num_of_args] = strtok (commands[i], " ");
                while (cmd[++num_of_args] = strtok (NULL, " "));

                execvp (cmd[0], cmd);
                perror (cmd[0]);
                return -1;
            }
        }

        close (pass_pipe); //закрываем пайп

        for (int i = 0; i < number_of_comnd; i++)
            wait (NULL);
    }

    return 0;
}
