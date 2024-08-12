#include "mmu.h"

#define TTB_BASE  0x4000

extern void enable_mmu(uintptr_t ttb_addr);

static inline void _map_section(uint32_t, uint32_t, uint32_t);

typedef struct {
  uint32_t descriptor[PAGE_COUNT];
} ttb_l1_t;
#define TTB_L1(X) ((ttb_l1_t *)TTB_BASE)->descriptor[X]

// Not used
typedef struct {
  uint32_t descriptor[256];
} ttb_l2_t;

void map_section(uint32_t vaddr, uint32_t paddr, uint32_t flags) {
  // Never remap the kernel, peripherals or mailboxes (for simplicity)
  if (vaddr < SECTION || vaddr >= PERIPH_BASE) {
    return;
  }

  _map_section(vaddr, paddr, flags);
}

void map_invalid(uint32_t vaddr) {
  int index = (vaddr & SECTION_MASK) >> 20;
  TTB_L1(index) = FAULT_ENTRY;
}

void mmu_flat(void) {
  // Kernel mapping
  _map_section(0, 0, AP_PRIV);
  for (int i = 1; i < PAGE_COUNT; i++) {
    _map_section(i << 20, i << 20, AP_RW);
  }
}

void mmu_init(void) {
  flush_tlb();
  enable_mmu(TTB_BASE);
}

void restart_mmu(void) { enable_mmu(TTB_BASE); }

static inline void _map_section(uint32_t vaddr, uint32_t paddr,
                                uint32_t flags) {
  int index = (vaddr & SECTION_MASK) >> 20;
  int descr = (paddr & SECTION_MASK) | flags | SECTION_ENTRY;
  TTB_L1(index) = descr;
}
