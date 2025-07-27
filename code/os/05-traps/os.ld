/*
 * rvos.ld
 * Linker script for outputting to RVOS
 * */


#include "platform.h"

/*
 * https://sourceware.org/binutils/docs/ld/Miscellaneous-Commands.html
 * OUTPUT_ARCH command specifies a particular ouput machine architecture.
 * "riscv" is the name of the architecture for both 64-bit and 32-bit RISC-V
 * target. We will further refine this by using -march and -mabi when caling gcc.
 * */

OUTPUT_ARCH("riscv")

/*
 * https://sourceware.org/binutils/docs/ld/Entry-Point.html
 * ENTRY command is used to set the "entry point", whitch is the first instruction
 * to execute in a program.
 * The argument of ENTRY command is a symbol name, here is "_start" whitch is defined in
 * start.S
 * */

ENTRY(_start)


/*
 * https://sourceware.org/binutils/docs/ld/MEMORY.html
 * The MEMORY command describes the location and size of blocks of memory in the target.
 * The syntax for MEMORY is:
 * MEMORY
 * {
 *  name [(attr)]: ORIGIN = origin, LENGTH = len
 *  .....
 * }
 * Each line defines a memory region.
 * Each memory region must have a distinct name within the MEMORY command. Here we only define
 * on region named as "ram".
 * The "attr" string is an optional list of attributes that spicify whether to 
 * use a particular memory region for an input section whitch is not explicity
 * mapped in the linker script. Here we assign 'w'(writeable), 'x'(executable), and 'a' (allocatbale).
 * We use '!' to invert 'r' (read-only) and 'i' (initialized).
 * The "ORIGIN" is used to set the start address of the memory region. Here we place it right at
 * the beginning of 0x8000_0000 because this is where the QEMU-virt machine will start executing.
 * Finally LENGTH = 128M tells the linker the we have 128 magebyte of RAM.
 * The linker will double check this to make sure everything can fit.
 * */

MEMORY
{
  ram (wxa!ri): ORIGIN = 0x80000000, LENGTH = LENGTH_RAM
}


/*
 * https://sourceware.org/binutils/docs/ld/SECTIONS.html
 * The SECTIONS command tells the linker how to map input section into  output sections, and how
 * to place the output section in memory.
 * The format of SECTIONS command is:
 * SECTIONS
 * {
 *    section-command
 *    section-command
 *    .....
 * }
 *
 * Each section-command may of be one of the following:
 * (1) an Entry command
 * (2) a symbol assignment
 * (3) an output section description
 * (4) an overlay description
 * We here only demo (2)&(3).
 *
 * We use PROVIDE command to define symbols.
 * https://sourceware.org/binutils/docs/ld/PROVIDE.html
 * The PROVIDE keyword may be used to define a symbol.
 * The syntax is PROVIDE(symbol = expression).
 * Such symbol as "_text_start", "_text_end" ... will be used in mem.s.
 * Notice the period '.' tells the linker to set symbol(e.h. _text_start) to the CURRENT location
 * ('.' = current memory location). This current memory location moves as we add things.
 */
SECTIONS
{
/*
 * We are going to layout all text section in .text output section,
 * startting with .text. The asterisk ("*")  in front of the parentheses means to 
 * march the .text section of ANY object file.
 * */

  .text : {
    PROVIDE(_text_start = .);
    *(.text .text.*)
    PROVIDE(_text_end = .);
  } >ram

  .rodata : {
    PROVIDE(_rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(_rodata_end = .);
  } > ram

  .data : {
    /*
     * . = ALIGN(4096) tells the linker to align the current memory location
     * to 4096 bytes. This will insert padding bytes until current location becomes aligned
     * on 4096-byte boundary.
     * This is because out pagin system's resolution is 4096 bytes.
     * */
     . = ALIGN(4096);
     PROVIDE(_data_start = .);
     /*
      * sdata and data are essentially the same thing. We do not need to distinguish sdata form data.
      * */
     *(.sdata .sdata.*)
     *(.data .data.*)
     PROVIDE(_data_end = .);
  } > ram

  .bss : {
    /*
     * . = ALIGN(4096) tells the linker to align the current memory location
     * to 4096 bytes. This will insert padding bytes until current location becomes aligned
     * on 4096-byte boundary.
     * This is because out pagin system's resolution is 4096 bytes.
     * */
     PROVIDE(_bss_start = .);
     /*
      * https://sourceware.org/binutils/docs/ld/Input-Section-Common.html
      * In most cases, common symbols in put files will placed in the '.bss'
      * section in the output file.
      * */
     *(.sbss .sbss.*)
     *(.bss .bss.*)
     *(COMMON)
     PROVIDE(_bss_end = .);
  } > ram

  PROVIDE(_memory_start = ORIGIN(ram));
  PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

  PROVIDE(_heap_start = _bss_end);
  PROVIDE(_heap_size = _memory_end - _heap_start);
}

