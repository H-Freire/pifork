#include "../sched/sched.h"
#include "../mmu/mmu.h"

extern void enable_irq(int);

void sys_main(void) {
  mmu_flat();
  mmu_init();
  sched_init(100);

  asm volatile("b task_switch");
}
