#ifndef KEYEXPAND_DEBUG
#define KEYEXPAND_DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <asm-generic/fcntl.h>

#include "string.h"

#define NL LOG("\n")

void LOGr(const char* txt, unsigned int n);
void LOG(const char* txt);
void LOGS(const String str);
void LOGN(const long long number);

#endif
