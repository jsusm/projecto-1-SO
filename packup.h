#ifndef packup_h
#define packup_h

#include <stdint.h>

typedef struct FileHeader{};

int verifyDirectoryExistense(){};

int packupModifiedFiles(const char *logDirectory, struct List *modifiedList){};

int compressPak(const char *pakFile){};

#endif