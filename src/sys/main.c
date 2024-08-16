#include "../bcm.h"
#include "../mmu/mmu.h"
#include "../sched/sched.h"

extern void enable_irq(int);

void sys_main(void) {
  GPIO_REG(gpfsel[4]) = (GPIO_REG(gpfsel[4]) & ~(0x7 << 21)) | __bit(21);

  mmu_flat();
  mmu_init();
  enable_cache();
  sched_init(100);

  asm volatile("b task_switch");
}

void dummy(void) {
  while (1)
    ;
}
