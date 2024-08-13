#include "../src/bcm.h"

#include "../include/pifork.h"

#define DELAY(instr)  for (int i = 0; i < (instr); i++)  \
                        asm volatile("nop");

void __attribute__((section(".user1"))) user1_main(void) {
  // Maps to the first 1 MiB section (kernel)
  int *ptr = (int *)0xC0DE;

  // Setup GPIO 16 as output
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) & ~(0x7 << 18)) | __bit(18);

  for (int i = 0; i < 8; i++) {
    GPIO_REG(gpset[0]) = __bit(16);
    DELAY(2000000);
    GPIO_REG(gpclr[0]) = __bit(16);
    DELAY(2000000);

    if (i == 4) {
      // Invalid data access (privileged region)
      // provokes forced exit
      *ptr = 69;
    }
  }

  exit(EXIT_SUCCESS);
}

void __attribute__((section(".user2"))) user2_main(void) {
  // Setup GPIO 20 as output
  GPIO_REG(gpfsel[2]) = (GPIO_REG(gpfsel[2]) & ~0x7) | __bit(0);

  while (1) {
    GPIO_REG(gpset[0]) = __bit(20);
    DELAY(600000);
    GPIO_REG(gpclr[0]) = __bit(20);
    DELAY(600000);
  }
  exit(EXIT_SUCCESS);
}
