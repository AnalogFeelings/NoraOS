global KiProcessorHalt
global KiInterruptEnable
global KiInterruptDisable

KiProcessorHalt:
    hlt
    jmp KiProcessorHalt

KiInterruptDisable:
    cli

KiInterruptEnable:
    sti
