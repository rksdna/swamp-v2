/*
 * CH32V003 sample
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
#include "ch32v003_timers.h"
#include "ch32v003_threads.h"
#include "ch32v003_handlers.h"
#include "ch32v003_registers.h"

static void usart(char value)
{
    while( !(USART1->STATR & USART_STATR_TC))
        continue;

    USART1->DATAR = value;
}

struct thread thread1;
u8_t stack1[256];

void task1(u32_t data)
{
    struct timer tmr;
    start_timer(&tmr, 325);

    while (data--)
    {
        GPIOD->OUTDR ^= GPIO_OUTDR_ODR4;
        wait_timer(&tmr);
    }

    stop_timer(&tmr);
}

struct thread thread2;
u8_t stack2[256];

void task2(u32_t data)
{
    struct timer tmr;
    start_timer(&tmr, 273);

    while (data--)
    {
        GPIOD->OUTDR ^= GPIO_OUTDR_ODR6;
        wait_timer(&tmr);
    }

    stop_timer(&tmr);
}

void main(void)
{
    u32_t data = 5;
    struct timer tmr;

    RCC->APB2PCENR |= RCC_APB2PCENR_IOPDEN | RCC_APB2PCENR_USART1EN;

    USART1->CTLR1 = USART_CTLR1_TE;
    USART1->CTLR2 = 0;
    USART1->CTLR3 = 0;
    USART1->BRR = 8000000 / 115200;
    USART1->CTLR1 |= USART_CTLR1_UE;

    GPIOD->CFGLR = GPIO_CFGLR_GPO_PP(0) |
                   GPIO_CFGLR_GPI_FL(1) |
                   GPIO_CFGLR_GPO_PP(4) |
                   GPIO_CFGLR_AFO_PP(5) |
                   GPIO_CFGLR_GPO_PP(6);

    start_timers_clock(1000);

    start_thread(&thread2, (handler_t)task2, (void*)21, stack2, sizeof(stack2));
    start_thread(&thread1, (handler_t)task1, (void*)11, stack1, sizeof(stack1));

    print(usart, "start %d\n", data);

    start_timer(&tmr, 500);
    while(data--)
    {
        print(usart, "ping %d\n", data);
        wait_timer(&tmr);
    }

    stop_timer(&tmr);
}
