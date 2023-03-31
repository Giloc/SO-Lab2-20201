#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/wait.h>
#include "wish_utils.h"
#include "wish_utils.c"

#define MAX_SIZE 100

char *mypath[] = { "bli","bla","/bin/",""};
int size = 4;
int redirection = 0;
int concurrency = 0;
int buffer_size = 1024;
int arguments_number = 0;

int check_ampersand(char *line){
	if(strcmp(line, "\0") == 0){
		return 0;
	}
	else if(strcmp(line, "&") == 0){
		return 0;
	}
	return 1;
}

int check_arguments(char *arguments){
	char *arguments_copy = strdup(arguments);
	int count = 0;
	char *argument;
	while((argument = strtok_r(arguments_copy, " ", &arguments_copy))){
		if(strcmp(argument, ">") == 0){
			break;
		}
		count++;		
	}
	if(count <= 1){
		return 1;
	}
	return 0;
}

int check_redirection(char *arguments){
	redirection = 0;
	char *arguments_copy = strdup(arguments);
	int count = 0;
	char *argument;
	while((argument = strtok_r(arguments_copy, " ", &arguments_copy))){
		if(strcmp(argument, ">") == 0){
			count++;
			redirection = 1;
			if(strcmp(arguments_copy, "\n") == 0 || strcmp(arguments_copy, "\0") == 0){
				return 0;
			}
			
		}		
	}
	if(count <= 1){
		return 1;
	}
	redirection = 0;
	
	return 0;
}

char** get_arguments(char *command){
	if(command == NULL){
		return NULL;
	}
	else if(strcmp(command, "\0") == 0){
		return NULL;
	}
	char **arguments = malloc(sizeof(char*) * MAX_SIZE);
	char *argument;
	int count = 0;
	if(redirection == 1){
		char *fp = strtok_r(command, ">", &command);
		if(strcmp(command, "\0") == 0){
			return NULL;
		}
		while((argument = strtok_r(fp, " ", &fp))){
			arguments[count] = malloc(strlen(argument) + 1);
			strcpy(arguments[count], argument);
			count++;
		}
		
	}
	else{
		while((argument = strtok_r(command, " ", &command))){
			arguments[count] = malloc(strlen(argument) + 1);
			strcpy(arguments[count], argument);
			count++;
		}
	}
	
	arguments_number = count;
	return arguments;
}

char* get_redirection(char *command){
	char *file = strdup(command);
	char *file1 = strtok_r(file, ">", &file);
	if(strcmp(file, "\0") == 0){
		char *file2;
		if(file1[0] == ' '){
			file2 = strtok_r(file1, " ", &file1);
			return file2;
		}
		return file1;
	}
	else{
		char *file2;
		if(file[0] == ' '){
			file2 = strtok_r(file, " ", &file);
			return file2;
		}
		return file;
	}
	
}

char** separate_commands(char *line){
	int commands_count = 0;
	char *command;
	char **commands = malloc(sizeof(char*) * MAX_SIZE);
	while((command = strtok_r(line, "&", &line))){
		commands[commands_count] = malloc(strlen(command) + 1);
		strcpy(commands[commands_count], command);
		commands_count++;
	}
	//commands[commands_count] = malloc(strlen(line) + 1);
	//strcpy(commands[commands_count], line);
	commands[commands_count] = "";
	if(commands_count > 1){
		concurrency = 1;
	}
	else{
		concurrency = 0;
	}
	return commands;
}

int check_sintaxis(char *command){
	if(check_arguments(command) == 0){
		//printf("problema de args\n");
		//return 0;
	}
	else if(check_redirection(command) == 0){
		printf("problema de red\n");
		return 0;
	}
	return 1;
}

void execute_redirection(char *command, char* filename){
	FILE *fp;
	char buffer[buffer_size];
	
	fp = popen(command, "r");
	if(fp == NULL){
		printf("feo\n");
		exit(1);
	}
	char *dir = strtok_r(filename, " ", &filename);
	FILE *out = fopen(dir, "w");
	if(out == NULL){
		printf("error abriendo archivo\n");
		exit(1);
	}
	while(fgets(buffer, buffer_size, fp) != NULL){
		fprintf(out, "%s", buffer);
	}
	pclose(fp);
	fclose(out);
}

int main(int argc, char *argv[]){

	char str[MAX_SIZE];
	char *command_string;
	char *s;
	int fd;
	
	if(argc == 1){
		do{
     			printf("whish> ");
     			fgets(str, MAX_SIZE, stdin);
			s = str;
                	while(*s != '\n') {
                		++s;
                	}
                	*s = '\0';
			s = str;
			if(check_ampersand(s) == 1){
				char **commands = separate_commands(s);
				char *fcommand = strdup(commands[0]);
                		command_string = strtok_r(fcommand, " ", &fcommand);
				if(strcmp(command_string, "exit") == 0){
					execute_exit(0);
				}else if(strcmp(command_string, "cd") == 0){
					execute_cd(fcommand);
				}else if(strcmp(command_string, "path") == 0){
					execute_path(mypath, fcommand, size);
					size = 2;
				}else{
					int i = 0;
					int childrens = 0;
					while(strcmp(commands[i], "") != 0){
						char *command = commands[i];
						
                				command_string = strtok_r(command, " ", &command);
						fd = -1;
						char **mp = mypath;
						char specificpath[MAX_SIZE];
						while( (strcmp(*mp, "") != 0)  && fd != 0){
							strcpy(specificpath, *mp++);
							strncat(specificpath, command_string, strlen(command_string));
							fd = access(specificpath, X_OK);
						}
						if(check_sintaxis(command) == 0){
							execute_error(176);
						}
						else if(fd==0){
							
							if(redirection == 0){
								int subprocess = fork();
								if(subprocess < 0){
									printf("Error launching the subprocess");
								}else if(subprocess == 0){
									char *myargs[arguments_number + 2];
									char **specargs = get_arguments(command);
	  								myargs[0] = strdup(specificpath);
	  								if(specargs == NULL){
	  									myargs[1] = NULL;
	  								}
	  								else{
	  									for(int k = 1; k < arguments_number + 1; k++){
	  										myargs[k] = specargs[k - 1];
	  									}
	  									myargs[arguments_number + 1] = NULL;
	  								}
	  								execvp(myargs[0], myargs);
								}else{
									childrens++;
								}
							}
							else if(redirection == 1){
								char *direction = get_redirection(command);
								int subprocess = fork();
								if(subprocess < 0){
									printf("Error launching the subprocess");
								}
								else if(subprocess == 0){
									char *arguments;
									strcat(specificpath, " ");
									if(command[0] == 62){
										execute_redirection(specificpath, direction);
									}
									else{
										arguments = strtok_r(command, ">", &command);
										strcat(specificpath, arguments);
										execute_redirection(specificpath, command);
									}
									exit(0);
								}
								else{
									childrens++;
								}
							}
							
						}else{
							execute_error(201);
						}
						i++;
					}
					int status;
					while(childrens > 0){
						pid_t pid = waitpid(-1, &status, WNOHANG);
						if(pid > 0){
							childrens--;
						}
					}
					
				}
			}
			else{
				//execute_error(223);
			}
   		}while(1);
	}
	else if(argc == 2){
		//do something
	}
	else{
		execute_error(220);
	}
	
}
