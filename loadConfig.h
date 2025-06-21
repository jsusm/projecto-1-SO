#ifndef LOAD_CONF_H
#define LOAD_CONF_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define CONFIG_PATH "/etc/proyecto so 1/proy1.ini"
#define MAX 80

struct Configuration {
  int interval;
  char *log_tag;
};

int load_configuration(struct Configuration *config);
#endif
