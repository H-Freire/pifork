.text

.global enable_cache
enable_cache:
  mrc p15, 0, r0, c1, c0, 0

  // enables data and instruction caches,
  // as well as branch prediction
  orr r0, r0, #0x1800
  orr r0, r0, #0x0004

  mcr p15, 0, r0, c1, c0, 0
  mov pc, lr

.global disable_mmu
disable_mmu:
  mrc p15, 0, r2, c1, c0, 0
  bic r2, #0x1000
  bic r2, #0x0005
  mcr p15, 0, r2, c1, c0, 0
  dsb
  mov pc, lr

.global enable_mmu
enable_mmu:
  mvn r2, #0
  bic r2, #0xC
  mcr p15, 0, r2, c3, c0, 0
  mcr p15, 0, r0, c2, c0, 0
  mcr p15, 0, r0, c2, c0, 1
  mrc p15, 0, r2, c1, c0, 0
  // orr r2, r2, #0x1000
  orr r2, r2, #0x5
  mcr p15, 0, r2, c1, c0, 0
  mov pc, lr

.global flush_tlb
flush_tlb:
  mov r2, #0
  mcr p15, 0, r2, c8, c7, 0
  mcr p15, 0, r2, c7, c10, 4
  dsb
  mov pc, lr

