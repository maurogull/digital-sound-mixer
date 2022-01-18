OUTPUT_ARCH(aarch64)


SECTIONS
{
    . = 0x100;
    .text : { KEEP(*(.text.boot)) *(.text ) } 
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) } 
    


    .bss (NOLOAD) : {

        *(.bss .bss.*)
    } 

 
    .noncacheable 0x200000 :  { KEEP(*(.noncacheable.*)) } 


   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}

