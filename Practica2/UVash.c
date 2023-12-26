/* Juan González Arranz
/  UVash - Estructuras de Sistemas Operativos
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define ERROR_MSG "An error has occurred\n"
#define MAX_ARGS 256

struct processedCommand {
	/* Node for argv + redirection of each command. Linked list.*/
	char** argv;
	FILE* output;
	struct processedCommand * nextCommand;
};

void cleanup(struct processedCommand * line){
	/* Frees all content of the struct, closes opened files and then frees the struct itself.*/
	while(line){
		free(line->argv);
		line->argv = NULL;
		if(line->output){
			fclose(line->output);
			line->output = NULL;
		}
		struct processedCommand * next = line->nextCommand;
		free(line);
		line = next;
	}
}

struct processedCommand * newprocessedCommand(){
	/* Provides a default new node for the list of processed lines of commands,
	/  with memory allocated to the argv array, and NULL file and next node pointers.
	/  RETURN: the pointer to the new node. In case an error occours, pointer is NULL.
	/  Supports a resasonable max of 255 args + command itself per command in the line.
	*/
	struct processedCommand * line = (struct processedCommand *)malloc(sizeof(struct processedCommand));
	if (!line){
		return(NULL);
	}

	line->argv = (char **)malloc(MAX_ARGS*sizeof(char*));
	if (!line->argv){
		return(NULL);
	}

	line->output = NULL;
	line->nextCommand = NULL;
	return(line);
}

int commandCreator(char* buffer, struct processedCommand * line){
	/* INPUT: line of input, pointer to struct processedCommand, a linked list with args and redirected
	/  output for each command in the line.
	/  RETURN: 0 on success, in the event of an input error (e.g. two ">") then -1.
	*/
	int iter = 0;
	char* commandLine;
	while((commandLine = strsep(&buffer, "&"))){
		if (iter){ //Do not run this the first time
			line->nextCommand = newprocessedCommand();
			line = line->nextCommand;
			if (!line){
				return(-1);
			}
		}

		if(!strcmp(commandLine, "")){
			//Empty before "&"
			return(-1);
		}

		//Command + args
		char* command = strsep(&commandLine, ">");
		if(!strcmp(command, "")){
			//Empty before ">"
			return(-1);
		}

		int i = 0;
		char* arg;
		while ((arg = strsep(&command, " \t\n"))){
			if (strcmp(arg, "")){
				line->argv[i] = arg;
				i++;
			}
		}
		line->argv[i] = NULL;

		//Output redirection
		char* redirect;
		if ((redirect = strsep(&commandLine, ">"))){
			//A ">" symbol is detected
			char* possibleOut = strsep(&redirect, " \t\n");
			while(possibleOut != NULL && !strcmp(possibleOut, "")){
				//Keep searching
				possibleOut = strsep(&redirect, " \t\n");
			}
			if (!possibleOut){
				//End was reached and no word was read
				return(-1);
			}

			//Check no more words after ">"
			char* extraSearch;
			while((extraSearch = strsep(&redirect, " \t\n"))){
				if(strcmp(extraSearch, "")){
					//Fail if a word is detected
					return(-1);
				}
			}

			//Check no more ">"
			if((extraSearch = strsep(&commandLine, ">"))){
				return(-1);
			}
			line->output = fopen(possibleOut,"w+");
			if (!line->output){
				//Error opening file
				return(-1);
			}
		}
		iter++;
	}
	return(0);
}


int main(int argc, char* argv[]){
	FILE* input = stdin;

	if (argc > 2){
		fprintf(stderr, "%s", ERROR_MSG);
		exit(1);
	}

	if(argc == 2){
		input = fopen(argv[1],"r");
		if (!input){
			fprintf(stderr, "%s", ERROR_MSG);
			exit(1);
		}
	}

	size_t bufferInitialSize = 64;
	char* buffer =  (char*) malloc(bufferInitialSize*sizeof(char));
	if (!buffer){
		fprintf(stderr, "%s", ERROR_MSG);
		exit(1);
	}

	/*MAIN LOOP*/
	while(1){
		if(argc == 1) printf("UVash> ");

		if (getline(&buffer, &bufferInitialSize, input) == -1){
			fclose(input);
			free(buffer);
			exit(0); //Read from input and check if batch has ended or EOF
		}else{
			struct processedCommand * line = newprocessedCommand();
			if (!line){
				fprintf(stderr, "%s", ERROR_MSG);
				exit(1);
			}

			struct processedCommand * ogLine = line; //backup for cleanup

			if(commandCreator(buffer, line) == -1){
				fprintf(stderr, "%s", ERROR_MSG);
			}else{
				while(line){
					if(line->argv[0]){ //Check for no command
						if (!strcmp(line->argv[0], "exit")){
							//exit must have 0 args
							if (line->argv[1] != NULL){
								 fprintf(stderr, "%s", ERROR_MSG);
							}
							cleanup(ogLine);
							fclose(input);
							free(buffer);
							exit(0);
						}else if (!strcmp(line->argv[0], "cd")) {
							//cd must have only 1 arg
							if (!line->argv[1]){
								fprintf(stderr, "%s", ERROR_MSG);
							}else{
								if(line->argv[2]){
									fprintf(stderr, "%s", ERROR_MSG);
								}
								chdir(line->argv[1]);
							}
						}else{
							//normal commands
							pid_t pid = fork();
							if(pid < 0){
								fprintf(stderr, "%s", ERROR_MSG);
							} else if (pid == 0){
								if (line->output){
									dup2(fileno(line->output), 1);
									dup2(fileno(line->output), 2);
								}
								execvp(line->argv[0], line->argv);
								//if it returns there was a problem
								fprintf(stderr, "%s", ERROR_MSG);
								exit(1);
							}
						}
					}
					line = line->nextCommand;
				}
			}
			while(wait(NULL) > 0){/*wait*/;}
			cleanup(ogLine);
		}
	}
}
