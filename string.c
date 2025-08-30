#include "string.h"
#include <stdbool.h>
#include <string.h>

String newRawString() {
    return (String) malloc(sizeof(struct string));
}

String newString(const char* content) {
    String str = newRawString();
    str->length = strlen(content);
    str->size = str->length;
    str->content = (char*) malloc(sizeof(char) * (str->size + 1));

    memcpy(str->content, content, str->length);
    return str;
}

String newStringWithSize(const int size) {
    String str = newRawString();
    str->size = size;
    str->content = (char*) malloc(sizeof(char) * (str->size + 1));
    memset(str->content, '\0', str->size);

    str->length = 0;
    return str;
}

String readAtDelimiter(int fd, char* del, bool incDel) {
    unsigned int del_len = strlen(del);
    unsigned int del_consume = 0;

    String str = newStringWithSize(50);

    do {
        if (del_consume == del_len) break;

        char ch;
        if (read(fd, &ch, 1) < 1) break;
        if (ch == del[del_consume])
            del_consume++;
        else
            del_consume = 0;
        pushS(str, ch);
    } while (1);
    
    if (incDel)
        return str;

    while (del_consume-- > 0)
        popS(str);

    return str;
}

String endsWith(String str, char* ends) {

}

unsigned int len(String str) {
    return str->length;
}

char popS(String str) {
    if (len(str) == 0)
        return '\0';

    char c = str->content[len(str) - 1];
    str->content[len(str) - 1] = '\0';
    str->length--;

    return c;
}

String growS(String str, unsigned int grow) {
    char* newContent = (char*) realloc(str->content, sizeof(char) * (str->size + grow + 1));
    if (newContent == NULL)
        return NULL;

    str->content = newContent;
    str->size = str->size + grow;

    return str;
}

String pushS(String str, char c) {
    while (str->length + 1 > str->size) {
        if (growS(str, str->size / 2) == NULL)
            return NULL;
    }
    str->content[str->length] = c;
    str->length++;
    return str;
}

String copyS(String src, String dst) {
    if (dst->size < src->length && growS(dst, src->length) == NULL) {
        return NULL;
    }

    memcpy(dst->content, src->content, src->length);
    dst->length = src->length;

    return dst;
}

void cleanString(String str) {
    memset(str->content, '\0', str->length);
    str->length = 0;
}

void freeString(String str) {
    if (str == NULL)
        return;
    if (str->content != NULL)
        free(str->content);
    free(str);
}
