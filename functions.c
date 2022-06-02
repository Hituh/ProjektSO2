#include "functions.h"



void instruction_manual()
{
  syslog(LOG_NOTICE, "Displaying manual page...");
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