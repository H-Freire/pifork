#pragma once

#include "types.h"

#define wait(status)  waitpid(-1, status)

static inline void __attribute__((always_inline)) yield(void) {
  asm volatile("mov r0, #1 \n\t"
               "swi #0"
               :
               :
               : "r0");
}

static inline pid_t __attribute__((always_inline)) getpid(void) {
  pid_t pid;
  asm volatile("mov r0, #2 \n\t"
               "swi #0     \n\t"
               "mov %0, r0"
               : "=r"(pid)
               :
               : "r0");
  return pid;
}

static inline unsigned __attribute__((always_inline)) getticks(void) {
  unsigned ticks;
  asm volatile("mov r0, #3 \n\t"
               "swi #0     \n\t"
               "mov %0, r0"
               : "=r"(ticks)
               :
               : "r0");
  return ticks;
}

static inline pid_t __attribute__((always_inline)) fork(void) {
  pid_t pid;
  asm volatile("mov r0, #4 \n\t"
               "swi #0     \n\t"
               "mov %0, r0"
               : "=r"(pid)
               :
               : "r0");
  return pid;
}

static inline pid_t __attribute__((always_inline)) waitpid(pid_t pid,
                                                           int *status) {
  pid_t awaited_pid;
  asm volatile("mov r1, %1 \n\t"
               "mov r2, %2 \n\t"
               "mov r0, #5 \n\t"
               "swi #0     \n\t"
               "mov %0, r0"
               : "=r"(awaited_pid)
               : "r"(pid), "r"(status)
               : "r0", "r1", "r2");
  return awaited_pid;
}

static inline void __attribute__((always_inline)) exit(int status) {
  asm volatile("mov r1, %0 \n\t"
               "mov r0, #6 \n\t"
               "swi #0"
               :
               : "r"(status)
               : "r0", "r1");
}
