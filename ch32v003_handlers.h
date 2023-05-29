/*
 * CH32V003 interrupt handlers
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

#ifndef CH32V003_HANDLERS_H
#define CH32V003_HANDLERS_H

#include "ch32v003_types.h"

void wwdg_handler(void) __attribute__((interrupt));
void pvd_handler(void) __attribute__((interrupt));
void flash_handler(void) __attribute__((interrupt));
void rcc_handler(void) __attribute__((interrupt));
void exti7_0_handler(void) __attribute__((interrupt));
void awu_handler(void) __attribute__((interrupt));
void dma1_ch1_handler(void) __attribute__((interrupt));
void dma1_ch2_handler(void) __attribute__((interrupt));
void dma1_ch3_handler(void) __attribute__((interrupt));
void dma1_ch4_handler(void) __attribute__((interrupt));
void dma1_ch5_handler(void) __attribute__((interrupt));
void dma1_ch6_handler(void) __attribute__((interrupt));
void dma1_ch7_handler(void) __attribute__((interrupt));
void adc1_handler(void) __attribute__((interrupt));
void i2c1_ev_handler(void) __attribute__((interrupt));
void i2c1_er_handler(void) __attribute__((interrupt));
void usart1_handler(void) __attribute__((interrupt));
void spi1_handler(void) __attribute__((interrupt));
void tim1_brk_handler(void) __attribute__((interrupt));
void tim1_up_handler(void) __attribute__((interrupt));
void tim1_trg_com_handler(void) __attribute__((interrupt));
void tim1_cc_handler(void) __attribute__((interrupt));
void tim2_handler(void) __attribute__((interrupt));

#endif
