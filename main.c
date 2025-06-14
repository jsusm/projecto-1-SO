#include "sayHello.h"
#include "linkedList.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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

	// read directory
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
	

	// create a pipe for comunicate with the child process
	int pipefd[2];
	pid_t pid;

	if(pipe(pipefd) == -1){
		perror("Cannot create pipe");
		list_free(files);
		exit(EXIT_FAILURE);
	}

	pid = fork();

	if(pid == 0) {
		// child process code
		close(pipefd[0]);

		// redirect stdout to pipe write end
		dup2(pipefd[1], STDOUT_FILENO);

		close(pipefd[1]);

		char fileNameArg[512];
		snprintf(fileNameArg, 512, "%s%s", content_directory, files->head->data);

		execlp("md5sum", "md5sum", fileNameArg, NULL);
		perror("error executing md5sum with execlp");
		exit(EXIT_FAILURE);
	}else {
		// parent process code
		// close pipe write end
		close(pipefd[1]);
		
		// 33 bytes giving space for \0
		char sumBuffer[33];

		int bytesReaded = read(pipefd[0], sumBuffer, 32);
		if(bytesReaded > 0) {
			printf("Leido desde proceso hijo: %s\n", sumBuffer);
		}else {
			printf("no se lello nada del proceso hijo\n");
		}

		close(pipefd[0]);
		wait(NULL);
	}

	list_free(files);
	return 0;
}
