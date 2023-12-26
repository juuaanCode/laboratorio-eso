#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("UVaunzip: file1 [file2 ...]\n");
		exit(1);
	}
	for (int i = 1; i<argc; i++){
		FILE* file = fopen(argv[i], "r");
		if(file == NULL){
			printf("UVaunzip: cannot open file");
			exit(1);
		}
		int reps = 0;
		char ch = EOF;

		while(getc(file) != EOF){
			fseek(file, -1, SEEK_CUR);
			fread(&reps, sizeof(int), 1, file);
			fread(&ch, sizeof(char), 1, file);
			for (int j = 0; j<reps; j++){
				fwrite(&ch, sizeof(char), 1, stdout);
			}
		}
		fclose(file);
	}
	return(0);
}
