#ifndef KEYEXPAND_STRING
#define KEYEXPAND_STRING

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct string {
    char* content;
    unsigned int size;
    unsigned int length;
}* String;

String newString(const char* content);
String newStringWithSize(const int size);

String readAtDelimiter(int fd, char* del, bool incDel);

String growS(String str, unsigned int grow);
String pushS(String str, char c);

String copyS(String src, String dst);

char popS(String str);

unsigned int len(String str);
void cleanString(String str);
void freeString(String str);

#define writeS(fd, str) write(fd, str->content, str->length)
#define strIO(str) str->content, len(str)

#endif
