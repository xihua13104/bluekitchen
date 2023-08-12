LR_IROM1 0x08000000 0x440  {    ; load region size_region
  ER_IROM1 0x08000000 0x8{  ; load address = execution address
   startup_flash.o (RESET, +First)
  }
  FLASH 0x8 EMPTY 0x438
  {
  }
}
LR_IROM2 0x08000440 0x80FFBBF  { 
  ER_IROM2 0x08000440 0x80FFBBF{  ; load address = execution address
   * (+RO)
  }
  RW_IRAM1 0x20000000 0x00000400  {  ; stack
   startup_flash.o (+RW +ZI)
  }
  RW_IRAM2 0x20000400 0x0000BC00  {
   * (+RW +ZI)
  }
}
