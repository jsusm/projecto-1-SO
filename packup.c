#include "packup.h"
#include "writePID.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int compressPakFile(char *pakFile);

int verifyDirectoryExistence() {
  char *log_dir_parent = "/var/log/PROYECTO SO 1";
  char *log_dir = "/var/log/PROYECTO SO 1/logs";
  // check the parent directory of log_dir
  struct stat st;
  if (stat(log_dir_parent, &st) == -1) {
    // Make the syscall mkdir
    if (mkdir(log_dir_parent, 0755) == -1) {
      // Check if the directory could be created
      perror("Error creating log directory");
      return -1;
    }
  }

  // If the directory was found, check if it is actually a directory.
  else if (!S_ISDIR(st.st_mode)) {
    return -1; // The path is not a directory
  }

  // Use stat() to check, if the returned value is -1 means the directory
  // couldn't be found.
  if (stat(log_dir, &st) == -1) {
    // Make the syscall mkdir
    if (mkdir(log_dir, 0755) == -1) {
      // Check if the directory could be created
      perror("Error creating log directory");
      return -1;
    }
  }

  // If the directory was found, check if it is actually a directory.
  else if (!S_ISDIR(st.st_mode)) {
    return -1; // The path is not a directory
  }

  return 0;
}

int packupModifiedFiles(const char *logDirectory,
                        struct FileSumList *modifiedList) {
  FILE *pak_file = NULL;
  char pak_filepath[512];  // Path for .pak file
  char timestamp_str[128]; // To get a string containing the date-hour for file
                           // naming
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  if (verifyDirectoryExistence() ==
      -1) { // Verify if /var/log/PROYECTO SO 1/logs exists and is a Directory
    return 1;
  }

  // Asign a name for the .pak file
  strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%d-%H-%M-%S", t);
  snprintf(pak_filepath, sizeof(pak_filepath),
           "/var/log/PROYECTO SO 1/logs/%s.pak", timestamp_str);

  // Open .pak file to truncate files name
  pak_file = fopen(pak_filepath, "wb");
  if (pak_file == NULL) {
    perror("Error al abrir el archivo .pak de salida");
    return 1;
  }

  // Iterate over modifiedList to check if it is already in directory
  struct FileSumNode *current_file_node = modifiedList->head;
  while (current_file_node != NULL) {
    const char *filename_only = current_file_node->data->filename;
    char full_current_filepath[512];
    snprintf(full_current_filepath, sizeof(full_current_filepath), "%s%s",
             logDirectory, filename_only); // to write in full_current_filepath
                                           // the fill_path of this file

    FILE *current_file = NULL;
    struct stat file_stat;
    FileHeader header;
    char *buffer = NULL;

    // if file could not be found, go to next file
    if (stat(full_current_filepath, &file_stat) == -1) {
      current_file_node = current_file_node->next;
      continue;
    }

    // Fill FileHeader constructor
    memset(&header, 0, sizeof(FileHeader));

    // Truncate to 32 chars and assign file size as a 64 bit value
    strncpy(header.filename, filename_only, sizeof(header.filename) - 1);
    header.filename[sizeof(header.filename) - 1] = '\0';
    header.filesize = (uint64_t)file_stat.st_size;

    // Change name
    if (fwrite(&header, sizeof(FileHeader), 1, pak_file) != 1) {
      perror("Error al escribir la cabecera en el .pak");
      fclose(pak_file);
      return 1;
    }

    // Check file content to write, by reading the content
    current_file = fopen(full_current_filepath, "rb");
    if (current_file == NULL) {
      perror("Error al abrir el archivo de origen para lectura");
      current_file_node = current_file_node->next;
      continue;
    }

    // Read and write content in a buffer
    buffer = (char *)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
      perror("Error al asignar memoria para el búfer de lectura");
      fclose(current_file);
      fclose(pak_file);
      return 1;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, current_file)) > 0) {
      if (fwrite(buffer, 1, bytes_read, pak_file) != bytes_read) {
        perror("Error al escribir el contenido en el .pak");
        free(buffer);
        fclose(current_file);
        fclose(pak_file);
        return 1;
      }
    }

    free(buffer);
    fclose(current_file);

    current_file_node = current_file_node->next;
  }

  // Set last header to FIN
  FileHeader fin_header;
  memset(&fin_header, 0, sizeof(FileHeader));
  strncpy(fin_header.filename, "FIN", sizeof(fin_header.filename) - 1);
  fin_header.filename[sizeof(fin_header.filename) - 1] = '\0';
  fin_header.filesize = 0;

  if (fwrite(&fin_header, sizeof(FileHeader), 1, pak_file) != 1) {
    perror("Error al escribir la cabecera 'FIN'");
    fclose(pak_file);
    return 1;
  }

  fclose(pak_file);
  ;
  int success = compressPakFile(pak_filepath);
  if (success != 0) {
    perror("The file could not be compressed");
    return 1;
  }

  return 0;
}

int compressPakFile(char *pakFile) {

  int end = 0;
  pid_t pid;
  pid = fork(); // Create a child process

  if (pid == -1) {
    perror("Error creating fork");
    return -1;
  }

  else if (pid == 0) {
    // write pid
    writePID(getpid(), 1);

    execlp("gzip", "gzip", pakFile,
           (char *)NULL); // Execute syscall gz to compress file
    end = 1;
    // If the command return a value it means it wasn't successful
    perror("Error executing gzip command with execlp");
    exit(1); // Child process exits with failure status
  }

  else {
    if (end) {
      printf("Compressed Successfuly");
    }
  }
  return 0;
}
