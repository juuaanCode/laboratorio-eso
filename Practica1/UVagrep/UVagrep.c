#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("UVagrep: searchterm [file ...]\n");
		exit(1);
	}

	size_t size = 20;
	char* buffer = (char *) malloc(size*sizeof(char));

	if(argc == 2){
		//Only stdin
		while(getline(&buffer, &size, stdin) > 0){
			if(strstr(buffer, argv[1]) != NULL){
				printf("%s", buffer);
			}
		}
	}else{
		for (int i = 2; i<argc; i++){
			FILE* file = fopen(argv[i], "r");
			if(file == NULL){
				printf("UVagrep: cannot open file\n");
				exit(1);
			}
			while(getline(&buffer, &size, file) > 0){
				if(strstr(buffer, argv[1]) != NULL){
					printf("%s", buffer);
				}
			}
		}
	}
	free(buffer);
	return(0);
}
