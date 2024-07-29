#include "bcm.h"
#include "sched.h"

extern void enable_irq(int);
extern void delay(unsigned int);

void sys_main(void) {
  sched_init();
  asm volatile("b task_switch");
}

void user1_main(void) {
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) & (~(0x7 << 21))) | __bit(21);

  while (1) {
    GPIO_REG(gpset[0]) = __bit(17);
    delay(0x300000);
    GPIO_REG(gpclr[0]) = __bit(17);
    delay(0x300000);
  }
}

void user2_main(void) {
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) & (~(0x7 << 24))) | __bit(24);

  while (1) {
    GPIO_REG(gpset[0]) = __bit(18);
    delay(0x300000);
    GPIO_REG(gpclr[0]) = __bit(18);
    delay(0x300000);
  }
}
