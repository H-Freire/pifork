#include "sched.h"
#include "bcm.h"
#include <stdint.h>

#define PAGE_COUNT  4096
#define TIMESLICE    200

extern uint8_t __stack_user1;
extern uint8_t __stack_user2;

extern int user1_main(void);
extern int user2_main(void);

void schedule(void);

static uint32_t ticks;
static uint32_t slice;
static uint8_t processes = 2;

tcb_t tcb_list[PAGE_COUNT-1] = {
  {
    .regs = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .sp = (uintptr_t)&__stack_user1,
    .lr = 0,
    .pc = (uintptr_t)user1_main,
    .cpsr = 0x10,
  },
  {
    .regs = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .sp = (uintptr_t)&__stack_user2,
    .lr = 0,
    .pc = (uintptr_t)user2_main,
    .cpsr = 0x10,
  }
};

int tid;
tcb_t *tcb = &tcb_list[0];

void swi_handler(unsigned op) {
  switch (op) {

  // yield
  case 1:
    slice = TIMESLICE;
    schedule();
    break;

  // getpid
  case 2:
    tcb->regs[0] = tid;
    break;

  // getticks
  case 3:
    tcb->regs[0] = ticks;
    break;
  }
}

void schedule(void) {
  tid = (tid + 1) % processes;
  tcb = &tcb_list[tid];
}

void irq_handler(void) {
  if (bit_is_set(IRQ_REG(pending_basic), 0)) {
    TIMER_REG(ack) = 1;
    ticks += 10;
    if (slice > 10) {
      slice -= 10;
    } else {
      slice = TIMESLICE;
      schedule();
    }
  }
}

void sched_init(void) {
  tid = 0;
  tcb = &tcb_list[0];
  slice = TIMESLICE;

  // 1MHz / 1000 = 100 Hz
  TIMER_REG(load) = 10000;
  TIMER_REG(control) = __bit(9) | __bit(7) | __bit(5) | __bit(1);

  IRQ_REG(enable_basic) = __bit(0);
}

void __attribute__((naked)) yield(void) {
  asm volatile("push {lr}  \n\t"
               "mov r0, #1 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}

int __attribute__((naked)) getpid(void) {
  asm volatile("push {lr}  \n\t"
               "mov r0, #2 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}

unsigned __attribute__((naked)) getticks(void) {
  asm volatile("push {lr}  \n\t"
               "mov r0, #3 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}
