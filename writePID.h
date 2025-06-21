#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define PID_FILENAME "30871641_30370544_28314837.txt"

/* Writes the pid into the PID_FILENAME
*/
int writePID(int pid, int isChild);

/* Writes the pid into the PID_FILENAME
*/
int writeInterval(int interval);
