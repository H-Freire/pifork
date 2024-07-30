#pragma once

#include <stdint.h>

#define PAGE_COUNT 4096

extern void flush_tlb(void);
extern void enable_mmu(uintptr_t);

void mmu_init(void);
void map_flat(void);
void map_section(uint32_t virt, uint32_t phy, uint32_t flags);
void map_invalid(uint32_t virt);
