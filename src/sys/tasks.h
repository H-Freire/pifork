#pragma once

#include <stdint.h>

#include "types.h"

typedef struct {
  uint32_t  regs[13];
  uintptr_t sp;
  uintptr_t lr;
  uintptr_t pc;
  uint32_t  cpsr;

  // Process management
  pid_t     parent_id;
  pstate_t  state;

  // Physical process address
  uintptr_t paddr;
} tcb_t;

extern uint32_t ticks;
extern uint32_t slice;
extern uint8_t processes;

extern tcb_t tcb_list[];
extern uint8_t mem_list[];

extern pid_t tid;
extern tcb_t *tcb;

