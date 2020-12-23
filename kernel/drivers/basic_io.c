#include "basic_io.h"

uint8_t io_byte_in(uint16_t port) {
    /* 
     * Original Reference: http://www.mathemainzel.info/files/x86asmref.html#in 
     * Note reversed syntax to code
     * IN DX, AL 
     * IN PORT, ACCUM
     * Reads a byte from "port" and places the result in AL.
     */
    uint8_t result;
    __asm__(
        "in %%dx, %%al"
        : "=a" (result) // put AL register in result
        : "d" (port)    // load [R,E]DX with port
    );
    return result;
}

uint16_t io_word_in(uint16_t port) {
    // This is just the word variant of above
    uint16_t result;
    __asm__(
        "in %%dx, %%ax"
        : "=a" (result) // put AX register in result
        : "d" (port)    // load [R,E]DX with port
    );
    return result;
}


void io_byte_out(uint16_t port, uint8_t data) {
    /*
     * Original Reference: http://www.mathemainzel.info/files/x86asmref.html#out
     * Note reversed syntax to code
     * OUT AL, DX
     * OUT ACCUM, PORT
     * Transfers byte in AL to specified port.
     */
    __asm__(
        "out %%al, %%dx"
        : // No input reg
        : "a" (data), // load [R,E]ax with data
          "d" (port) // load [R,E]dx with port
    );
}

void io_word_out(uint16_t port, uint16_t data) {
    // This is just the word variant of above
    __asm__(
        "out %%ax, %%dx"
        : // No input reg
        : "a" (data), // load [R,E]ax with data
          "d" (port) // load [R,E]dx with port
    );
}