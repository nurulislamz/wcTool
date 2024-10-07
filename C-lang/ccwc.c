#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>

#define BOM_UTF8 "\xEF\xBB\xBF"      // UTF-8 BOM
#define BOM_UTF16_BE "\xFE\xFF"      // UTF-16 Big Endian BOM
#define BOM_UTF16_LE "\xFF\xFE"      // UTF-16 Little Endian BOM


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

void skipBOM(FILE *fp) {
    char bom[4] = {0};

    // Read first 3 bytes from the file to check for BOM
    fread(bom, 1, 3, fp);

    // Check for UTF-8 BOM
    if (memcmp(bom, BOM_UTF8, 3) == 0) {
        // BOM found, return without rewinding
        return;
    }

    // If not UTF-8 BOM, check for UTF-16 BOMs (first 2 bytes)
    if (memcmp(bom, BOM_UTF16_BE, 2) == 0 || memcmp(bom, BOM_UTF16_LE, 2) == 0) {
        // Handle UTF-16 BOM case (you could implement specific UTF-16 handling if needed)
        fprintf(stderr, "Warning: UTF-16 BOM detected, but not handled in this implementation.\n");
        return;
    }

    // If no BOM found, rewind to the beginning
    fseek(fp, 0, SEEK_SET);
}

long getFileCharacterCount(const char* filename) {
    // Set the locale for multibyte character handling (e.g., UTF-8)
    if (setlocale(LC_ALL, "") == NULL) {
        perror("Error setting locale");
        return -1;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    long charCount = 0;
    int ch;

    // Read the file byte by byte
    while ((ch = fgetc(fp)) != EOF) {
        unsigned char byte = (unsigned char)ch;

        // Detect the length of the current UTF-8 character based on the first byte
        if ((byte & 0x80) == 0) {
            // ASCII character (1 byte)
            charCount++;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte character
            fgetc(fp);  // Skip the next byte
            charCount++;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte character
            fgetc(fp);  // Skip the next 2 bytes
            fgetc(fp);
            charCount++;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4-byte character
            fgetc(fp);  // Skip the next 3 bytes
            fgetc(fp);
            fgetc(fp);
            charCount++;
        } else {
            // If it's an invalid byte, still count it as 1 character
            charCount++;
        }
    }

    fclose(fp);
    return charCount;
}

int main(int argc, char* argv[])
{

  if (!argc == 2 || !argc == 3)
  {
    printf("Error: Expected 2/3 arguments, but got %d\n", argc);
    return -1;
  }

  if (argc == 2){
    if (!doesFileExists(argv[1])){
      return -1;
    }
    char* fileName = argv[1];
    long fileSize = getFileSize(fileName);
    long fileLines = getFileLineCount(fileName);
    long fileWords = getFileWordCount(fileName);
    printf("%ld %ld %ld %s", fileSize, fileLines, fileWords, fileName);
    return 0;
  }


  if (argc == 3){
    if (!doesFileExists(argv[2]))
    {
      return -1;
    }

    if (isOperatorC(argv[1]))
    {
      long fileSize = getFileSize(argv[2]);
      printf("%ld %s", fileSize, argv[2]);
      return 0;
    }

    else if (isOperatorL(argv[1]))
    {
        long fileLines = getFileLineCount(argv[2]);
        printf("%ld %s", fileLines, argv[2]);
        return 0;
    }

    else if (isOperatorW(argv[1]))
    {
        long fileWords = getFileWordCount(argv[2]);
        printf("%ld %s", fileWords, argv[2]);
        return 0;
    }

    else if (isOperatorM(argv[1]))
    {
        long characterCount = getFileCharacterCount(argv[2]);
        printf("%ld %s", characterCount, argv[2]);
        return 0;
    }
  }
  return -1;
}