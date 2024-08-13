#include "../src/bcm.h"

#include "../include/pifork.h"

#define DELAY(instr)  for (int i = 0; i < (instr); i++)  \
                        asm volatile("nop");

void __attribute__((section(".user1"))) user1_main(void) {
  int pin;
  pid_t pid = ~0;

  // Setup GPIO 16-19 as output
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) &
                         (~(0x7 << 18 | 0x7 << 21 | 0x7 << 24 | 0x7 << 27))) |
                        __bit(18) | __bit(21) | __bit(24) | __bit(27);

  for (int i = 0; i < 3; i++) {
    if (pid) {
      pin = 16 + i;
      pid = fork();
    }
  }

  if (pid) {
    pin++;
    waitpid(pid, NULL);
  }

  for (int i = 0; i < 8; i++) {
    GPIO_REG(gpset[0]) = __bit(pin);
    DELAY(600000);
    GPIO_REG(gpclr[0]) = __bit(pin);
    DELAY(600000);
  }

  exit(EXIT_SUCCESS);
}

void __attribute__((section(".user2"))) user2_main(void) {
  while (1)
    ;
  exit(EXIT_SUCCESS);
}
