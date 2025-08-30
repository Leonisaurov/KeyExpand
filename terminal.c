#include "terminal.h"
#include <pty.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <unistd.h>

struct termios set_raw_terminal(int fd) {
    struct termios raw_t, original_t;

    tcgetattr(fd, &original_t);

    raw_t = original_t;
    raw_t.c_lflag &= ~(ICANON | ECHO | ISIG);
    tcsetattr(fd, TCSANOW, &raw_t);

    return original_t;
}

int set_normal_terminal(struct termios *original_t, int fd) {
    return tcsetattr(fd, TCSANOW, original_t);
}

int set_nonblockig_input(int fd, int *flags) {
    *flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    return fcntl(STDIN_FILENO, F_SETFL, *flags | O_NONBLOCK);
}

int set_normal_input(int fd, int flags) {
    return fcntl(STDIN_FILENO, F_SETFL, flags);
}

int unset_buffering(FILE* file) {
    return setvbuf(file, NULL, _IONBF, 0);
}

int set_buffering(FILE* file) {
    return setvbuf(file, NULL, _IOLBF, BUFSIZ);
}
