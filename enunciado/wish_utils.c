#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"
#define MAX_SIZE 100

void execute_error(int line){
	printf("An error has ocurred in line %i\n", line);
}

void execute_exit(int value){
	exit(0);
}

void execute_cd(char *newpath){
	char *path = strtok_r(newpath, " ", &newpath);
	if(path == NULL){
		execute_error(19);
	}
	else{
		if(chdir(path) == -1){
			execute_error(23);
		};
	}
}

char* concatenate_path(char* original){
	char *relative = "./";
	char *thepath = strdup(original);
	char *lp = "/";
	char *fp = malloc(sizeof(char*) * MAX_SIZE);
	strcpy(fp, relative);
	strncat(fp, thepath, strlen(thepath));
	strcat(fp, lp);
	free(thepath);
	return fp;
}

void execute_path(char** path, char *newpaths, int size){
	
	for(int i = 0; i < size; i++){
		path[i] = NULL;	
	}
	char* newpath;
	int index = 0;
	while((newpath = strtok_r(newpaths, " ", &newpaths))){
		if(strncmp(newpath, ".", 1) != 0 && strncmp(newpath, "/", 1) != 0){
			char *fp = concatenate_path(newpath);
			path[index] = malloc(strlen(fp) + 1);
			strcpy(path[index], fp);
			free(fp);
		}
		else{
			char *actualpath = strdup(newpath);
			strcat(actualpath, "/");
			path[index] = actualpath;
		}
		index++;
	} 
	path[index] = "";
	
	
}


