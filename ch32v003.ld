/*
 * CH32V003 linker script
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

ENTRY(reset_handler)

MEMORY
{
    sram (rwx) : ORIGIN = 0x20000000, LENGTH = 2K
    flash (rx) : ORIGIN = 0x00000000, LENGTH = 16K
}

SECTIONS
{
    .text :
    {
            *(.startup)
            *(.text*)
            *(.rodata*)
            . = ALIGN(4);
    } > flash
    text_section_end = .;

    .data : AT (text_section_end)
    {
            data_section_begin = .;
            data_section_center = . + 0x800;
            *(.data*)
            *(.sdata*)
            . = ALIGN(4);
    } > sram
    data_section_end = .;

    .bss :
    {
            bss_section_begin = .;
            *(.bss*)
            *(.sbss*)
            *(COMMON)
            . = ALIGN(4);
    } > sram
    bss_section_end = .;

    stack_section_end = ORIGIN(sram) + LENGTH(sram);
}
