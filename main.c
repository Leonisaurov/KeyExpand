#include "terminal.h"
#include "command.h"

#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int flags;

    struct termios term = set_raw_terminal(STDIN_FILENO);
    set_nonblockig_input(STDIN_FILENO, &flags);
    unset_buffering(stdout);

    char* cmd[] = {
        getenv("SHELL"),
        NULL,
    };
    if (argc > 1)
        encapsule_cmd(term, argv + 1);
    else
        encapsule_cmd(term, cmd);

    set_buffering(stdout);
    set_normal_input(STDIN_FILENO, flags);
    set_normal_terminal(&term, STDIN_FILENO);
    return 0;
}
