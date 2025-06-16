#include "linkedList.h"

/** Structure to hold filename and data in a linked list
 */
struct FileSumData {
  int pipefd[2];
  char *filename;
  char sum[33];
};

/** Linked List Node structure
 */
struct FileSumNode {
  struct FileSumData *data;
  struct FileSumNode *next;
};

/** Free the memory of the FileSumNode and it's data field
 */
void FileSumNode_free(struct FileSumNode *node);

/** Structure to hold and manage the linked list
 */
struct FileSumList {
  struct FileSumNode *head;
};

/** Append a node with the data field at the end of the list
 * in recursive
 */
void fileSumList_append(struct FileSumList *list, struct FileSumData *data);

/** Free the list and its nodes
 */
void fileSumList_free(struct FileSumList *list);

/** Populate the list with sum of the files in the files list
 * with md5sum, it creates a child process for each entry in
 * the list, returns 1 in failure
 */
int getfilesSumsInDirectory(char *directory, struct FileSumList *files);
