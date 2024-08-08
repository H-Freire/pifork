#pragma once

/**
 * Translation Table format and configuration, based on the ARMv7-A documentation.
 * Refer to [https://developer.arm.com/documentation/ddi0406/b/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Translation-tables/Translation-table-entry-formats]
 */

#include <stdint.h>

#include "../bcm.h"

#define PAGE_COUNT             4096
#define SECTION            0x100000
#define SECTION_MASK     0xfff00000

// TTB entry type
#define FAULT_ENTRY             0x0
#define COARSE_ENTRY            0x1
#define SECTION_ENTRY           0x2
#define FINE_ENTRY              0x3

// Access Permission (read-only privileged configurations excluded)
#define AP_PRIV        (0b01 << 10)
#define AP_RO          (0b01 << 10)
#define AP_RW          (0b11 << 10)

// Defines if the entry is process specific (ASID).
// See [https://developer.arm.com/documentation/ddi0406/b/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Translation-Lookaside-Buffers--TLBs-/Global-and-non-global-regions-in-the-virtual-memory-map?lang=en]
#define ASID_SPEC         __bit(17)

// per section caching and buffering
#define CACHE_WT                0x8
#define CACHE_WB                0xb

/**
 * @brief Disables the MMU.
 */
extern void disable_mmu(void);

/**
 * @brief Invalidates the current TLB state.
 */
extern void flush_tlb(void);

/**
 * @brief Enables the system caches.
 */
extern void enable_cache(void);

/**
 * @brief Clear TLB and enables the MMU
 */
void mmu_init(void);

/**
 * @brief Enables the MMU with its former TTB register.
 */
void restart_mmu(void);

/**
 * @brief Maps all physical addresses to corresponding (equal)
 *        virtual ones
 */
void mmu_flat(void);

/**
 * @brief Inserts a section entry in the L1 table
 *
 * @param vaddr Virtual address established
 * @param paddr Corresponding physical address
 * @param flags Aditional flags
 */
void map_section(uint32_t vaddr, uint32_t paddr, uint32_t flags);

/**
 * @brief Invalidates a L1 section
 *
 * @param vaddr Section virtual address
 */
void map_invalid(uint32_t vaddr);

