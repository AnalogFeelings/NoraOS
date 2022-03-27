extern HalDispatchInterrupts

%macro HalIsr 1
HalInterrupt%+%1:
    call HalDispatchInterrupts
    iretq
%endmacro
%macro HalIsrErr 1
HalInterrupt%+%1:
    call HalDispatchInterrupts
    iretq
%endmacro

global HalIsrTable
HalIsrTable:
%assign i 0
%rep    32
    dq HalInterrupt%+i ; use DQ instead if targeting 64-bit
%assign i i+1
%endrep

HalIsr 0
HalIsr 1
HalIsr 2
HalIsr 3
HalIsr 4
HalIsr 5
HalIsr 6
HalIsr 7
HalIsrErr    8
HalIsr 9
HalIsrErr    10
HalIsrErr    11
HalIsrErr    12
HalIsrErr    13
HalIsrErr    14
HalIsr 15
HalIsr 16
HalIsrErr    17
HalIsr 18
HalIsr 19
HalIsr 20
HalIsr 21
HalIsr 22
HalIsr 23
HalIsr 24
HalIsr 25
HalIsr 26
HalIsr 27
HalIsr 28
HalIsr 29
HalIsrErr    30
HalIsr 31
