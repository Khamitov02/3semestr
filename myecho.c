
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main (int argc, char* argv[]) {
	int nswitch = 0, i = 1;
	while(i < argc - 1) {
		if(i == 1 && (strcmp(argv[i], "-n") == 0 )) {
			nswitch = 1;
		}
		else {
			printf("%s ", argv[i]);
		}
	i++;
	}
	if(nswitch == 1) {
	       	printf("%s", argv[argc - 1]);
	}
	else {
		printf("%s\n", argv[argc - 1]);
	}
}
