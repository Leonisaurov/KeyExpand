#include "config.h"
#include "hashmap.h"
#include "string.h"
#include <asm-generic/fcntl.h>
#include <asm-generic/poll.h>
#include <linux/stat.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

Config new_config() {
    struct hashmap_s keys;
    if (hashmap_create(5, &keys) != 0) {
        // TODO: error
    }
    return (Config) {
        .keybinds = keys,
    };
}

void consumeAtDelimiter(int fd, char* del) {
    unsigned int del_len = strlen(del);
    unsigned int del_consume = 0;

    do {
        if (del_consume == del_len) break;

        char ch;
        if (read(fd, &ch, 1) < 1) break;
        if (ch == del[del_consume]) del_consume++;
    } while(1);
}

#define DEBUG

#ifdef DEBUG
#include "debug.h"
#endif

Config read_config(const char* file_path) {
#ifdef DEBUG
    LOG("CONF:"); NL;
#endif
    Config conf = new_config();
    int config_fd = open(file_path, O_RDONLY);
#ifdef DEBUG
    LOG("FD: "); LOGN(config_fd); NL;
#endif
    if (config_fd < 0)
        return conf;

    char ch[1];
    do {
        consumeAtDelimiter(config_fd, "\"");
        String key = readAtDelimiter(config_fd, "\"", false);
        consumeAtDelimiter(config_fd, "\"");
        String bind = readAtDelimiter(config_fd, "\"", false);

        if (hashmap_put(&conf.keybinds, key->content, key->length, bind) != 0) {
            // TODO: unhandle
        }

#ifdef DEBUG
        LOG("KEYBIND: "); LOGS(key); LOG(" -> "); LOGS(bind); NL;
#endif

        freeString(key);
    } while(read(config_fd, ch, 1) > 0);

    close(config_fd);
    return conf;
}

static int freeKeybind(void* const context, void* const value) {
    freeString((String) value);
    return 1;
}

void freeConf(Config conf) {
    if (hashmap_iterate(&conf.keybinds, freeKeybind, NULL) != 0) {
        // TODO: unhandled
    }
    hashmap_destroy(&conf.keybinds);
}
