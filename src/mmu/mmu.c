#include "mmu.h"

#define TTB_BASE  0x4000

typedef struct {
  uint32_t descriptor[PAGE_COUNT];
} ttb_l1_t;
#define TTB_L1(X) ((ttb_l1_t *)TTB_BASE)->X

typedef struct {
  uint32_t descriptor[256];
} ttb_l2_t;

/** @brief Inserts a section entry in the L1 table
 *
 *  @param virt  Virtual address established
 *  @param phy   Corresponding physical address
 *  @param flags Aditional flags
 */
void map_section(uint32_t virt, uint32_t phy, uint32_t flags) {
  int index = (virt & 0xfff00000) >> 20;
  int descr = (phy & 0xfff00000) | 0xc02 | flags;
  TTB_L1(descriptor[index]) = descr;
}

/** @brief Invalidates a L1 section
 *
 *  @param virt Section virtual address
 */
void map_invalid(uint32_t virt) {
  int index = (virt & 0xfff00000) >> 20;
  TTB_L1(descriptor[index]) = 0;
}

/** @brief Maps all physical addresses to corresponding (equal)
 *         virtual ones
 */
void mmu_flat(void) {
  for (int i = 0; i < 4096; i++) {
    map_section(i << 20, i << 20, 0x0000);
  }
}

void mmu_init(void) {
  flush_tlb();
  enable_mmu(TTB_BASE);
}

