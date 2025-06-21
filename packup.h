#ifndef packup_h
#define packup_h

#include <stdint.h>
#include "getFileSums.h"

typedef struct {
    char filename[32];
    uint64_t filesize; 
} FileHeader;

#define BUFFER_SIZE 4096

int verifyDirectoryExistense();

int packupModifiedFiles(const char *logDirectory, struct FileSumList *modifiedList);

int compressPak(const char *pakFile);

#endif
