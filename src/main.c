#include "bcm.h"
#include "sched/sched.h"
#include "mmu/mmu.h"

#define DELAY(instr)  for (int i = 0; i < instr; i++) \
                        asm volatile("nop");

// .global delay
// delay:
//   subs r0, r0, #1
//   bne delay
//   mov pc, lr

extern void enable_irq(int);

void sys_main(void) {
  // Setup GPIO 16 and 19 as output
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) & (~(0x7 << 18 | 0x7 << 27)))
                        | __bit(18)
                        | __bit(27);
  sched_init(100);
  mmu_init();
  mmu_flat();

  asm volatile("b task_switch");
}

void __attribute__((section(".user1"))) user1_main(void) {
  while (1) {
    GPIO_REG(gpset[0]) = __bit(16);
    DELAY(0x300000);
    GPIO_REG(gpclr[0]) = __bit(16);
    DELAY(0x300000);
  }
}

void __attribute__((section(".user2"))) user2_main(void) {
  while (1) {
    GPIO_REG(gpset[0]) = __bit(19);
    DELAY(0x300000);
    GPIO_REG(gpclr[0]) = __bit(19);
    DELAY(0x300000);
  }
}
