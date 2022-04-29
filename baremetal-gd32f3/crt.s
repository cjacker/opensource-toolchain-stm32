.cpu cortex-m4
.thumb

// end of 48K RAM
.word 0x2000c000
.word _reset
.thumb_func
_reset:
    bl main
    b .
