/*
 * CH32V003 timers management
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

#include "ch32v003_timers.h"
#include "ch32v003_threads.h"
#include "ch32v003_handlers.h"
#include "ch32v003_registers.h"

static struct timer* top_timer = 0;

void sys_tick_handler(void) __attribute__((interrupt));
void sys_tick_handler(void)
{
    struct timer* timer = top_timer;
    while (timer)
    {
        if (++timer->ticks >= timer->interval)
        {
            timer->ticks = 0;
            timer->events++;
        }

        timer = timer->next;
    }

    STK->SR = 0;
}

void start_timers_clock(u32_t prescaler)
{
    STK->CTLR = 0;
    STK->CMP = prescaler;
    STK->CNT = 0;
    STK->CTLR = STK_CTLR_STE | STK_CTLR_STRE | STK_CTLR_STIE;

    PFIC->IENR[0] |= (1 << 12);
}

void start_timer(struct timer* timer, u32_t interval)
{
    timer->ticks = 0;
    timer->events = 0;
    timer->interval = interval;
    timer->next = top_timer;
    top_timer = timer;
}

void stop_timer(struct timer* timer)
{
    if (top_timer == timer)
    {
        top_timer = timer->next;
    }
    else
    {
        struct timer* previos_timer = top_timer;
        while (previos_timer->next != timer)
            previos_timer = previos_timer->next;
        previos_timer->next = timer->next;
    }
}

static u32_t timer_condition(struct timer* timer)
{
    return timer->events;
}

void wait_timer(struct timer* timer)
{
    yield_thread((condition_t)timer_condition, timer);
    timer->events = 0;
}

void sleep(u32_t ticks)
{
    struct timer timer;
    start_timer(&timer, ticks);
    wait_timer(&timer);
    stop_timer(&timer);
}

