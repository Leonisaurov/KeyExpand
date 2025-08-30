#include "debug.h"

int logFile = -1;

void closeLog() {
    if (logFile == -1) {
        return;
    }
    close(logFile);
}

void init() {
    if (logFile != -1) {
        return;
    }
    logFile = open("log.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    atexit(closeLog);
}

void LOGr(const char* txt, unsigned int n) {
    init();
    write(logFile, txt, n);
}

void LOG(const char* txt) {
    LOGr(txt, strlen(txt));
}

void LOGS(const String str) {
    LOGr(str->content, str->length);
}

void LOGN(const long long number) {
    // PENDING:
    char buffer[256];
    LOGr(buffer, snprintf(buffer, 256, "%lld", number));
}
