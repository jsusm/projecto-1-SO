#include "loadConfig.h"
#include "writePID.h"
#include <dirent.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

struct Configuration* conf;

void _syslog(char *value, int priority) {
  if(conf == NULL){
    printf("trying to loggin without loading configuration.");
    return;
  }
  openlog(conf->log_tag, LOG_PID | LOG_CONS, LOG_USER);
  syslog(priority, "%s", value);
  closelog();
}

void setConf(struct Configuration* _conf){
  conf = _conf;
}
