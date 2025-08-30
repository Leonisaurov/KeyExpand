#ifndef TERMINAL_INTERCEP_COMMAND
#define TERMINAL_INTERCEP_COMMAND

#include <termios.h>

void encapsule_cmd(struct termios term, char** cmd);

#endif
