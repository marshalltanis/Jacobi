/* Original Author: Aran Clauson
 * Modified by : Marshall Tanis
 * Assignment 1, question 2.
 * Threaded Matrix Multiplication
 */




#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "rdtsc.h"

bool verify(result_matrix_t result);
void *threadWork(void *args);
static result_matrix_t result;

/* thread argument structure */
typedef struct argStr{
  int who;
  int numThreads;
}argStr_t;




/*  This program times the multiplication of a 4x4 matrix, LEFT, by a 4x1000
 *  matrix, RIGHT.  The result is, of course, a 4x1000 matrix.  If the answer is
 *  correct, then the time is reported.
 */

int main(int argcp, char *argv[])
{
  /* no argument specified, 1 thread */
  int numThreads = 1;
  void *unused;
  /* determine number of threads */
  if(argv[1] != '\0'){
    numThreads = atoi(argv[1]);
  }
  /* create thread arrays */
  pthread_t threads[numThreads];
  argStr_t threadArgs[numThreads];
  int i, j, k;

  uint64_t start = rdtsc();
  /* create thread, assign arguments (who and number of threads), and perform thread work */
  for(i = 0; i < numThreads; i ++){
    threadArgs[i].who = i;
    threadArgs[i].numThreads = numThreads;
    if((k = pthread_create(&threads[i], NULL, &threadWork, (void *)&threadArgs[i]))!= 0){
      perror("pthread_create");
      return -1;
    }
  }
  /* wait for all threads to finish */
  for(i = 0; i < numThreads; i ++){
    pthread_join(threads[i], &unused);
  }
  uint64_t end  = rdtsc();

  if(verify(result)) {
    printf("time = %zu\n", end - start);
    printf("Good job!\n");
  }

  return EXIT_SUCCESS;
}

/* work to be performed by each thread. Determines how much of the 1000 columns
 * each thread calculates */
void *threadWork(void *arg){
  /* creates new arg field for each thread. */
  argStr_t *args = (argStr_t *)arg;
  int start =0;
  int end =0;
  /* if evenly divided by 1000, assign start and end values for columns to be
   * calculated by each thread
   */
  if(1000 % args->numThreads == 0){
    start = (args->who) * 1000/args->numThreads;
    end = start + 1000/args->numThreads - 1;
  }
  /* adds 1 to the end argument in order to account for unevenly divided number
   * of threads
   */
  else{
    start = (args->who) * 1000/args->numThreads;
    end = start + (1000/args->numThreads + 1) - 1;
  }
  /* perform calculation of section. rows and columns were switched in order
   * to accomodate more threads
   */
  for(int j = start; j <= end; j ++){
    for(int i = 0; i < 4; i ++){
      result[i][j] = 0;
      for(int k = 0; k < 4; k ++){
        result[i][j] = result[i][j] + (LEFT[i][k]) * (RIGHT[k][j]);
      }
    }
  }

}

/* Verify that the result matrix is the correct answer.
 *
 */
bool verify(result_matrix_t result) {
  bool error = false;
  int i, j;
  for(i = 0; !error && i < 4; ++i)
  {
    for(j = 0; !error && j < 1000; ++j)
    {
      if(ANSWER[i][j] != result[i][j]) {
        fprintf(stderr, "error at [%d,%d]\n", i, j);
        error = true;
      }
    }
  }
  return !error;
}
