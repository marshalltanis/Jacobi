/* $Id: $ */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define ROWS 2046
#define COLUMNS 2046
#define EPSILON .00005
#define BOUNDARY 2048

static double array[BOUNDARY][BOUNDARY];
static double results[ROWS][COLUMNS];
static double dif = 0.0002;
static double *diffArray;

struct thd {
  int who;
  int numThreads;
};

int openInput();
void *calcJacobi(void *thdst);
int getNumThreads();

int main(){

  int result = openInput();
  if(result < 0){
    return 127;
  }

  int numThreads = getNumThreads();
  if (numThreads < 1){
    dprintf(2, "Please enter a valid number of threads\n");
    return 127;
  }
  diffArray = malloc(sizeof(double) * numThreads);

  pthread_t threads[numThreads];
  struct thd thdst[numThreads];

  for (int i = 0; i < numThreads; i++) {
    thdst[i].who = i;
    thdst[i].numThreads = numThreads;
    pthread_create(&threads[i], NULL, &calcJacobi, (void *)&thdst[i]);

  }
  printf("%f\n", dif);
  for(int i = 0; i < ROWS; i ++){
    for (int j = 0; j < COLUMNS; j ++){
      printf("%lf ", array[i][j]);
    }
  }
  printf("\n");

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
  int converged = 0;
  struct thd *t = (struct thd *)thisthread;
  int start = 0;
  int end = 0;

  start = 1 + ((t->who * COLUMNS)/t->numThreads);
  end = 1 + (((t->who + 1) * COLUMNS)/t->numThreads);

  while(!converged)
  {
    for(int i = 1; i <= ROWS; i ++)
    {
      for(int j = start; j <= end; j ++)
      {
        results[i][j] = (array[i - 1][j] + array[i + 1][j] +
                        array[i][j - 1] + array[i][j + 1]) * .25;
      }
    }

    dif = 0.0;
    for(int i = 1; i < ROWS; i ++){
      for(int j = start; j <= end; j ++){
        if(dif < fabs(array[i][j] - results[i][j])){
          dif = fabs(array[i][j] - results[i][j]);
        }
      }
    }
    int numConverged = 0;
    for (int i = 0; i < t->numThreads; i++) {
      if (diffArray[i] <= EPSILON) {
        numConverged = numConverged + 1;
      }
    }
    if (numConverged == t->numThreads)
      converged = !converged;

    //printf("%f\n", dif);
    //syncronize
    for(int i = 1; i < ROWS; i ++){
      for (int j = 1; j < COLUMNS; j ++){
        array[i][j] = results[i][j];
      }
    }
  }
  return 0;
}
