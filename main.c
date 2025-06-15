#include "linkedList.h"
#include "sayHello.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "getFileSums.h"

// interfaces to read
struct Configuration {
  int interval;
  char *log_tag;
};

// directory we are goint to read from
char *content_directory = "/var/log/";

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

  // Get file sums
  struct FileSumList *fileSumList = malloc(sizeof(struct FileSumList));
  fileSumList->head = NULL;
  
  getfilesSumsInDirectory(content_directory, files, fileSumList);

  // print file sums
  puts("Files and sums:");
  struct FileSumNode *fileSumNode = fileSumList->head;
  while (fileSumNode != NULL) {
    printf("\tfilename: %s, sum: %s\n", fileSumNode->data->filename,
           fileSumNode->data->sum);
    fileSumNode = fileSumNode->next;
  }

  list_free(files);
  return 0;
}
