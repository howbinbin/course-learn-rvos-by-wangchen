#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/*
 * QEMU RISC-V Virt machine with 16550a UART and VirIO MMIO 
 * */


/*
 * maximum number of CPUS
 * see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
 * #define VIRT_CPUS_MAX 8
 */

#define MAXNUM_CPU 8

/*used in os.ld*/
#define LENGTH_RAM 128*1024*1024

/*
 * MemoryMap
 * see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[]
 * 0x00001000 -- boot ROM, provide by qemu
 * 0x02000000 -- CLINT Core Local Interrupt
 * 0x0c000000 -- PLIC  Platform-Level Interrupt Controller
 * 0x10000000 -- UART0
 * 0x10001000 -- virtio disk
 * 0x80000000 -- boot ROM jumps here in machine mode, where we load our kernel
 * */

#define UART0 0x10000000L

#endif /*__PLATFORM_H__*/
