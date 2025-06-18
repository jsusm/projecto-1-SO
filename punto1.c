#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>

#define PATH "/etc/proyecto so 1/proy1.ini"
#define MAX 80

struct Configuration {
    int interval;
    char* log_tag;
};


int read_configuration(struct Configuration *config) {
    FILE *file = NULL;
    char line[MAX];
    char *value_start;
    config->log_tag = NULL;

    file = fopen(PATH, "r");
    if (!file) {
        openlog("ProyectoSO1_Init", LOG_PID | LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "Error opening configuration file");
        closelog();
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0'; ///eliminate the newline character (\n)

        /// Ignore empty lines or comments ';'
        if (line[0] == '\0' || line[0] == ';') {
            continue;
        }

        if ((value_start = strstr(line, "log_tag=")) == line) { /// save log_tag
        value_start = value_start + strlen("log_tag=");

            if (*value_start == '\0') {
                 /// Warning If the value is empty
                 syslog(LOG_WARNING, "Configuration file: 'log_tag' is empty. This entry will be skipped.");
                 continue;
            }
            config->log_tag = (char*) malloc(strlen(value_start) + 1);
            if (config->log_tag == NULL) {
                fclose(file);
            }
            strcpy(config->log_tag, value_start);
            
        }
        else if ((value_start = strstr(line, "interval=")) == line) { ///save interval
            if (sscanf(value_start, "interval=%d", &config->interval) == 1) {
                
            } 
        }
    }

    fclose(file);

    
    return 0;
}


int main() {
    struct Configuration config = {0, NULL};

    ///it is checked if the file exists.
    if (access(PATH, F_OK) != 0) {
        openlog("ProyectoSO1_Init", LOG_PID | LOG_CONS, LOG_USER);
        syslog(LOG_ERR, "Configuration file not found. Terminating");
        closelog();
        return 1;
    }

    if (read_configuration(&config) != 0) {
        return 1; 
    }
    printf("Valor de 'interval': %d segundos\n", config.interval);
    printf("Valor de 'log tag': '%s'\n", config.log_tag); 
     closelog();
    openlog(config.log_tag, LOG_PID | LOG_CONS, LOG_USER);

    // --- Syslog para probar la estructura ---
    syslog(LOG_INFO, "Configuration loaded successfully.");
    syslog(LOG_INFO, "Configured interval: %d seconds.", config.interval); // Usando config.interval
    syslog(LOG_INFO, "Configured log tag: '%s'.", config.log_tag);       // Usando config.log_tag
    syslog(LOG_DEBUG, "This is a debug message demonstrating the log tag: %s", config.log_tag); // Otra prueba


    closelog();

    return 0;
}