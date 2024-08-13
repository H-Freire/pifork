#include <pifork.h>

#define DELAY(instr)  for (int i = 0; i < (instr); i++)  \
                        asm volatile("nop");

void __attribute__((section(".user1"))) user1_main(void) {
  int stat;
  pid_t pid, pid_awaited;

  // Setup GPIO 16 and 19 as output
  GPIO_REG(gpfsel[1]) = (GPIO_REG(gpfsel[1]) & (~(0x7 << 18 | 0x7 << 27)))
                        | __bit(18)
                        | __bit(27);

  pid = fork();
  if (!pid) {
    for (int i = 0; i < 12; i++) {
      GPIO_REG(gpset[0]) = __bit(19);
      DELAY(2000000);
      GPIO_REG(gpclr[0]) = __bit(19);
      DELAY(2000000);
    }
  } else {
    for (int i = 0; i < 8; i++) {
      GPIO_REG(gpset[0]) = __bit(16);
      DELAY(1000000);
      GPIO_REG(gpclr[0]) = __bit(16);
      DELAY(1000000);
    }
    pid_awaited = wait(&stat);

    if (WIFEXITED(stat) && !WEXITSTATUS(stat) && pid_awaited == pid) {
      for (int i = 0; i < pid_awaited; i++) {
        GPIO_REG(gpset[0]) = __bit(16);
        DELAY(500000);
        GPIO_REG(gpclr[0]) = __bit(16);
        DELAY(500000);
      }
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
