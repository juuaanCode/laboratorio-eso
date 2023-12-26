#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("UVazip: file1 [file2 ...]\n");
		exit(1);
	}
	char lastChar = EOF;
	int reps = 1;
	for(int i = 1; i<argc; i++){
		FILE* file = fopen(argv[i],"r");
		if (file == NULL){
			printf("UVazip: cannot open file\n");
			exit(1);
		}
		char ch2;
		while((ch2 = getc(file)) != EOF){
			if(ch2 != lastChar){
				if(lastChar != EOF) {
					fwrite(&reps, sizeof(int), 1, stdout);
					fwrite(&lastChar, sizeof(char), 1, stdout);
				}
				reps = 1;
				lastChar = ch2;
			} else {
				reps++;
			}
		}
		fclose(file);
	}
	//print last char
	fwrite(&reps, sizeof(int), 1, stdout);
	fwrite(&lastChar, sizeof(char), 1, stdout);
	return(0);
}
