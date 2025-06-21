#include "writePID.h"
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

FILE *openPIDFile() {
  char *path_value = getenv("HOME");

  char filepath[100];
  FILE *file;

  snprintf(filepath, 100, "%s/%s", path_value, PID_FILENAME);

  file = fopen(filepath, "a");

  return file;
}

int writePID(int pid, int isChild) {
  FILE *file = openPIDFile();

  if (file == NULL) {
    return 1;
  }

  fprintf(file, "%d ", pid);
  if (isChild) {
    fprintf(file, "Hijo");
  } else {
    fprintf(file, "Padre");
  }
  fprintf(file, "\n");
  fclose(file);
  return 0;
}

int writeInterval(int interval) {
  FILE *file = openPIDFile();

  if (file == NULL) {
    return 1;
  }

  fprintf(file, "Intervalo %d\n", interval);
  fclose(file);
  return 0;
}
