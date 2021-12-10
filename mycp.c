#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char *argv[]) {
	int infd;         // дескриптор копируемого файла
	int outfd;        // дескриптор файла, куда копируем
	int creatfd;      // дескриптор создаваемого файла
	char buffer[512]; // буффер для копирования данных
	char *infile = argv[argc-2];
	char answ[3];
	char *outfile = argv[argc-1];
	int flagf = 0, flagi = 0, flagv = 0;;
	
	/* Проверка на аргументы */
	if( (argv[argc-2] == NULL) || (argv[argc-1] == NULL) ) {
		printf("\nYou must supply command line with at least two arguments.\n\n");
		exit(1);
	}
	for(int i = 0; i < argc; i++) {
		if(strcmp(argv[i], "-f") == 0) {
			flagf = 1;
		}
		if(strcmp(argv[i], "-i") == 0) {
                        flagi = 1;
                }
		if(strcmp(argv[i], "-v") == 0) {
                        flagv = 1;
                }
	}
	/* открываем копируемый файл -- выводим сообщение об ошибке в случае неудачи */
	if ( (infd = open(infile, 0)) < 0 ) { // 0 - это флаг O_RDONLY
		printf ("Can't open %s\n", infile); 
		exit(1);
	}
	/* Создаем новый файл */  
	if ( (creatfd = creat(outfile, 0777)) < 0 ) { // информация по mode есть тут: http://manpages.ubuntu.com/manpages/precise/ru/man2/open.2.html
		if(flagi == 1) {
			printf("cp: overwrite 'exampl2.txt'? \n");
			scanf("%s", answ);
			if(strcmp(answ, "Yes") != 0) {
				exit(1);
			}
		}
		if(flagf == 1) {
			if(unlink(argv[argc-1]) == 0) {
				creatfd = creat(outfile, 0777);
			}
		}
		else {
			printf ("Can't create %s\n", outfile);
			exit(1);
		}
	} 
	
	if(flagv == 1) {
		printf("'%s' -> '%s'\n", argv[argc-2], argv[argc-1]);
	}
		
	/* открываем созданный для записи файл и записываем в него данные */ 
	if ( ( outfd = open(outfile, 1)) < 0 ) { // 1 - это флаг O_WRONLY (https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/fcntl.h)
		printf ("Can't open %s\n", outfile);
		exit(1); 
	}
	else {
		while (read(infd, buffer, sizeof(buffer)))  {
			write(outfd, buffer, strlen(buffer));
		} 
		close(infd);
		close(outfd);
	}
}
