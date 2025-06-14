#include "sayHello.h"
#include "linkedList.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// interfaces to read
struct Configuration {
	int interval;
	char* log_tag;
};

// directory we are goint to read from
const char* content_directory = "/var/log/";

int main(){

	struct Configuration conf = {10, "PROY_SO_1"};

	struct List* files = malloc(sizeof(struct List));

	// leer el directorio
	DIR* dir;
	struct dirent* ent;
	
	dir = opendir(content_directory);
	if(dir == NULL){
		perror("could not open directory");
		exit(EXIT_FAILURE);
	}
	while((ent = readdir(dir)) != NULL){
		if(ent->d_type == DT_REG){// is a regular file
			list_append_back(files, ent->d_name);
		}
	}
	closedir(dir);

	// read the list
	puts("Files:");
	struct Node* file = files->head;
	int idx = 0;
	while(file != NULL){
		idx++;
		printf("\t%s, idx: %d\n", file->data, idx);
		file = file->next;
	}
	
	list_free(files);
	return 0;
}
