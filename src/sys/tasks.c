#include "tasks.h"

#include "../bcm.h"
#include "../mmu/mmu.h"
#include "../sched/sched.h"

extern void section_copy(uint32_t src, uint32_t dest);

static inline void _yield(void);
static inline void _getpid(void);
static inline void _getticks(void);
static void _fork(void);
static void _waitpid(pid_t);
static void _exit(int);

void swi_handler(unsigned op) {
  switch (op) {
  case 1:
    _yield();
    break;

  case 2:
    _getpid();
    break;

  case 3:
    _getticks();
    break;

  case 4:
    _fork();
    break;

  case 5:
    _waitpid(tcb->regs[1]);
    break;

  case 6:
    _exit(tcb->regs[1]);
    break;
  }
}

void irq_handler(void) {
  if (bit_is_set(IRQ_REG(pending_basic), 0)) {
    TIMER_REG(ack) = 1;
    ticks += 10;

    if (slice > 10) {
      slice -= 10;
    } else {
      schedule();
    }
  }
}

static inline void _yield(void) { schedule(); }

static inline void _getpid() { tcb->regs[0] = tid; }

static inline void _getticks() { tcb->regs[0] = ticks; }

static void _fork(void) {
  pid_t pid = 0;
  uint32_t mem_addr = 0;

  for (uint32_t i = 0; i < MEM_SECTIONS; i++) {
    // At least one unoccupied page in the registry
    if (mem_list[i] == 255) {
      continue;
    }
    // Find first free bit on the registry
    while (mem_list[i] & (1 << (7 - mem_addr))) {
      mem_addr++;
    }
    mem_addr = (mem_addr + 1 + 8 * i) * SECTION;
    mem_list[i] |= (1 << (7 - mem_addr));
  }

  disable_mmu();
  section_copy(tcb->paddr, mem_addr);

  for (pid_t p = 0; p < USER_SECTIONS; p++) {
    if (tcb_list[p].state == EXITED) {
      pid = p;
      break;
    }
  }
  restart_mmu();

  tcb_list[pid] = *tcb;
  tcb_list[pid].paddr = mem_addr;
  processes++;

  // PID write on parent and child, respectively
  tcb->regs[0] = pid;
  tcb_list[pid].regs[0] = 0;
}

static void _waitpid(pid_t pid) {
  if (pid <= -1) {
    tcb->state = BLOCKED;
  } else {
    // BLOCKED state with specific pid dependency
    if (tcb_list[pid].parent_id != tid) {
      return;
    }
    tcb->state = -(pid + 2);
  }
  schedule();
}

static void _exit(int status) {
  tcb_t *parent = &tcb_list[tcb->parent_id];
  uint8_t mem_index = tcb->paddr / SECTION - 1;

  tcb->state = EXITED;
  if (parent->state == BLOCKED || parent->state == -(tid + 2)) {
    map_section(parent->pc, parent->paddr, AP_PRIV);
    *((int *)(uintptr_t)parent->regs[2]) = status;
    map_invalid(parent->pc);

    parent->state = READY;
  }
  // Free physical memory section
  mem_list[mem_index / 8] &= ~(1 << (7 - (mem_index % 8)));
  processes--;
  schedule();
}
