#include "string.h"

uint32_t strlen(const char* str)
{
    uint32_t len = 0;
    while(str[len] != '\0')
        ++len;
    return len;
}

void* memcpy(void* dst, void* src, uint32_t data_size)
{
    int i;
    for(i = 0; i < data_size; i++)
        ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
    return dst;
}

void* memset(void* ptr, uint8_t value, uint32_t len)
{
    if(ptr == 0)
        return 0;
    uint8_t* byte_ptr = (uint8_t*)ptr;
    while(len > 0) {
        *byte_ptr = value;
        byte_ptr++;
        len--;
    }
    return ptr;
}