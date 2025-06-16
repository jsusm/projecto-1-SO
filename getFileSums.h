#include "linkedList.h"

/** Structure to hold filename and data in a linked list
 */
struct FileSumData {
  char *filename;
  char sum[33];
};

void copySumData(struct FileSumData* dest, struct FileSumData* source);

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

/** Returns the pointer to the FileSumNode that match the filename
 * returns NULL if is not in the list
 */
struct FileSumNode* fileSumList_getNode(struct FileSumList* list, char* filename);

/** Populate the list with sum of the files in the files list
 * with md5sum, it creates a child process for each entry in
 * the list, returns 1 in failure
 */
int getfilesSumsInDirectory(char *directory, struct FileSumList *files);
