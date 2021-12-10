nes (33 sloc)  787 Bytes
  
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZE 8

void print_text(int fd)
{
        int n = 1; //start quantitive if bytes
        char buf[BUFFER_SIZE];
        while (n != 0)
        {
                n = read(fd, buf, BUFFER_SIZE);
                if (n < 0)
                {
                        perror("ERROR");
                        break;
                }
                else
                {
                        write(1, buf, n);
                }
        }
}

int main(int argc, char *argv[])
{
        for (int i = 1; i < argc; i++)
        {
                int fd = open(argv[i], O_RDONLY);
                if (fd < 0)
                        perror("ERROR");
                print_text(fd);
                close(fd);
        }
        return 0;
}
