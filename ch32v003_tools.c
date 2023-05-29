/*
 * CH32V003 general purpose routines and definitions
 * Copyright (c) 2023 rksdna
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ch32v003_tools.h"

#define COPY(dst, src, type) *(type*)dst = *(const type*)src, dst += sizeof(type), src += sizeof(type)
#define FILL(dst, src, type) *(type*)dst = src, dst += sizeof(type)

static s32_t convert(char* buffer, u32_t shift, u32_t base, u32_t sign, s32_t value)
{
    static const char digits[] = "0123456789ABCDEF";
    char* ptr = buffer;
    u32_t remainder = sign && value < 0 ? -value : value;
    u32_t position = 0;
    while (remainder || position <= shift)
    {
        *ptr++ = digits[remainder % base], remainder = remainder / base;
        if (position == shift - 1)
            *ptr++ = '.';

        position++;
    }

    if (sign && value < 0)
        *ptr++ = '-';

    return ptr - buffer;
}

static void put_integer(put_t put, u32_t size, u32_t shift, u32_t base, u32_t sign, s32_t value)
{
    char buffer[12];
    const char pad = sign ? ' ' : '0';
    const u32_t count = convert(buffer, shift, base, sign, value);

    if (size == 0)
        size = count;

    while (size--)
        put(size >= count ? pad : buffer[size]);
}

static void put_string(put_t put, u32_t size, const char* data)
{
    if (!size)
    {
        while (*data)
            put(*data++);
    }
    else
    {
        while (*data && size)
            size--, put(*data++);

        while (size--)
            put(' ');
    }
}

static void put_char(put_t put, u32_t size, char value)
{
    if (!size)
        size = 1;

    while (size--)
        put(value);
}

static void put_memory(put_t put, u32_t size, const u8_t* data)
{
    if (!size)
        size = 1;

    while (size--)
        put_integer(put, 2, 0, 16, 0, *data++);
}

void explicit_print(put_t put, const char* format, const void* args)
{
    while (*format)
    {
        char token = *format++;
        if (token == '%')
        {
            u32_t size = 0;
            u32_t shift = 0;
            while (*format)
            {
                token = *format++;
                if (token >= '0' && token <= '9')
                {
                    size = 10 * size + token - '0';
                    continue;
                }

                if (token == '*')
                {
                    size = *(u32_t*)args, args += sizeof(u32_t);
                    continue;
                }

                if (token == '.')
                {
                    shift = size;
                    size = 0;
                    continue;
                }

                if (token == 'c')
                {
                    put_char(put, size, *(u32_t*)args);
                    args += sizeof(u32_t);
                    break;
                }

                if (token == 'x')
                {
                    put_integer(put, size, shift, 16, 0, *(u32_t*)args);
                    args += sizeof(u32_t);
                    break;
                }

                if (token == 'd')
                {
                    put_integer(put, size, shift, 10, 1, *(s32_t*)args);
                    args += sizeof(s32_t);
                    break;
                }

                if (token == 'u')
                {
                    put_integer(put, size, shift, 10, 0, *(u32_t*)args);
                    args += sizeof(u32_t);
                    break;
                }

                if (token == 'm')
                {
                    put_memory(put, size, *(const u8_t**)args);
                    args += sizeof(const u8_t*);
                    break;
                }

                if (token == 's')
                {
                    put_string(put, size, *(const char**)args);
                    args += sizeof(const char*);
                    break;
                }

                put(token);
                break;
            }
        }
        else
        {
            put(token);
        }
    }
}

__attribute__((noinline))
void print(put_t put, const char* format, ...)
{
    explicit_print(put, format, &format + sizeof(const char*));
}

void copy(void* destination, const void* source, u32_t size)
{
    if (size > 4 * sizeof(u32_t))
    {
        u32_t count = (u32_t)destination % sizeof(u32_t);
        size -= count;
        while (count--)
            COPY(destination, source, u8_t);

        switch ((u32_t)source % sizeof(u32_t))
        {
        case 0:
            count = size / sizeof(u32_t);
            size %= sizeof(u32_t);
            while (count--)
                COPY(destination, source, u32_t);
            break;

        case 2:
            count = size / sizeof(u16_t);
            size %= sizeof(u16_t);
            while (count--)
                COPY(destination, source, u16_t);
            break;

        default:
            break;
        }
    }

    while (size--)
        COPY(destination, source, u8_t);
}

void* memcpy(void* destination, const void* source, u32_t size)
{
    copy(destination, source, size);
    return destination;
}

void fill(void* destination, u8_t value, u32_t size)
{
    if (size > 2 * sizeof(u32_t))
    {
        const u32_t pattern = value | (value << 8) | (value << 16) | (value << 24);

        u32_t count = (u32_t)destination % sizeof(u32_t);
        size -= count;
        while (count--)
            FILL(destination, pattern, u8_t);

        count = size / sizeof(u32_t);
        size %= sizeof(u32_t);
        while (count--)
            FILL(destination, pattern, u32_t);
    }

    while (size--)
        FILL(destination, value, u8_t);
}

void* memset(void* destination, int value,  u32_t size)
{
    fill(destination, value, size);
    return destination;
}

