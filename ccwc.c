#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <locale.h>

// booleanOriented function names
bool isOperatorC(const char* arg){
  if (strcmp(arg, "-c") == 0){
    printf("The argument is -c\n");
    return true;
  }
  else {
    printf("The argument is not -c\n");
    return false;
  }
}

bool isOperatorL(const char* arg){
  if (strcmp(arg, "-l") == 0){
    printf("The argument is -l\n");
    return true;
  }
  else {
    printf("The argument is not -l\n");
    return false;
  }
}

bool isOperatorW(const char* arg){
  if (strcmp(arg, "-w") == 0){
    printf("The argument is -w\n");
    return true;
  }
  else {
    printf("The argument is not -w\n");
    return false;
  }
}

bool isOperatorM(const char* arg){
  if (strcmp(arg, "-m") == 0){
    printf("The argument is -m\n");
    return true;
  }
  else {
    printf("The argument is not -m\n");
    return false;
  }
}

bool doesFileExists(const char* arg){
  if (access(arg, F_OK) == 0){
    printf("File exists\n");
    return true;
  }
  else {
    printf("File Does Not Exist\n");
    return false;
  }
}

FILE* openFile(const char* filename){
  FILE *fp = fopen(filename, "rb"); // opens file

  if (fp == NULL){
    printf("Unable to open file");
  }
  return fp;
}

long getFileSize(char* filename){
  char *buffer = 0;
  long length;

  FILE *fp = openFile(filename);

  if (fp){
    fseek(fp, 0, SEEK_END);  // moves pointer to end of file
    length = ftell(fp);      // each char is c is 1 Bytes, cannot be more (its a standard). So when you seek to end, it gives you the length of the file.
    fclose(fp);
   }
   return length;
}

long getFileLineCount(char* filename){
  long length = 0;
  long count = 0;

  FILE *fp = openFile(filename);

  for (char c = getc(fp); c != EOF; c = getc(fp)){
    if (c == '\n')
      count += 1;
  }

  fclose(fp);
  return count;
}

long getFileWordCount(char* filename){
  long length = 0;
  long count = 0;

  FILE *fp = openFile(filename);

  if (fp){
    fseek(fp, 0, SEEK_SET);

    int c;
    int prev = ' ';

    while ((c = getc(fp)) != EOF)
    {
      if (!isspace(c) && (isspace(prev)))
      {
        count++;
      }
      prev = c;
    }

    if (!isspace(prev)){
      count++;
    }
  }

  fclose(fp);
  return count;
}

long getCharacterCount(char* filename){

  FILE *fp = openFile(filename);

  setlocale(LC_ALL, NULL);

  long count = 0;
  int n;
  char buffer[MB_CUR_MAX];

  while (fread(buffer, 1, 1, fp) == 1){
    n = mblen(buffer, MB_CUR_MAX);

    if (n > 0){
      fread(buffer + 1, 1, n - 1, fp);
      count++;
    } else if (n == 0){
      count++;
    } else {
      perror("Invalid Multibyte character.");
      fclose(fp);
      return -1;
    }
  }
  fclose(fp);
  return count;
}


int main(int argc, char* argv[])
{

  if (argc != 3)
  {
    printf("Error: Expected 2 arguments, but got %d\n", argc - 1);
    return -1;
  }

  if (!doesFileExists(argv[2])){
    return -1;
  }

  if (isOperatorC(argv[1]))
  {
    long fileSize = getFileSize(argv[2]);
    printf("%ld %s\n", fileSize, argv[2]);
  }

  else if (isOperatorL(argv[1]))
  {
      long fileLines = getFileLineCount(argv[2]);
      printf("%ld %s\n", fileLines, argv[2]);
  }

  else if (isOperatorW(argv[1]))
  {
      long fileWords = getFileWordCount(argv[2]);
      printf("%ld %s\n", fileWords, argv[2]);
  }

  else if (isOperatorM(argv[1]))
  {
      long characterCount = getCharacterCount(argv[2]);
      printf("%ld %s\n", characterCount, argv[2]);
  }

  return 0;
}