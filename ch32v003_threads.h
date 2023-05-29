/*
 * CH32V003 threads management and startup code
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

#ifndef CH32V003_THREADS_H
#define CH32V003_THREADS_H

#include "ch32v003_types.h"

typedef void (* handler_t)(void* data);
typedef u32_t (* condition_t)(void* data);

struct thread
{
    void* mepc;
    void* stack;
    condition_t condition;
    void* data;
    struct thread* next;
};

void start_thread(struct thread* thread, handler_t handler, void* data, void* stack, u32_t size);
void yield_thread(condition_t condition, void* data);

void wait_for(volatile u32_t* location, u32_t mask, u32_t value);

void reboot(void);

#endif
