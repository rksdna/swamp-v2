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

#include "ch32v003_threads.h"
#include "ch32v003_handlers.h"
#include "ch32v003_registers.h"

struct context
{
    u32_t x9; // s1 x9 manual
    u32_t x8; // s0 x8 manual
    u32_t x15; // a5 x15 hpe
    u32_t x14; // a4 x14 hpe
    u32_t x13; // a3 x13 hpe
    u32_t x12; // a2 x12 hpe
    u32_t x11; // a1 x11 hpe
    u32_t x10; // a0 x10 hpe
    u32_t x7; // t2 x7 hpe
    u32_t x6; // t1 x6 hpe
    u32_t x5; // t0 x5 hpe
    u32_t x1; // ra x1 hpe
};

struct bind
{
    volatile u32_t* location;
    u32_t mask;
    u32_t value;
};

extern u32_t stack_section_end;
extern u32_t text_section_end;
extern u32_t data_section_begin;
extern u32_t data_section_center;
extern u32_t data_section_end;
extern u32_t bss_section_begin;
extern u32_t bss_section_end;

extern void main(void);

static struct thread main_thread = {0, 0, 0, 0, &main_thread};
static struct thread* active_thread = &main_thread;

void startup_table(void) __attribute__((naked)) __attribute((section(".startup")));
void startup_table(void)
{
    asm volatile(".align 2\n"
                 ".option norvc;\n"
                 "j reset_handler\n"
                 ".word null_handler\n"
                 ".word nmi_handler\n"
                 ".word hard_fault_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word null_handler\n"
                 ".word sys_tick_handler\n"
                 ".word null_handler\n"
                 ".word sw_handler\n"
                 ".word null_handler\n"
                 ".word wwdg_handler\n"
                 ".word pvd_handler\n"
                 ".word flash_handler\n"
                 ".word rcc_handler\n"
                 ".word exti7_0_handler\n"
                 ".word awu_handler\n"
                 ".word dma1_ch1_handler\n"
                 ".word dma1_ch2_handler\n"
                 ".word dma1_ch3_handler\n"
                 ".word dma1_ch4_handler\n"
                 ".word dma1_ch5_handler\n"
                 ".word dma1_ch6_handler\n"
                 ".word dma1_ch7_handler\n"
                 ".word adc1_handler\n"
                 ".word i2c1_ev_handler\n"
                 ".word i2c1_er_handler\n"
                 ".word usart1_handler\n"
                 ".word spi1_handler\n"
                 ".word tim1_brk_handler\n"
                 ".word tim1_up_handler\n"
                 ".word tim1_trg_com_handler\n"
                 ".word tim1_cc_handler\n"
                 ".word tim2_handler\n");
}

void null_handler(void) __attribute__((naked));
void null_handler(void)
{
    asm volatile("1: j 1b\n");
}

__attribute__((naked))
void reset_handler(void)
{
    asm volatile(".option push\n"
                 ".option norelax\n"
                 "la gp, data_section_center\n"
                 ".option pop\n"
                 "la sp, stack_section_end\n"
                 "li a0, 0x80\n"
                 "csrw mstatus, a0\n"
                 "li a3, 0x3\n"
                 "csrw 0x804, a3\n"
                 "la a0, startup_table\n"
                 "or a0, a0, a3\n"
                 "csrw mtvec, a0\n"
                 "la a0, bss_section_begin\n"
                 "la a1, bss_section_end\n"
                 "li a2, 0\n"
                 "bge a0, a1, 2f\n"
                 "1: sw a2, 0(a0)\n"
                 "addi a0, a0, 4\n"
                 "blt a0, a1, 1b\n"
                 "2: la a0, text_section_end\n"
                 "la a1, data_section_begin\n"
                 "la a2, data_section_end\n"
                 "3: beq a1, a2, 4f\n"
                 "lw a3, 0(a0)\n"
                 "sw a3, 0(a1)\n"
                 "addi a0, a0, 4\n"
                 "addi a1, a1, 4\n"
                 "bne a1, a2, 3b\n"
                 "4: lui a4, 0xE000E\n"
                 "lw a5, 256(a4)\n"
                 "lui a3, 0x4\n"
                 "or a5, a5, a3\n"
                 "sw a5, 256(a4)\n"
                 "la a0, main\n"
                 "csrw mepc, a0\n"
                 "la ra, exit_handler\n"
                 "la tp, main_thread\n"
                 "mret\n");
}

