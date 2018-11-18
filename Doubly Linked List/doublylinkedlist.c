/* Yanling Wang Fall 2018 */
#include <stdio.h>
#include <stdlib.h>
#include "doublylinkedlist.h"

void insertHead(doublyLinkedList *listPtr, int value) {
//Create a new head node
  struct listNode *newNode = (struct listNode*)malloc(sizeof(struct listNode));
  newNode->value = value;
//If the list is empty, insert the new head node
if((listPtr->head == NULL)&&(listPtr->tail == NULL)){
  newNode->next = NULL;
  newNode->prev = NULL;
  listPtr->head = newNode;
  listPtr->tail = newNode;
}else{
//If list is not empty, change the headnode
  newNode->next = listPtr->head;
  newNode->prev = NULL;
  listPtr->head->prev = newNode;
  listPtr->head = newNode;
}
 return;
}

int removeTail(doublyLinkedList *listPtr) { 
//Store removed value  
int removedVal=(listPtr->tail->value);
//If there is more than 1 node in list, remove the current tail and update new tail
if(listPtr->tail->prev != NULL){
  listPtr->tail = listPtr->tail->prev;
  free(listPtr->tail->next);
  listPtr->tail->next=NULL;
  return removedVal;
}else{
//If there is 1 node in the list, remove the current tail and update new tail
  free(listPtr->tail);
  listPtr->head = NULL;
  listPtr->tail = NULL;
  return removedVal;
  }
// NOTE: edge case for empty list is already handled
} 

void showHead(doublyLinkedList *listPtr) {
  listNode *head = listPtr->head;
  printf("list head: ");
  while (head != NULL) {
    printf("%d ", head->value);
    head = head->next;
  } 
  printf("nil\n");
  return;
} 
void showTail(doublyLinkedList *listPtr) {
  listNode *tail = listPtr->tail;
  printf("list tail: ");
  while (tail != NULL) {
    printf("%d ", tail->value);
    tail = tail->prev;
  } 
  printf("nil\n");
  return;
} 
void freeList(doublyLinkedList *listPtr) { 
  listNode *head = listPtr->head;
  listNode *temp;
  while(head != NULL) {
    temp = head->next;
    free(head);
    head = temp;
  }
  listPtr->head = NULL;
  listPtr->tail = NULL;
  return;
}

