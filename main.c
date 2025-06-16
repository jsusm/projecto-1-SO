#include "getFileSums.h"
#include "sayHello.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// interfaces to read
struct Configuration {
  int interval;
  char *log_tag;
};

// directory we are goint to read from
char *content_directory = "/var/log/";

int main() {

  struct Configuration conf = {2, "PROY_SO_1"};

  // read content_directory and get the file sums
  // every conf->interval seconds
  while (1) {
    struct FileSumList *fileSumList = malloc(sizeof(struct FileSumList));
    fileSumList->head = NULL;

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
        // create file data structure
        struct FileSumData* filedata = malloc(sizeof(struct FileSumData));

        filedata->filename = malloc(strlen(ent->d_name) + 1);
        strcpy(filedata->filename, ent->d_name);
        filedata->sum[32]=0;

        fileSumList_append(fileSumList, filedata);
      }
    }
    closedir(dir);

    // Get file sums
    getfilesSumsInDirectory(content_directory, fileSumList);

    // print file sums
    puts("Files and sums:");
    struct FileSumNode *fileSumNode = fileSumList->head;
    while (fileSumNode != NULL) {
      printf("\tfilename: %s, sum: %s\n", fileSumNode->data->filename,
             fileSumNode->data->sum);
      fileSumNode = fileSumNode->next;
    }

    fileSumList_free(fileSumList);
    sleep(conf.interval);
  }
  return 0;
}
