global KiProcessorHalt
global KiPauseProcessor
global KiInterruptEnable
global KiInterruptDisable

KiProcessorHalt:
    hlt
    jmp KiProcessorHalt

KiPauseProcessor:
    pause

KiInterruptDisable:
    cli

KiInterruptEnable:
    sti
