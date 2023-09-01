armcc -g -O0 -c --cpu Cortex-M4 sieve.c -o sieve.o
armasm -g --cpu Cortex-M4 startup_flash.s -o startup_flash.o
armasm -g --cpu Cortex-M4 startup_sram.s -o startup_sram.o
armlink --scatter=scatter_sram.ld sieve.o startup_sram.o -o demo_sram.axf
armlink --scatter=scatter_flash.ld startup_flash.o sieve.o -o demo_flash.axf
del *.o
