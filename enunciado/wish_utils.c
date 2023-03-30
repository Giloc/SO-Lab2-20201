#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"
#define MAX_SIZE 100

void execute_error(){
	printf("An error has ocurred\n");
}

void execute_exit(int value){
	exit(0);
}

void execute_cd(char *newpath){
	char *path = strtok_r(newpath, " ", &newpath);
	if(path == NULL){
		execute_error();
	}
	else{
		if(chdir(path) == -1){
			execute_error();
		};
	}
}

void execute_path(char** path, char *newpaths, int size){
	char *relative = "./";
	for(int i = 0; i < size; i++){
		path[i] = NULL;	
	}
	if(strncmp(newpaths, ".", 1) != 0 && strncmp(newpaths, "/", 1) != 0){
		char *thepath = strdup(newpaths);
		char *lp = "/";
		char fp[MAX_SIZE];
		strcpy(fp, relative);
		strncat(fp, thepath, strlen(thepath));
		strcat(fp, lp);
		path[0] = malloc(strlen(fp) + 1);
		strcpy(path[0], fp);
		free(thepath);
	}
	else{
		path[0] = strdup(newpaths);
	}
	path[1] = "";
}


