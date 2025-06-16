#include "getFileSums.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void copySumData(struct FileSumData* dest, struct FileSumData* source){
      dest->filename = malloc(strlen(source->filename) + 1);
      strcpy(dest->filename, source->filename);
      strcpy(dest->sum, source->sum);
}

void FileSumNode_free(struct FileSumNode *node) {
  if (node == NULL) {
    return;
  }
  FileSumNode_free(node->next);

  free(node->data);
  free(node);
}

void fileSumList_append(struct FileSumList *list, struct FileSumData *data) {
  struct FileSumNode *node = malloc(sizeof(struct FileSumNode));
  node->next = NULL;
  node->data = data;

  if (list->head == NULL) {
    list->head = node;
    return;
  }

  struct FileSumNode *curr = list->head;
  while (curr->next != NULL) {
    curr = curr->next;
  }
  curr->next = node;
}

void fileSumList_free(struct FileSumList *list) {
  FileSumNode_free(list->head);
  free(list);
}

struct FileSumNode* fileSumList_getNode(struct FileSumList* list, char* filename) {
  struct FileSumNode* curr = list->head;
  while(curr != NULL){
    if(strcmp(curr->data->filename, filename) == 0) {
      break;
    }

    curr = curr->next;
  }

  return curr;
}

int getfilesSumsInDirectory(char *directory, struct FileSumList *files) {
  struct FileSumNode *file = files->head;

  while (file != NULL) {
    // create a pipe for comunicate with the child process
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
      perror("Cannot create pipe");
      return 1;
    }

    pid = fork();

    if (pid == 0) {
      // child process code
      close(pipefd[0]);

      // redirect stdout to pipe write end
      dup2(pipefd[1], STDOUT_FILENO);

      close(pipefd[1]);

      // create the argument for md5sum (create an absolute path for the file)
      char fileNameArg[512];
      snprintf(fileNameArg, 512, "%s%s", directory, file->data->filename);

      execlp("md5sum", "md5sum", fileNameArg, NULL);
      perror("error executing md5sum with execlp");
      exit(EXIT_FAILURE);
    } else {
      // parent process code
      // close pipe write end
      close(pipefd[1]);

      // read the sum through the pipe
      int bytesReaded = read(pipefd[0], file->data->sum, 32);
      file->data->sum[32] = 0;

      if (bytesReaded > 0) {
        // printf("Leido desde proceso hijo: %s\n", fileData->sum);
      } else {
        // printf("no se lello nada del proceso hijo\n");
      }

      close(pipefd[0]);

      wait(NULL);
      file = file->next;
    }
  }
	return 0;
}
