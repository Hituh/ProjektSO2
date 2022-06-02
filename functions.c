#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



void instruction_manual()
{
  char* filename = "manual.txt";
  FILE* fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("MAN_ERR: could not open file %s", filename);
    return;
  }
  char ch;
  while ((ch = fgetc(fp)) != EOF)
  putchar(ch);
  fclose(fp);
}