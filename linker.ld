SECTIONS {
  .text 0x8000: {
    boot.o(.text)
    *(.text)
  }

  .data : {
    . = ALIGN(4);
    __data = .;
    *(.data)
  }

  .bss : {
    . = ALIGN(4);
    __bss_start = .;
    *(.bss*)
    __bss_end = .;
  }

  .heapstack : {
     . = ALIGN(8);
     __heap = .;
     . += 32K;

     __stack_sys = .;
     . += 4K;
     __stack_irq = .;
     . += 4K;
     __stack_user1 = .;
     . += 4K;
     __stack_user2 = .;
   }
}
