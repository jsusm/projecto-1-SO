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

  struct FileSumList *lastFiles = malloc(sizeof(struct FileSumList));
  lastFiles->head = NULL;

  // read content_directory and get the file sums
  // every conf->interval seconds
  while (1) {
    struct FileSumList *currentFiles = malloc(sizeof(struct FileSumList));
    currentFiles->head = NULL;

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
        struct FileSumData *filedata = malloc(sizeof(struct FileSumData));

        filedata->filename = malloc(strlen(ent->d_name) + 1);
        strcpy(filedata->filename, ent->d_name);
        filedata->sum[32] = 0;

        fileSumList_append(currentFiles, filedata);
      }
    }
    closedir(dir);

    // Get file sums
    getfilesSumsInDirectory(content_directory, currentFiles);

    // compute the difference between the current files and the last files
    // that means the files that changes
    struct FileSumList *changedFiles = malloc(sizeof(struct FileSumList));
    changedFiles->head = NULL;
    // iterate over the currentFiles and check if the file exists in lastFiles
    // if it don't add it to changedFiles otherwise, compare the sum and if
    // dont match add it to changedFiles
    struct FileSumNode *file = currentFiles->head;
    while (file != NULL) {
      struct FileSumNode *lastFile =
          fileSumList_getNode(lastFiles, file->data->filename);
      // TODO: try to combine the body of this two ifs
      if (lastFile == NULL) {
        struct FileSumData *data = malloc(sizeof(struct FileSumData));
        copySumData(data, file->data);
        fileSumList_append(changedFiles, data);
      } else {
        if (strcmp(lastFile->data->sum, file->data->sum) != 0) {
          struct FileSumData *data = malloc(sizeof(struct FileSumData));
          copySumData(data, file->data);
          fileSumList_append(changedFiles, data);
        }
      }
      file = file->next;
    }

    // print file sums
    if (changedFiles->head == NULL) {
      puts("No files has changed");
    } else {

      puts("Files That Change");
      struct FileSumNode *fileSumNode = changedFiles->head;
      while (fileSumNode != NULL) {
        printf("\tfilename: %s, sum: %s\n", fileSumNode->data->filename,
               fileSumNode->data->sum);
        fileSumNode = fileSumNode->next;
      }
    }

    // copy currentfiles into lastFiles
    // first we delete lastFiles
    fileSumList_free(lastFiles);
    // and create a fresh one
    lastFiles = malloc(sizeof(struct FileSumList));
    lastFiles->head = NULL;
    // then copy every data file entry and appendit to last files
    struct FileSumNode *f = currentFiles->head;
    while (f != NULL) {
      struct FileSumData *data = malloc(sizeof(struct FileSumData));
      copySumData(data, f->data);
      fileSumList_append(lastFiles, data);
      f = f->next;
    }

    fileSumList_free(currentFiles);
    sleep(conf.interval);
  }
  return 0;
}
