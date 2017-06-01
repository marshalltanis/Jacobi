/* $Id: $ */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

static double array[2049][2049];
static double results[2049][2049];
static double dif = 0.0002;

struct thd {
  int who;
  int numThreads;
};

int openInput();
void calcJacobi();
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

  pthread_t threads[numThreads];
  struct thd thdst[numThreads];
  int diffArray[numThreads];

  for (int i = 0; i < numThreads; i++) {
    thdst[i].who = i;
    thdst[i].numThreads = numThreads;
    pthread_create(&threads[i], NULL, &calcJacobi, (void *)&thdst[i]);
  }
  
  calcJacobi();
  printf("%f\n", dif);
  for(int i = 0; i < 2048; i ++){
    for (int j = 0; j < 2048; j ++){
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
  while(ix < 2048){
    for(int i = 0; i < 2048; i ++){
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

void calcJacobi(){

  while(1){
    for(int i = 1; i < 2048; i ++){
      for(int j = 1; j < 2048; j ++){
        /* value = (top + bottom + left + right) * .25 */
        results[i][j] = (array[i - 1][j] + array[i + 1][j] + array[i][j - 1] + array[i][j + 1]) * .25;
      }
    }

    dif = 0.0;
    for(int i = 1; i < 2048; i ++){
      for(int j = 1; j < 2048; j ++){
        if(dif < fabs(array[i][j] - results[i][j])){
          dif = fabs(array[i][j] - results[i][j]);
        }
      }
    }
    if(dif < .00005){
      break;
    }
    printf("%f\n", dif);
    for(int i = 1; i < 2048; i ++){
      for (int j = 1; j < 2048; j ++){
        array[i][j] = results[i][j];
      }
    }
  }
}
