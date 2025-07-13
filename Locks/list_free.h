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
  int pointerCounter;
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
  // if (pthread_mutex_init(&ll->lock, NULL) != 0) { 
  //   printf("Failed to initialize mutex\n"); 
  //   return NULL;
  // } 
  return ll;
}

static inline int ll_destroy(struct linked_list* ll) {
  // pthread_mutex_lock(&ll->lock);
  if(ll->head == NULL){
    free(ll);
    // wont work since we are cannot edit the pointer value
    // ll = NULL;

    // pointer is gone anyways
    // pthread_mutex_unlock(&ll->lock);
    return 1;
  } else {
    // pthread_mutex_unlock(&ll->lock);
    return 0;
  }
}

static inline void ll_add(struct linked_list* ll, int value) {
  struct node* node = (struct node*) malloc(sizeof(struct node));
  // add as ll->head
  node->value = value;
  // node->toDelete = false;
  node->pointerCounter = 1;
  do{
    node->next = ll->head;
  } while(!__sync_bool_compare_and_swap(&ll->head, node->next, node));
  __sync_fetch_and_add(&ll->length, 1);
}

static inline int ll_length(struct linked_list* ll) {
  return ll->length;
}

static inline bool ll_remove_first(struct linked_list* ll) {
  if(ll->head == NULL){
    return false;
  }

  struct node* node;
  do{
    node = ll->head;
  } while(!__sync_bool_compare_and_swap(&ll->head, node, ll->head->next));
  // now have access to node, while stopping it from being free()'d by another thread
  // all we need to do, it check if it has any pointerCounters on it, and free() if none
  if(__sync_sub_and_fetch(&node->pointerCounter, 1) == 0){
    free(node);
  }
  __sync_fetch_and_sub(&ll->length, 1);
  return true;
}

static inline bool ll_remove_value(struct linked_list* ll, int value){
  struct node* node = ll->head;
  if(node == NULL)
    return false;
  __sync_fetch_and_add(&node->pointerCounter, 1); // unsafe, node could be free()'d at this point, but idk any other way
  struct node* prev = NULL;

  while(node != NULL){
    if(prev != NULL){
      if(__sync_sub_and_fetch(&prev->pointerCounter, 1) == 0){
        free(prev);
      }
    }

    // check condition
    if(node->value == value){
      if(prev == NULL){
        if(__sync_bool_compare_and_swap(&ll->head, node, node->next)){
          // now have access to node, while stopping it from being free()'d by another thread
          // all we need to do, it check if it has any pointerCounters on it, and free() if none
          if(__sync_sub_and_fetch(&node->pointerCounter, 1) == 0){
            free(node);
          }
          __sync_fetch_and_sub(&ll->length, 1);
          return true;
        }
      } else {
        if(__sync_bool_compare_and_swap(&prev->next, node, node->next)){
          // now have access to node, while stopping it from being free()'d by another thread
          // all we need to do, it check if it has any pointerCounters on it, and free() if none
          if(__sync_sub_and_fetch(&node->pointerCounter, 1) == 0){
            free(node);
          }
          __sync_fetch_and_sub(&ll->length, 1);
          return true;
        }
      }
    }

    prev = node;

    // CAS
    node = node->next;
    if(node != NULL)
      __sync_fetch_and_add(&node->pointerCounter, 1); // unsafe
  }
  return false;
}

static inline bool ll_contains(struct linked_list* ll, int value) {
  struct node* node = ll->head;
  if(node == NULL)
    return false;
  __sync_fetch_and_add(&node->pointerCounter, 1); // unsafe, node could be free()'d at this point, but idk any other way
  struct node* prev = NULL;

  while(node != NULL){
    if(prev != NULL){
      if(__sync_sub_and_fetch(&prev->pointerCounter, 1) == 0){
        free(prev);
      }
    }
    prev = node;

    // check condition
    if(node->value == value){
      return true;
    }

    // CAS
    node = node->next;
    if(node != NULL)
      __sync_fetch_and_add(&node->pointerCounter, 1); // unsafe, again, node could be free()'d at this point
  }
  return false;
}

#endif
