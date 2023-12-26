#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){
	if (argc == 1){
		exit(0);
	}

	char buffer[30];
	for (int i = 1; i < argc; i++){
		FILE* file = fopen(argv[i], "r");
		if (file == NULL){
			printf("UVacat: no puedo abrir fichero\n");
			exit(1);
		}

		while (fgets(buffer, 30, file) != NULL){
			printf("%s", buffer);
		}
		fclose(file);
	}
	return(0);
}
