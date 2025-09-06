#include "command.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <pty.h>

#include <poll.h>
#include <wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include "hashmap.h"
#include "string.h"

#include <stdbool.h>

#include "config.h"

#define STR(txt) txt, strlen(txt)

// #define DEBUG

#ifdef DEBUG
#include "debug.h"
#endif

typedef enum {
    REPLACED,
    REPLACE_PENDING,
    NO_REPLACE,
} REPLACE_STATE;

void encapsule_cmd(struct termios term, char** cmd) {
    struct winsize win;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &win);

    int master_fd, slave_fd;

    struct termios slave_t = term;
    if (openpty(&master_fd, &slave_fd, NULL, &slave_t, &win) == -1) {
        perror("openpty");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
        return;
    }

    if (pid == 0) {
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDERR_FILENO);

        if(setsid() == -1) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        if (ioctl(slave_fd, TIOCSCTTY, slave_fd) == -1) {
            perror("ioctl TIOCSCTTY");
        }

        close(master_fd);
        close(slave_fd);

        execvp(*cmd, cmd);
        perror("execvp");
        exit(EXIT_FAILURE);
        return;
    }

    close(slave_fd);

    const char* configDir = getenv("XDG_CONFIG_HOME");
    if (configDir == NULL) {
        configDir = getenv("HOME");
        if (configDir == NULL) {
            // TODO:
            configDir = ".";
        }
    }

    String config_path = newString(configDir);
    appendS(config_path, "/.config/keyexpand.conf");
    Config conf = read_config(config_path->content);

    freeString(config_path);

    char buffer[256];
    int timeout = 10000;
    struct pollfd fd[2];

    fd[0].fd = master_fd;
    fd[0].events = POLLIN;

    fd[1].fd = STDIN_FILENO;
    fd[1].events = POLLIN;

    int status;

    Keybind replace = NULL;
    REPLACE_STATE replaceState = REPLACED;

    String input = newStringWithSize(256);

    do {
        int ret = poll(fd, 2, timeout);
        if (ret == -1) break;
        if (ret == 0) continue;

        if (fd[0].revents & POLLIN) {
            int n = read(master_fd, buffer, 255);
            if (n < 1) continue; 
            buffer[n] = '\0';

            write(STDOUT_FILENO, buffer, n);
        }

        if (fd[1].revents & POLLIN) {
            int n = read(STDIN_FILENO, buffer, 255);
            if (n < 1) continue;
            buffer[n] = '\0';
#ifdef DEBUG
            LOG("Key: ");

            LOGr(buffer, n);
            LOG(", ");
            LOGN(*buffer);
            NL;
#endif
            write(master_fd, buffer, n);
            
            for (int i = 0; i < n; i++) {
                char c = buffer[i];
                if (c < 28) {
#ifdef DEBUG
                    if (c == 19) {
                        LOGhashmap(&conf.keybinds);
                    }
#endif
                    replaceState = REPLACED;
                    cleanString(input);
                    continue;
                }
                switch (buffer[i]) {
                    case ' ':
                    case '\n':
                    case '\t':
                        replaceState = REPLACED;
                        cleanString(input);
                        break;
                    case '\b':
                    case 127:
                        if (replaceState == REPLACE_PENDING) {
                            replaceState = NO_REPLACE;
                            for (unsigned int i = 0; i < replace->cursor; i++) {
                                write(master_fd, STR("\x1b[C"));
                            }
                            for (unsigned int i = 1; i < len(replace->bind); i++) {
                                write(master_fd, STR("\b"));
                            }
                            writeS(master_fd, input);
#ifdef DEBUG
                            LOG("RETURNS: ");
                            LOGS(input);
                            NL;
#endif
                            break;
                        } else {
                            replaceState = REPLACED;
                        }
                        if (len(input) != 0) {
                            popS(input);
                        }
                        break;
                    default:
                        if (replaceState == REPLACE_PENDING) {
                            replaceState = REPLACED;
                            copyS(replace->bind, input);
                        } 
                        pushS(input, buffer[i]);
                }
            }

#ifdef DEBUG
            LOG("Buffer: ");
            LOGS(input);
            NL;
#endif
            if (replaceState == REPLACED) {
                replace = hashmap_get(&conf.keybinds, strIO(input));
                if (replace != NULL) {
                    replaceState = REPLACE_PENDING;
                    for (int i = 0; i < len(input); i++) {
                        write(master_fd, STR("\b"));
                    }
                    writeS(master_fd, replace->bind);
                    for (int i = 0; i < replace->cursor; i++) {
                        write(master_fd, STR("\x1b[D"));
                    }
#ifdef DEBUG

                    LOG("REPLACE: "); LOGS(replace->bind); NL;
                    LOG("OFFSET: "); LOGN(replace->cursor); NL;
#endif
                }
            }
        }
    } while((waitpid(pid, &status, WNOHANG)) == 0);

    freeString(input);

    freeConf(conf);
    close(master_fd);
}


