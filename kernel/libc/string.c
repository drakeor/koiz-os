#include "string.h"

uint8_t* strcpy(uint8_t* dst, uint8_t* src, uint32_t dst_size)
{
    if(dst == 0)
        return 0;
    
    int i;
    for(i = 0; i < dst_size; i++)
    {
        if(src[i] == '\0')
            break;
        dst[i] = src[i];
    }
    dst[i] = '\0';

    return dst;
}

uint32_t strlen(const uint8_t* str)
{
    uint32_t len = 0;
    while(str[len] != '\0')
        ++len;
    return len;
}

int32_t strcmp(const char* str_a, const char* str_b)
{
    while (*str_a && (*str_a == *str_b)) {
        str_a++;
        str_b++;
    }
    int32_t d = *(const unsigned char*)str_a - *(const unsigned char*)str_b;
    return d;
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

int32_t memcmp(const void* ptr_a, const void* ptr_b, uint32_t data_size)
{
    const uint8_t* a = (const uint8_t*)ptr_a;
    const uint8_t* b = (const uint8_t*)ptr_b;
    uint32_t i = 0;
    for(i = 0; i < data_size; i++) {
        if(a[i] < b[i])
            return -1;
        if(b[i] > a[i])
            return 1;
    }
    return 0;
}

void rpad(uint8_t* str, char pad_char, int length)
{
    int src_length = strlen(str);
    if(src_length >= length)
        return;
    int i;
    for(i = src_length; i < length; i++) {
        str[i] = pad_char;
    }
    str[length-1] = '\0';
}