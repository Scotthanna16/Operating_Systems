#ifndef THREADING_TESTING_H
#define THREADING_TESTING_H
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

// prints linked list as head->val1->val2->null
void print_list(struct linked_list* ll){
  struct node* node = ll->head;
  printf(" - head");
  while(node != NULL){
    printf("->%d", node->value);
    node = node->next;
  }
  printf("->null");
  printf("\n");
}

// // prints whether the list contains the value, easier to see for certain tests
// int contains_iteration(struct linked_list* ll, int max_len){
//   if(max_len <= -1)
//     max_len = ll_length(ll);
//   printf(" contains list:\n");
//   for(int i = 0; i < max_len; i++){
//     printf("%d -> %s\n", i, ll_contains(ll, i) ? "True" : "False");
//   }
//   printf("\n");
//   return 0;
// }

// random number 0 to 10
int rn(){
  return rand() % 10;
}

// adds 1,2,3
void* thread0(void* arg){
  struct linked_list* ll = (struct linked_list*) arg;

  usleep(rand() % 1000000);
  ll_add(ll, 1);

  usleep(rand() % 1000000);
  ll_add(ll, 2);
  
  usleep(rand() % 1000000);
  ll_add(ll, 3);

  return 0;
}

// adds and removes_first 3 values, while calling contains() and length() inbetween
void* thread1(void* arg){
  // will print if one of the removes failed, 
  // and thus the list will be non zero length at the end
  bool removeCalls = false;

  struct linked_list* ll = (struct linked_list*) arg;
  usleep(rand() % 1000000);
  ll_add(ll, rn());

  usleep(rand() % 1000000);

  usleep(rand() % 1000000);
  ll_add(ll, rn());

  usleep(rand() % 1000000);
  removeCalls = removeCalls || !ll_remove_first(ll);

  usleep(rand() % 1000000);
  ll_contains(ll ,rn());

  usleep(rand() % 1000000);
  removeCalls = removeCalls || !ll_remove_first(ll);

  usleep(rand() % 1000000);
  ll_add(ll, rn());

  usleep(rand() % 1000000);
  removeCalls = removeCalls || !ll_remove_first(ll);

  usleep(rand() % 1000000);
  ll_contains(ll ,rn());

  usleep(rand() % 1000000);
  ll_length(ll);

  if(removeCalls)
    printf("removeFailed: %d\n", removeCalls);
  return 0;
}

// adds 3 specific values, and removes them, while checking if they exist in list
void* thread2(void* arg){
  bool removeCalls = false;
  bool containsCalls = false;
  int random[3];
  random[0] = rn();
  random[1] = rn();
  random[2] = rn();

  struct linked_list* ll = (struct linked_list*) arg;
  usleep(rand() % 1000000);
  ll_add(ll, random[0]);

  usleep(rand() % 1000000);

  usleep(rand() % 1000000);
  
  ll_add(ll, random[1]);

  usleep(rand() % 1000000);
  removeCalls = removeCalls || !ll_remove_value(ll, random[0]);

  usleep(rand() % 1000000);
  containsCalls = containsCalls || !ll_contains(ll ,random[1]);

  usleep(rand() % 1000000);
  removeCalls = removeCalls || !ll_remove_value(ll, random[1]);

  usleep(rand() % 1000000);
  ll_add(ll, random[2]);

  usleep(rand() % 1000000);
  containsCalls = containsCalls || !ll_contains(ll ,random[2]);

  usleep(rand() % 1000000);
  removeCalls = removeCalls || !ll_remove_value(ll, random[2]);

  usleep(rand() % 1000000);
  ll_length(ll);

  if(removeCalls)
    printf("Remove Failed: %d\n", removeCalls);
  if(containsCalls)
    printf("Contains Failed: %d\n", containsCalls);
  return 0;
}


int testCase0(const int numThreads){
  srand(time(NULL));
  struct linked_list* ll = ll_create();
  pthread_t threads[numThreads];

  printf("\n----===TEST CASE 0===---\n");
  printf("Starts threads that add 1,2,3 with sleep in between\n");

  for(int i = 0; i < numThreads; i++){
    pthread_create(&threads[i], NULL, thread0, ll);
  }
  printf("waiting for threads\n");
  printf("Threads ");
  for(int i = 0; i < numThreads; i++){
    pthread_join(threads[i], NULL);
    printf("%d,", i);
    fflush(stdout);
  }
  printf("finished\n");

  printf("List should have 3,2,1 n times, with possible inbetween values\n");
  print_list(ll);
  printf("length, expected %d -> %d\n", numThreads * 3, ll_length(ll));

  while(ll_remove_first(ll)){

  }
  if(!ll_destroy(ll)){
    printf("Failed destroy list\n");
  }
  return 0;
}

int testCase1(const int numThreads){

  srand(time(NULL));
  struct linked_list* ll = ll_create();
  pthread_t threads[numThreads];

  // should be empty now
  printf("\n----===TEST CASE 1===---\n");
  printf("Starts threads that 3 values and remove first 3 values, while calling random ll_length and ll_contains. with random sleeps\n\n");
  printf("List should be empty now: \n");
  print_list(ll);
  printf("\n");

  for(int i = 0; i < numThreads; i++){
    pthread_create(&threads[i], NULL, thread1, ll);
  }

  // may crash, but good to see if it actually is adding anything
  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");

  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");
  
  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");

  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");

  printf("waiting for threads\n");


  printf("Threads ");
  for(int i = 0; i < numThreads; i++){
    pthread_join(threads[i], NULL);
    printf("%d,", i);
    fflush(stdout);
  }
  printf("finished\n");

  // should be empty now
  printf("List should be empty now: \n");
  print_list(ll);
  printf("\n");

  printf("length: %d\n", ll_length(ll));

  if(!ll_destroy(ll)){
    printf("Failed to destroy list\n");
  }

  return 0;
}

int testCase2(const int numThreads){


  srand(time(NULL));
  struct linked_list* ll = ll_create();
  pthread_t threads[numThreads];

  // should be empty now
  printf("\n----===TEST CASE 2===---\n");
  printf("Starts threads that generate 3 values, and add them and remove them, while calling random ll_length and ll_contains. with random sleeps\n\n");
  printf("List should be empty now: \n");
  print_list(ll);
  printf("\n");

  for(int i = 0; i < numThreads; i++){
    pthread_create(&threads[i], NULL, thread2, ll);
  }

  // --== this part may crash, but shows how list changes while threads are running

  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");

  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");
  
  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");

  // sleep(1);

  // printf("idk what would happen: \n");
  // print_list(ll);
  // printf("\n");

  printf("waiting for threads\n");


  printf("Threads ");
  for(int i = 0; i < numThreads; i++){
    pthread_join(threads[i], NULL);
    printf("%d,", i);
    fflush(stdout);
  }
  printf("finished\n");

  // should be empty now
  printf("List should be empty now: \n");
  print_list(ll);
  printf("\n");

  printf("length: %d\n", ll_length(ll));

  if(!ll_destroy(ll)){
    printf("Failed to destroy list\n");
  }

  return 0;
}

#endif