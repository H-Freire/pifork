SECTIONS {
  .text 0x8000 : {
    *boot.o(.text)
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
    . += 16K;

    __stack_svc = .;
    . += 4K;
    __stack_irq = .;
  }

  . = ALIGN(1M);
  .user1 : {
    *(.user1)
    . += 4K;
    __stack_usr1 = .;
  }

  . = ALIGN(1M);
  .user2 : {
    *(.user2)
    . += 4K;
    __stack_usr2 = .;
  }
}

