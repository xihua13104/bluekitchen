armcc -g -O0 -c --thumb --cpu Cortex-M4 printf.c -o printf.o
armcc -g -O0 -c --thumb --cpu Cortex-M4 vsprintf.c -o vsprintf.o
armlink --ro-base 0x20000000 vsprintf.o printf.o -o demo_printf.axf
rm *.o