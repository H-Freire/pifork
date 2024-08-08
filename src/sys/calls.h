#pragma once

#include "types.h"

#define wait(status)  waitpid(-1, status)

void yield(void);
pid_t getpid(void);
unsigned getticks(void);
pid_t fork(void);
pid_t waitpid(pid_t, int*);
void exit(int);

