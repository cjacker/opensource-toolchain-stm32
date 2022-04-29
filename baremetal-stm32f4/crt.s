.cpu cortex-m4
.thumb

// end of 128K RAM
.word 0x20020000
.word _reset
.thumb_func
_reset:
    bl main
    b .
