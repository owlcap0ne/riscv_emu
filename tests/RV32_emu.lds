MEMORY {
    mem : ORIGIN = 0x00000000, LENGTH = 0x00010000
}

SECTIONS {
    .text : { *(.text*) } > mem
    .rodata : { *(.rodata*) } > mem
    .bss : { *(.bss*) } > mem
}
