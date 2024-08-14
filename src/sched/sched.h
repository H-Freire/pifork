#pragma once

#include "../mmu/mmu.h"
#include "../sys/tasks.h"

#define TIMESLICE  200

// PAGE_COUNT - kernel - peripherals (0x3f000000-0xfff00000)
#define USER_SECTIONS                (PAGE_COUNT - 1 - (0xfff - PERIPH_BASE / SECTION))
#define MEM_SECTIONS  ((USER_SECTIONS % 8) ? USER_SECTIONS / 8 + 1 : USER_SECTIONS / 8)

/**
 * @brief Initializes the scheduler and the core
 *        timer
 *
 * @param tiemr_freq Core timer frequency (Hz) configuration
 */
void sched_init(uint32_t timer_freq);

/**
 * @brief Dispatch an available registered task
 */
void schedule(void);