void nmi_handler(void) __attribute((weak, alias("null_handler")));
void hard_fault_handler(void) __attribute((weak, alias("null_handler")));
void sys_tick_handler(void) __attribute((weak, alias("null_handler")));

void sw_handler(void) __attribute__((naked));
void sw_handler(void)
{
    asm volatile ("sw x8, 4(sp)\n"
                  "sw x9, 0(sp)\n"
                  "csrr ra, mepc\n"
                  "sw ra, 0(tp)\n"
                  "sw sp, 4(tp)\n"
                  "lw tp, active_thread\n"
                  "lw sp, 4(tp)\n"
                  "lw ra, 0(tp)\n"
                  "csrw mepc, ra\n"
                  "lui a0, 0xE000F\n"
                  "lw a1, (a0)\n"
                  "slli a1, a1, 0x1\n"
                  "srli a1, a1, 0x1\n"
                  "sw a1, (a0)\n"
                  "lw x9, 0(sp)\n"
                  "lw x8, 4(sp)\n"
                  "mret\n");
}

void wwdg_handler(void) __attribute((weak, alias("null_handler")));
void pvd_handler(void) __attribute((weak, alias("null_handler")));
void flash_handler(void) __attribute((weak, alias("null_handler")));
void rcc_handler(void) __attribute((weak, alias("null_handler")));
void exti7_0_handler(void) __attribute((weak, alias("null_handler")));
void awu_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch1_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch2_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch3_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch4_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch5_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch6_handler(void) __attribute((weak, alias("null_handler")));
void dma1_ch7_handler(void) __attribute((weak, alias("null_handler")));
void adc1_handler(void) __attribute((weak, alias("null_handler")));
void i2c1_ev_handler(void) __attribute((weak, alias("null_handler")));
void i2c1_er_handler(void) __attribute((weak, alias("null_handler")));
void usart1_handler(void) __attribute((weak, alias("null_handler")));
void spi1_handler(void) __attribute((weak, alias("null_handler")));
void tim1_brk_handler(void) __attribute((weak, alias("null_handler")));
void tim1_up_handler(void) __attribute((weak, alias("null_handler")));
void tim1_trg_com_handler(void) __attribute((weak, alias("null_handler")));
void tim1_cc_handler(void) __attribute((weak, alias("null_handler")));
void tim2_handler(void) __attribute((weak, alias("null_handler")));

void exit_handler(void) __attribute__((naked));
void exit_handler(void)
{
    if (active_thread->next == active_thread)
        reboot();

    struct thread* thread = active_thread;
    while (thread->next != active_thread)
        thread = thread->next;

    thread->next = active_thread->next;
    yield_thread(0, 0);
}

void start_thread(struct thread* thread, handler_t handler, void* data, void* stack, u32_t size)
{
    struct context* context = (struct context* )(stack + size - sizeof(struct context));
    context->x9 = 0;
    context->x8 = 0;
    context->x15 = 0;
    context->x14 = 0;
    context->x13 = 0;
    context->x12 = 0;
    context->x11 = 0;
    context->x10 = (u32_t)data;
    context->x7 = 0;
    context->x6 = 0;
    context->x5 = 0;
    context->x1 = (u32_t)exit_handler;

    thread->mepc = (void*)handler;
    thread->stack = stack + size - sizeof(struct context);
    thread->condition = 0;
    thread->data = 0;
    thread->next = active_thread->next;
    active_thread->next = thread;
}

void yield_thread(condition_t condition, void* data)
{
    active_thread->condition = condition;
    active_thread->data = data;

    struct thread* thread = active_thread->next;
    while (thread->condition && !thread->condition(thread->data))
        thread = thread->next;

    active_thread = thread;
    STK->CTLR |= STK_CTLR_SWIE;
}

static u32_t bind_condition(struct bind* bind)
{
    return (*bind->location & bind->mask) == bind->value;
}

void wait_for(volatile u32_t* location, u32_t mask, u32_t value)
{
    struct bind bind = {location, mask, value};
    yield_thread((condition_t)bind_condition, &bind);
}

void reboot(void) __attribute__((naked));
void reboot(void)
{
    PFIC->SCTLR = PFIC_SCTRL_SYSRESET;
}
