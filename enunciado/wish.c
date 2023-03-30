#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"
#include "wish_utils.c"

#define MAX_SIZE 100

char *mypath[] = { "bli","bla","/bin/",""};
int size = 4;

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
	int count = 0;
	char *argument;
	while((argument = strtok_r(arguments, " ", &arguments))){
		if(strcmp(argument, ">") == 0){
			count++;
		}		
	}
	if(count == 1){
		return 1;
		
	}
	return 0;
}

int check_sintaxis(char *command){
	if(check_arguments(command) == 0){
		return 0;
	}
	else if(check_redirection(command) == 0){
		return 0;
	}
	return 1;
}

int main(int argc, char *argv[]){

	char str[MAX_SIZE];
	char *command_string;
	char *s;
	int fd;

	do{
     		printf("whish> ");
     		fgets(str, MAX_SIZE, stdin);
		s = str;
                while(*s != '\n') {
                	++s;
                }
                *s = '\0';
		s = str;
                command_string = strtok_r(s, " ", &s);
		if(strcmp(command_string, "exit") == 0){
			execute_exit(0);
		}else if(strcmp(command_string, "cd") == 0){
			execute_cd(s);
		}else if(strcmp(command_string, "path") == 0){
			execute_path(mypath, s, size);
			size = 2;
		}else{
			fd = -1;
			char **mp = mypath;
			char specificpath[MAX_SIZE];
			while( (strcmp(*mp, "") != 0)  && fd != 0){
				strcpy(specificpath, *mp++);
				strncat(specificpath, command_string, strlen(command_string));
				fd = access(specificpath, X_OK);
			}
			if(check_sintaxis(s) == 0){
				execute_error();
			}
			else if(fd==0){
				
				int subprocess = fork();
				if(subprocess < 0){
					printf("Error launching the subprocess");
				}else if(subprocess == 0){
					char *myargs[3];
	  				myargs[0] = strdup(specificpath);
	  				if(strcmp(s, "") == 0){
	  					myargs[1] = NULL;
	  				}
	  				else{
	  					myargs[1] = strdup(s);
	  				}
	  				myargs[2] = NULL;
	  				execvp(myargs[0], myargs);
				}else{
					wait(NULL);
				}
			}else{
				execute_error();
			}
		}
   	}while(1);
}
