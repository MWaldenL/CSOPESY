/*
***************************************************************
Names:    Badulis, Keith Gabriel
          Lua, Matthew Walden
Group:    27
Section:  S15
***************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include "fcfs.c"
#include "nsjf.c"
#include "psjf.c"
#include "rr.c"

int main() {
  FILE* fp;
  printf("Please input a filename (include extension .txt): \n");
  do {
    char filename[101];
    scanf("%s", filename);
    fp = fopen(filename, "r");
    if (fp == NULL) {
      printf("%s not found. Please input a valid filename.\n", filename);
    }
  } while (fp == NULL);

  int x, y, z;
  fscanf(fp, "%d%d%d", &x, &y, &z);
  switch (x) {  
    case 0: fcfs(fp, y), z = 1; break;
    case 1: nsjf(fp, y), z = 1; break;    
    case 2: psjf(fp, y), z = 1; break;    
    case 3: rr(fp, y, z); break;
  }
  return 0;
} 