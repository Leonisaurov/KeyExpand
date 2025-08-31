#include "debug.h"
#include "config.h"

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

void LOGp(const void* pointer) {
    char buffer[256];
    LOGr(buffer, snprintf(buffer, 256, "%p", pointer));
}

int hashmap_iterator(void* const context, void* const element) {
    Keybind keybind = element;
    LOG("\t'"); LOGS(keybind->key); LOG("': '"); LOGS(keybind->bind); LOG("'; "); LOGN(keybind->cursor); LOG(","); NL;
    return 1;
}

void LOGhashmap(struct hashmap_s *hashmap) {
    LOG("{"); NL;
    hashmap_iterate(hashmap, hashmap_iterator, 0);
    LOG("}"); NL;
}

void TODO() {
    LOG("Not handled yet!");
}
