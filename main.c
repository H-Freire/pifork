#include "bcm.h"
#include "sched.h"

extern void enable_irq(int);
extern void delay(unsigned int);

void sys_main(void) {
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) & (~(0x7 << 18 | 0x7 << 27))) |
                        __bit(18) | __bit(27);
  sched_init();
  asm volatile("b task_switch");
}

void user1_main(void) {
  while (1) {
    GPIO_REG(gpset[0]) = __bit(16);
    delay(500000);
    GPIO_REG(gpclr[0]) = __bit(16);
    delay(500000);
  }
}

void user2_main(void) {
  while (1) {
    GPIO_REG(gpset[0]) = __bit(19);
    delay(500000);
    GPIO_REG(gpclr[0]) = __bit(19);
    delay(500000);
  }
}
