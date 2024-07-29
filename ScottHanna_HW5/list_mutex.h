#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

// linked list structure, containing next pointer and value
// first pointer will be head, first value is only gotten by linked_list->next->value
struct linked_list{
  struct node* head;
  pthread_mutex_t lock;
  int length;
};

struct node{
  struct node* next;
  int value;
};

static inline struct linked_list* ll_create(void) {
  struct linked_list* ll = (struct linked_list*) malloc(sizeof(struct linked_list));
  if(ll == NULL){
    return NULL;
  }
  // set end of ll
  ll->head = NULL;
  ll->length = 0;
  
  // initialize mutex
  if (pthread_mutex_init(&ll->lock, NULL) != 0) { 
    printf("Failed to initialize mutex\n"); 
    return NULL;
  } 
  return ll;
}

static inline int ll_destroy(struct linked_list* ll) {
  pthread_mutex_lock(&ll->lock);
  if(ll->head == NULL){
    free(ll);
    // wont work since we are cannot edit the pointer value
    // ll = NULL;

    // pointer is gone anyways
    // pthread_mutex_unlock(&ll->lock);
    return 1;
  } else {
    pthread_mutex_unlock(&ll->lock);
    return 0;
  }
}

static inline void ll_add(struct linked_list* ll, int value) {
  struct node* node = (struct node*) malloc(sizeof(struct node));
  // add between ll and ll->next
  node->value = value;
  pthread_mutex_lock(&ll->lock);
  node->next = ll->head;
  ll->head = node;
  ll->length++;
  pthread_mutex_unlock(&ll->lock);
}

static inline int ll_length(struct linked_list* ll) {
  pthread_mutex_lock(&ll->lock);
  int tlen = ll->length;
  pthread_mutex_unlock(&ll->lock);
  return tlen;
}

static inline bool ll_remove_first(struct linked_list* ll) {
  // check that there exists a node to remove
  pthread_mutex_lock(&ll->lock);
  if(ll->head == NULL){
    pthread_mutex_unlock(&ll->lock);
    return false;
  }

  struct node* node = ll->head->next;
  free(ll->head);
  ll->head = node;
  ll->length--;
  pthread_mutex_unlock(&ll->lock);
  return true;
}

static inline bool ll_remove_value(struct linked_list* ll, int value){
  // could probably move inside? idk
  pthread_mutex_lock(&ll->lock);
  struct node* node = ll->head;
  struct node* prev = NULL;
  while(node != NULL){
    if(node->value == value){
      if(prev == NULL)
        ll->head = node->next;
      else
        prev->next = node->next;
      free(node);
      ll->length--;
      pthread_mutex_unlock(&ll->lock);
      return true;
    }
    prev = node;
    node = node->next;
  }
  pthread_mutex_unlock(&ll->lock);
  return false;
}

static inline int ll_contains(struct linked_list* ll, int value) {
  int counter = 1;
  // could probably move lock inside? idk
  pthread_mutex_lock(&ll->lock);
  struct node* node = ll->head;
  while(node != NULL){
    if(node->value == value){
      pthread_mutex_unlock(&ll->lock);
      return counter;
    }
    node = node->next;
    counter++;
  }
  pthread_mutex_unlock(&ll->lock);
  return 0;
}

#endif
