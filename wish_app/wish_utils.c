#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"

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
		chdir(path);
	}
}

void execute_path(){
	printf("path executed\n");
}


