#include "config.h"
#include "hashmap.h"
#include "string.h"
#include <asm-generic/fcntl.h>
#include <asm-generic/poll.h>
#include <linux/stat.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef DEBUG
#include "debug.h"
#endif

Keybind new_keybind(String key, String bind) {
    Keybind keybind = (Keybind) malloc(sizeof(struct keybind));
    keybind->key = key;
    keybind->bind = NULL;

    keybind->cursor = replaceFirst(bind, "$|$", "", &keybind->bind);
    freeString(bind);

    return keybind;
}

void free_keybind(Keybind keybind) {
    freeString(keybind->bind);
    freeString(keybind->key);
    free(keybind);
}

Config new_config() {
    struct hashmap_s keys;
    if (hashmap_create(5, &keys) != 0) {
#ifdef DEBUG
            TODO();
#endif
        // TODO: error
    }
    return (Config) {
        .keybinds = keys,
    };
}

short consumeAtDelimiter(int fd, char* del) {
    unsigned int del_len = strlen(del);
    unsigned int del_consume = 0;

    do {
        if (del_consume == del_len) break;

        char ch;
        if (read(fd, &ch, 1) < 1) return -1;
        if (ch == del[del_consume]) del_consume++;
        else del_consume = 0;
    } while(1);

    return 0;
}

#ifdef DEBUG
#include "debug.h"
#endif

unsigned long getFileSize(int fd) {
    struct stat properties;
    if(fstat(fd, &properties) == -1)
        return 0;
    return properties.st_size;
}

Config read_config(const char* file_path) {
    Config conf = new_config();
    int config_fd = open(file_path, O_RDONLY);
    if (config_fd < 0) {
#ifdef DEBUG
        LOG("NO CONFIG FILE FOUND IN: "); LOG(file_path); NL;
#endif
        return conf;
    }
    unsigned int fileSize = getFileSize(config_fd);
#ifdef DEBUG
    LOG("CONF: "); LOG(file_path); NL;
    LOG("FD: "); LOGN(config_fd); NL;
    LOG("FILESIZE: "); LOGN(fileSize); NL;
#endif

    do {
        if (consumeAtDelimiter(config_fd, "\"") != 0) {
            break;
        }
        String key = readAtDelimiter(config_fd, "\"", false);
        consumeAtDelimiter(config_fd, "\"");
        String bind = readAtDelimiter(config_fd, "\"", false);

        Keybind keybind = new_keybind(key, bind);

#ifdef DEBUG
        LOG("KEYBIND: '"); LOGS(keybind->key); LOG("' -> '"); LOGS(keybind->bind); LOG("'"); NL;
#endif

        if (hashmap_put(&conf.keybinds, keybind->key->content, keybind->key->length, keybind) != 0) {
#ifdef DEBUG
            TODO();
#endif
            // TODO: unhandle
        }

        unsigned int cursorPosition = lseek(config_fd, 0, SEEK_CUR);
#ifdef DEBUG
        LOG("CURSOR: "); LOGN(cursorPosition); NL;
#endif
        if (cursorPosition >= fileSize) break;
    } while(1);

    close(config_fd);
    return conf;
}

static int freeHashmapElement(void* const context, struct hashmap_element_s* const element) {
    free_keybind((Keybind) element->data);
    return -1;
}

void freeConf(Config conf) {
    if (hashmap_iterate_pairs(&conf.keybinds, freeHashmapElement, NULL) != 0) {
        // TODO: unhandled
    }
    hashmap_destroy(&conf.keybinds);
}
