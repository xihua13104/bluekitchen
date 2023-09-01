LR_IROM1 0x20000000 0x1000  {    ; load code region size_region
  ER_IROM1 0x20000000 0x1000{  ; load address = execution address
   startup_sram.o (INIT, +First)
   * (+RO)
  }
}
LR_IRAM1 0x2000B000 0x000001000  {   ; stack region
  
  RW_IRAM1 0x2000B000 0x00000400  {  ; stack
   startup_sram.o (+RW +ZI)
  }
  RW_IRAM2 0x2000B400 0x00000C00  {
   * (+RW +ZI)
  }
}
