#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<time.h>
#include<errno.h>
#include<ctype.h>
#include<getopt.h>

int main (int argc, char* argv[]) {
	struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);

	static struct option long_options[] = {
		{"quiet", no_argument, NULL, 0},
	};
	int geopt = getopt_long(argc, argv, "q", long_options, NULL) > -1 ? 1 : 0;
	int pid, wstatus, ch;
	int mypipefd[2], pipestat;
	pipestat = pipe(mypipefd);
	if(pipestat != 0) {
		return errno;
	}
	pid = fork();
	if(pid == 0) {
		int statusch;
		statusch = dup2(mypipefd[1], 1);
		if(statusch < 0) {
			return errno;
		}
		statusch = close(mypipefd[0]);
		if(statusch != 0) {
			return errno;
		}
		execvp(argv[geopt ? 2 : 1], argv + (geopt ? 2 : 1));
	}
	else {
		int statuspar;
		statuspar = close(mypipefd[1]);
		if(statuspar < 0) {
			return errno;
		}
		int befsp = 0, cursp = 0, wordcount = 0, strcount = 0, bytecount = 0;
	while (read(mypipefd[0], &ch, 1) == 1) {
		cursp = isspace(ch);
		if((befsp == 0) && (cursp != 0)) {
			wordcount ++;
		}
		befsp = cursp;
		if(ch == '\n') {
			strcount++;
		}
		bytecount ++;
		if(!geopt)
			write(STDOUT_FILENO, &ch, 1);
	}
	waitpid(-1, &wstatus, 0);
	struct timespec finish;
        clock_gettime(CLOCK_MONOTONIC, &finish);
	long int timesec = (finish.tv_sec - start.tv_sec) * 1000;
	long int timensec = finish.tv_nsec / 1000000 - start.tv_nsec / 1000000;
	long int msec = timesec + timensec;
	statuspar = close(mypipefd[0]);
                if(statuspar < 0) {
                        return errno;
                }

	fprintf(stderr, "\nms [%ld]\n", msec);
	fprintf(stderr, "strings = %d   words = %d   bytes = %d\n", strcount, wordcount, bytecount);
	}
} 
