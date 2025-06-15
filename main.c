#include "linkedList.h"
#include "sayHello.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

struct FileSumData {
  int pipefd[2];
  char *filename;
  char sum[33];
};

struct FileSumNode {
  struct FileSumData *data;
  struct FileSumNode *next;
};

void FileSumNode_free(struct FileSumNode *node) {
  if (node == NULL) {
    return;
  }
  FileSumNode_free(node->next);

  free(node->data);
  free(node);
}

struct FileSumList {
  struct FileSumNode *head;
};

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

// interfaces to read
struct Configuration {
  int interval;
  char *log_tag;
};

// directory we are goint to read from
const char *content_directory = "/var/log/";

int main() {

  struct Configuration conf = {10, "PROY_SO_1"};

  struct List *files = malloc(sizeof(struct List));

  // read directory
  DIR *dir;
  struct dirent *ent;

  dir = opendir(content_directory);
  if (dir == NULL) {
    perror("could not open directory");
    exit(EXIT_FAILURE);
  }
  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_type == DT_REG) { // is a regular file
      list_append_back(files, ent->d_name);
    }
  }
  closedir(dir);

  // // read the list
  // puts("Files:");
  // struct Node* file = files->head;
  // int idx = 0;
  // while(file != NULL){
  // 	idx++;
  // 	printf("\t%s, idx: %d\n", file->data, idx);
  // 	file = file->next;
  // }

  struct Node *file = files->head;

  struct FileSumList *fileSumPList = malloc(sizeof(struct FileSumList));
  fileSumPList->head = NULL;

  while (file != NULL) {
    // create a pipe for comunicate with the child process
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
      perror("Cannot create pipe");
      list_free(files);
      exit(EXIT_FAILURE);
    }

    // create a fileSum node
    struct FileSumData *fileData = malloc(sizeof(struct FileSumData));

    fileData->filename = malloc(strlen(file->data) + 1);
    strcpy(fileData->filename, file->data);
    fileData->sum[0] = '\0';

    pid = fork();

    if (pid == 0) {
      // child process code
      close(pipefd[0]);

      // redirect stdout to pipe write end
      dup2(pipefd[1], STDOUT_FILENO);

      close(pipefd[1]);

      char fileNameArg[512];
      snprintf(fileNameArg, 512, "%s%s", content_directory, fileData->filename);

      execlp("md5sum", "md5sum", fileNameArg, NULL);
      perror("error executing md5sum with execlp");
      exit(EXIT_FAILURE);
    } else {
      // parent process code
      // close pipe write end
      close(pipefd[1]);

      int bytesReaded = read(pipefd[0], fileData->sum, 32);

      if (bytesReaded > 0) {
        printf("Leido desde proceso hijo: %s\n", fileData->sum);
      } else {
        printf("no se lello nada del proceso hijo\n");
      }

      close(pipefd[0]);

      fileSumList_append(fileSumPList, fileData);
      wait(NULL);
      file = file->next;
    }
  }

  puts("Files and sums:");
  struct FileSumNode *fileSumNode = fileSumPList->head;
  while (fileSumNode != NULL) {
    printf("\tfilename: %s, sum: %s\n", fileSumNode->data->filename,
           fileSumNode->data->sum);
    fileSumNode = fileSumNode->next;
  }

  list_free(files);
  return 0;
}
