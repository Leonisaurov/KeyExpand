#ifndef KEYEXPAND_CONFIG
#define KEYEXPAND_CONFIG

#include "hashmap.h"
#include "string.h"

typedef struct {
    struct hashmap_s keybinds;
} Config;

typedef struct keybind {
    String key;
    String bind;
    long cursor;
}* Keybind;

Config read_config(const char* file_path);
void freeConf(Config conf);

#endif
