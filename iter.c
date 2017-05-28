/* $Id: $ */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static double array[2048][2048];
//static double results[2048][2048];
int openInput();
int main(){
  int result = openInput();
  if(result < 0){
    return 127;
  }
  return 0;
}

int openInput(){
  char path[200];
  printf("%s: ", "Please enter the input file name");
  if(fgets(path, 200, stdin) == NULL){
    perror("fgets");
    return -1;
  }
  FILE *input = fopen(path, "r");
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
