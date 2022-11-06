#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdint.h>

typedef struct content content;

char* replace(char* input, uint32_t dest, uint32_t len, char* replacement);
char* replaceMatch(char* input, char* expression, char* replacement);
char* replaceAll(char* input, char* expression, char* replcement);
char* removeAll(char* input, char* expression);
content* findMatch(char* input, char* expression);
content** findAll(char* input, char* expression);
void freeAll(content** dest);
void printMatches(content** dest);
