#include "getFileSums.h"
#include "loadConfig.h"
#include "log.h"
#include "packup.h"
#include "writePID.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <sys/wait.h>
#include <unistd.h>

// directory we are goint to read from
char *content_directory = "/var/log/";

int main() {

  /* ---------- Load configuration ---------- */

  struct Configuration *conf = malloc(sizeof(struct Configuration));

  /// it is checked if the file exists.
  if (access(CONFIG_PATH, F_OK) != 0) {
    openlog("ProyectoSO1_Init", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_ERR, "Configuration file not found. Terminating");
    closelog();
    return 1;
  }

  if (load_configuration(conf) != 0) {
    return 1;
  }
  setConf(conf);
  char *message = (char *)malloc(256);
  sprintf(message, "Valor de 'interval': %d segundos\n", conf->interval);
  _syslog(message, LOG_INFO);
  sprintf(message, "Valor de 'log tag': '%s'\n", conf->log_tag);
  _syslog(message, LOG_INFO);

  /* ---------- Check for changes in content_directory ---------- */

  struct FileSumList *lastFiles = malloc(sizeof(struct FileSumList));
  lastFiles->head = NULL;

  writePID(getpid(), 0);

  int interval = 0;

  // read content_directory and get the file sums
  // every conf->interval seconds
  while (1) {
    writeInterval(interval++);
    struct FileSumList *currentFiles = malloc(sizeof(struct FileSumList));
    currentFiles->head = NULL;

    // read directory
    DIR *dir;
    struct dirent *ent;

    dir = opendir(content_directory);
    // this should never happen
    if (dir == NULL) {

      sprintf(message, "could not open the directory: '%s'\n",
              content_directory);
      _syslog(message, LOG_ERR);

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

    if (changedFiles->head == NULL) {
      _syslog("No files has changed\n", LOG_INFO);
    } else {
      _syslog("Changes detected\n", LOG_INFO);
    }

    /* ---------- Create .pak file and compress ---------- */
    if (packupModifiedFiles(content_directory, changedFiles) != 0) {
      _syslog("Error creating .pak files or compressing them", LOG_ERR);
    }

    /* ---------- Clean up lists ---------- */

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
    sleep(conf->interval);
  }
  return 0;
}
