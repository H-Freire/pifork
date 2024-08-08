.text

/**
 * @brief Copies a section of 1 MiB pointed by src to dest
 *
 * @param src Page source address
 * @param dest Page destination address
 */
.global section_copy
section_copy:
  push {r4-r10}
  // 2^20 bytes <=> 2^18 words <=> 2^13 iterations
  // (4 instructions, 8 registers each)
  mov r2, #(1 << 13)

copy:
  // Loop Unrolling
  ldmia r0!, {r3-r10}
  stmia r1!, {r3-r10}
  ldmia r0!, {r3-r10}
  stmia r1!, {r3-r10}
  ldmia r0!, {r3-r10}
  stmia r1!, {r3-r10}
  ldmia r0!, {r3-r10}
  stmia r1!, {r3-r10}

  subs r2, r2, #1
  bne copy

  pop {r4-r10}
  mov pc, lr

