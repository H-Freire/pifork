#include "calls.h"

void __attribute__((naked)) yield(void) {
  asm volatile("push {lr}  \n\t"
               "mov r0, #1 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}

pid_t __attribute__((naked)) getpid(void) {
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

pid_t __attribute__((naked)) fork(void) {
  asm volatile("push {lr}  \n\t"
               "mov r0, #4 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}

pid_t __attribute__((naked)) waitpid(pid_t pid, int *status) {
  asm volatile("push {lr}  \n\t"
               "mov r3, r2 \n\t"
               "mov r2, r1 \n\t"
               "mov r1, r0 \n\t"
               "mov r0, #5 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}

void __attribute__((naked)) exit(int status) {
  asm volatile("push {lr}  \n\t"
               "mov r1, r0 \n\t"
               "mov r0, #6 \n\t"
               "swi #0     \n\t"
               "pop {pc}");
}

