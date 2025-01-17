/* $Id: $ */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

#define ROWS 2046
#define COLUMNS 2046
#define EPSILON .0001
#define BOUNDARY 2048

static double array[BOUNDARY][BOUNDARY];
static double results[BOUNDARY][BOUNDARY];
static double dif = 0.0002;
static double *diffArray;
static int numHere = 0;
static int numThreads = 0;


struct thd {
  int who;
};

static sem_t wait;
static sem_t here;


int openInput();
void *calcJacobi(void *thdst);
int getNumThreads();
void boundary(struct thd *t);


int main(){

  sem_init(&wait, 1, 0);
  sem_init(&here, 1, 1);
  int result = openInput();
  if(result < 0){
    return 127;
  }
  numThreads = getNumThreads();
  if (numThreads < 1){
    dprintf(2, "Please enter a valid number of threads\n");
    return 127;
  }
  diffArray = malloc(sizeof(double) * numThreads);
  for(int i = 0; i < numThreads; i ++){
    diffArray[i] = dif;
  }
  pthread_t threads[numThreads];
  struct thd thdst[numThreads];

  for (int i = 0; i < numThreads; i++) {
    thdst[i].who = i;
    pthread_create(&threads[i], NULL, &calcJacobi, (void *)&thdst[i]);
  }

  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }
  return 0;
}

int getNumThreads(){
  int numThreads = 0;
  printf("Please enter the number of threads: ");
  scanf("%d", &numThreads);
  return numThreads;
}

int openInput(){
  FILE *input = fopen("/home/clausoa/public/input.mtx", "r");
  if(input == NULL){
    perror("fopen");
    return -1;
  }

  int ix = 0;
  while(ix < BOUNDARY){
    for(int i = 0; i < BOUNDARY; i ++){
      errno = 0;
      int n = fscanf(input, "%lf", &array[ix][i]);
      if(n == 0){
        if(errno != 0){
          perror("fscanf");
          return -1;
        }
      }
    }
    ix ++;
  }
  return 0;
}
void *calcJacobi(void *thisthread){
  struct thd *t = (struct thd *)thisthread;
  int start = 0;
  int end = 0;

  start = 1 + (t->who * COLUMNS)/numThreads;
  end = 1 + (((t->who + 1) * COLUMNS)/numThreads);
  double difference = 0;

  while(1)
  {
    for(int i = start; i < end; i ++)
    {
      for(int j = 1; j <= COLUMNS; j ++)
      {
        results[i][j] = (array[i - 1][j] + array[i + 1][j] +
                        array[i][j - 1] + array[i][j + 1]) * .25;
      }
    }
    difference = 0.0;
    for(int i = start; i < end; i ++){
      for(int j = 1; j <= COLUMNS; j ++){
        if(difference < fabs(results[i][j] - array[i][j])){
          difference = fabs(results[i][j] - array[i][j]);
        }
      }
    }
    diffArray[t->who] = difference;
    boundary(t);
    for(int i = start; i < end; i ++){
      for (int j = 1; j <= COLUMNS; j ++){
        array[i][j] = results[i][j];
      }
    }
    if(diffArray[t->who] < EPSILON){
      sem_wait(&here);
      numThreads --;
      sem_post(&here);
      pthread_exit(NULL);
    }
  }
  return 0;
}

void boundary(struct thd *t){
  sem_wait(&here);
  numHere ++;
  if(numHere < numThreads){
    sem_post(&here);
    sem_wait(&wait);
    return;
  }
  else{
    for(int i = 0; i < numThreads - 1; i ++){
      sem_post(&wait);
    }
  }
  numHere = 0;
  sem_post(&here);
}
