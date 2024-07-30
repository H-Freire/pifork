.text

.extern tcb
.extern sys_main

_start:
  ldr pc, _reset
  ldr pc, _undef
  ldr pc, _swi
  ldr pc, _iabort
  ldr pc, _dabort
  nop
  ldr pc, _irq
  ldr pc, _fiq
_reset:  .word reset
_undef:  .word panic
_swi:    .word swi
_iabort: .word panic
_dabort: .word panic
_irq:    .word irq
_fiq:    .word panic

panic:
   wfi
   b panic

reset:
  // Checks execution privilege EL1 (SVC) or EL2 (HYP)
  mrs r0, cpsr
  and r0, r0, #0x1f
  cmp r0, #0x1a
  bne 0f

  // Exit mode EL2 (HYP)
  mrs r0, cpsr
  bic r0, r0, #0x1f
  orr r0, r0, #0x13
  msr spsr_cxsf, r0
  add lr, pc, #4       // aponta o rótulo 'continua'
  msr ELR_hyp, lr

  // Returns from the exception handling
  // with lower privilege
  eret

0:
  // appoint core 0
  mrc p15, 0, r0, c0, c0, 5
  ands r0, r0, #0x3
  bne hang

  mov r0, #0xd2
  msr cpsr, r0
  ldr sp, =__stack_irq

  mov r0, #0xd3
  msr cpsr, r0
  ldr sp, =__stack_sys

  // interrupt vector copying
  ldr r0, =_start
  mov r1, #0x0000
  ldmia r0!, {r2-r9}
  stmia r1!, {r2-r9}
  ldmia r0!, {r2-r9}
  stmia r1!, {r2-r9}

  // bss clearing
  ldr r0, =__bss_start
  ldr r1, =__bss_end
  mov r2, #0
  cmp r0, r1
  bge sys_main
loop_bss:
  strb r2, [r0], #1
  cmp r0, r1
  blt loop_bss
  b sys_main
hang:
  wfe
  b hang

irq:
  // load current task status
  sub lr, lr, #4
  push {r0}
  ldr r0, =tcb
  ldr r0, [r0]

  // update user registers
  stmib r0, {r1-r14}^
  str lr, [r0, #60]
  mrs r1, spsr
  str r1, [r0, #64]
  pop {r1}
  str r1, [r0]

  // assign a task (same or new) to the current core
  bl irq_handler
  b task_switch

swi:
  // disable interrupts
  cpsid if

  push {r0}
  ldr r0, =tcb
  ldr r0, [r0]

  // update user registers
  stmib r0, {r1-r14}^
  str lr, [r0, #60]
  mrs r1, spsr
  str r1, [r0, #64]
  pop {r1}
  str r1, [r0]

  // syscall param
  mov r0, r1

  // handle call and dispatch a task
  bl swi_handler
  b task_switch

/**
 * @brief Waits for n machine instructions.
 *
 * @param n The number of instructions executed.
 */
.global delay
delay:
  subs r0, r0, #1
  bne delay
  mov pc, lr

/**
 * @brief Toggle system interrupts.
 *
 * @param enable The state selector of interrupts to be set
 *        (0 - disabled, other - enabled).
 */
.global enable_irq
enable_irq:
  movs r0, r0
  mrs r0, cpsr
  orreq r0, r0, #(1 << 7)
  bicne r0, r0, #(1 << 7)
  msr cpsr, r0
  mov pc, lr

/**
 * @brief Swaps the CPU's context with the chosen
 *        user's
 */
.global task_switch
task_switch:
  ldr r0, =tcb
  ldr r0, [r0]

  // load user context
  ldr r1, [r0, #64]
  msr spsr, r1
  ldmib r0, {r1-r14}^
  ldr lr, [r0, #60]
  ldr r0, [r0]

  // switch context
  movs pc, lr

