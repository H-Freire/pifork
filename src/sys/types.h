#pragma once

// Non-zero if STATUS indicates normal termination
#define WIFEXITED(status)    (((status) & 0x7f) == 0)

// The low-order 8 bits of the status
#define WEXITSTATUS(status)  (((status) & 0xff00) >> 8)

typedef int pid_t;
typedef enum { BLOCKED = -1, EXITED, READY} pstate_t;

