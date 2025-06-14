#ifndef LINKED_LIST_H
#define LINKED_LIST_H


struct Node {
  struct Node *next;
  char *data;
};

/* Initilize a node set next to null
**/
void node_init(struct Node *node);

/* Delete memory address of the idx'th node
**/
int node_detroy(struct Node *node, int idx);

/* Delete all linked nodes
**/
void node_free_chain(struct Node *node);

struct List {
  struct Node *head;
  int length;
};

/* Add a node at the end of the list, copy the data string
**/
void list_append_back(struct List *list, char *data);

/* Removes the idx'th node in the list
**/
void list_remove(struct List *list, int idx);

/* Free the memory of the list and it's nodes
**/
void list_free(struct List *list);
#endif // !LINKED_LIST_H
