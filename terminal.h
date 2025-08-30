#ifndef TERMINAL_INTERCEPTOR_TERM
#define TERMINAL_INTERCEPTOR_TERM

#include <stdio.h>
#include <termios.h>

struct termios set_raw_terminal(int fd);
int set_normal_terminal(struct termios *original_t, int fd);

int set_nonblockig_input(int fd, int *flags);
int set_normal_input(int fd, int flags);

int unset_buffering(FILE* file);
int set_buffering(FILE* file);

#endif
