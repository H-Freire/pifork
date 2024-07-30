#pragma once

#include <stdint.h>

typedef struct {
  uint32_t  regs[13];
  uintptr_t sp;
  uintptr_t lr;
  uintptr_t pc;
  uint32_t  cpsr;
} tcb_t;

void sched_init(void);

void yield(void);
int getpid(void);
unsigned getticks(void);
