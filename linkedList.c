#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

void node_init(struct Node* node){
	node->data= NULL;
	node->next= NULL;
}

int node_detroy(struct Node* node, int idx){
	if(node == NULL){
		printf("[WARN] trying to remove a node that doesn't exist");
	}
	if(idx == 1){
		if(node->next == NULL){
			printf("[WARN] trying to remove a node that doesn't exist");
			return 1;
		}
		struct Node* tmp = node->next->next;
		free(node->next);
		node->next = tmp;
		return 0;
	}else {
		return node_detroy(node->next, idx-1);
	}
}

void node_free_chain(struct Node* node) {
	if(node == NULL){
		return;
	}
	node_free_chain(node->next);
	free(node);
	return;
}

void list_append_back(struct List* list, char* data){
	struct Node* node = malloc(sizeof(struct Node));
	node_init(node);

	node->data = malloc(strlen(data) + 1);
	strcpy(node->data, data);

	if(list->head == NULL) {
		list->head = node;
	}else {
		struct Node* curr = list->head;
		while(curr->next != NULL){
			curr = curr->next;
		}
		curr->next = node;
	}
	list->length++;
}

void list_remove(struct List* list, int idx) {
	// go throug the list
	if(idx == 0){
		struct Node* tmp = list->head->next;
		free(list->head);
		list->head = tmp;
	}
	if(node_detroy(list->head, idx)  == 0) {
		list->length --;
	}
}

void list_free(struct List* list) {
	node_free_chain(list->head);
	free(list);
}

