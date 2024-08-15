#include "sched.h"

#include "../bcm.h"
#include "../mmu/mmu.h"

#define DSB()     asm volatile("dsb \n\t")
#define ISB()     asm volatile("isb \n\t")
#define FREQ(hz)  ((uint32_t)(1e6 / (hz)))

extern uint8_t __stack_usr1;
extern uint8_t __stack_usr2;

extern int user1_main(void);
extern int user2_main(void);
extern void dummy(void);

uint32_t ticks;
uint32_t slice;
uint8_t processes = 2;

uint8_t mem_list[MEM_SECTIONS] = {(uint8_t)((1 << 7) + (1 << 6))};
tcb_t tcb_list[USER_SECTIONS + 1] = {
    {
        .regs  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .sp    = (uintptr_t)&__stack_usr1,
        .lr    = 0,
        .pc    = (uintptr_t)user1_main,
        .cpsr  = 0x10,
        .paddr = (uintptr_t)user1_main,
        .state = READY,
        .parent_id = -1,
    },
    {
        .regs  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .sp    = (uintptr_t)&__stack_usr2,
        .lr    = 0,
        .pc    = (uintptr_t)user2_main,
        .cpsr  = 0x10,
        .paddr = (uintptr_t)user2_main,
        .state = READY,
        .parent_id = -1,
    }
};

pid_t tid;
tcb_t *tcb;

void sched_init(uint32_t timer_freq) {
  tid = 0;
  tcb = &tcb_list[0];
  slice = TIMESLICE;

  tcb_list[USER_SECTIONS].pc = (uintptr_t)dummy;
  tcb_list[USER_SECTIONS].state = READY;

  // 1MHz / FREQ(timer_freq) = timer_freq
  TIMER_REG(load) = FREQ(timer_freq);
  TIMER_REG(control) = __bit(9) | __bit(7) | __bit(5) | __bit(1);

  IRQ_REG(enable_basic) = __bit(0);
}

void schedule(void) {
  pid_t pid = tid,
        pcount = 0;

  // Dummy process whenever no other is available
  if (!processes) {
    tid = USER_SECTIONS;
    tcb = &tcb_list[USER_SECTIONS];
    return;
  }

  do {
    pid = (pid + 1) % USER_SECTIONS;
    pcount++;
  } while (tcb_list[pid].state != READY && pcount < USER_SECTIONS);

  if (pid != tid) {
    tid = pid;
    tcb = &tcb_list[tid];

    // Safe CONTEXTIDR change: unmap previous entry, change ASID, map new section
    // and syncronize both data and instruction (pre-fetching).
    // See the the second method proposed by the ARM documentation
    // [https://developer.arm.com/documentation/ddi0406/b/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Translation-Lookaside-Buffers--TLBs-/TLB-maintenance?lang=en#BABEAHHC]
    map_invalid(tcb->pc);
    DSB();
    asm volatile("mcr p15,0,%0,c13,c0,1 \n\t"
                 :
                 : "r"(pid)
                 : "memory");

    map_section(tcb->pc, tcb->paddr, AP_RW | ASID_SPEC);
    DSB();
    ISB();
  }
  slice = TIMESLICE;
}
