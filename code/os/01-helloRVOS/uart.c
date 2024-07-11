#include "types.h"
#include "platform.h"


/*
 *The UART control registers are memary-mampped at address UART0.
 *This macro return the address of one of the registers.
 * */

#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))
/*
 *Reference
 *[1]: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html
 *
 * */

/*
 *UART control registers map. see [1] "PROGRRAMMING TABLE"
 *note some are reused by multiple functions
 *0(write mode): THR/DLL
 *1(write mode): THR/DLL
 * */

#define RHR 0 // Receive Holding Registers (read mode)
#define THR 0 // Transmit Holding Registers (write mode)
#define DLL 0 // LSB of Divisor Latch (write mode)
#define IER 1 // Interrupt Enable Registers(write mode)
#define DLM 1 // MSB of Divisor Latch (write mode)
#define FCR 2 // FIFO control Registers (write mode)
#define ISR 2 // Interrupt Control Registers (read mode)
#define LCR 3 // Line Control Registers
#define MCR 4 // Modem Control Registers
#define LSR 5 // Line Status Registers
#define MSR 6 // Modem Status Registers
#define SPR 7 // ScratchPad Registers

/*
 * POWER UP DEFAULTS
 * IER = 0 : TX/RX holding register interrupts are both disabled
 * ISR = 1 : no Interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0 BITS 4-6 = inputs
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTS = High
 * RXRDY = High
 * TXRDY = Low
 * INT = LOW
 * */

/*
 * LINE STATUS Registers (LSR)
 * LSR BIT 0:
 * 0 = no data in receive holding registers or FIFO
 * 1 = data has been receive and saved in the receive holding registers or FIFO
 * .....
 * LSR BIT5:
 * 0 = transmit holding register is full. 16500 will note accept any data for transmission
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * ......
 * */

#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE (1 << 5)

#define uart_read_reg(reg)  (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void uart_init()
{
  /* disable interrupts. */
  uart_write_reg(IER, 0x00);
  /*
   * Setting baud rate. Just a demo here if we care about the divisor
   * but for our purpose [QEMU-virt], this doesn't really do anything.
   *
   * Notice that the divisor register DLL (divisor latch least ) and DLM (divisor latch most)
   * has the same base address as the receiver/transmitter and the interrupt enable register.
   * To change what the base address point to, we open the "divisor latch" by writting 1 into
   * the Divisor Latch Access Bit (DLAB), whitch is bit index 7 of the Line Control Registers(LCR).
   *
   * Regarding the baud rate value, see [1] "BAUD RATE GENERATOR PROGRAMMING TABLE".
   * We use 38.4k while 1.8432 MHZ crystal, so the corresponding value is 3.
   * And due to the divisor register is tow bytes (16 bits), so we need to split the value of
   * 3(0x0003) into two bytes, DLL stores the low byte,
   * DLM store the high byte.
   * */
  uint8_t lcr = uart_read_reg(LCR);
  uart_write_reg(LCR, lcr|(1<<7));
  uart_write_reg(DLL, 0x03);
  uart_write_reg(DLM, 0x00);

  /*
   * Continue setting thr asynchronous data communiction format.
   * - number of the word length: 8bits
   * - number of stop bits: 1 bit when word length is 8bits
   * - no parity
   * - no break control
   * - disable baud latch
   */

  lcr = 0;
  uart_write_reg(LCR, lcr | (3 <<0 ));
}


int uart_putc(char ch)
{
  while((uart_read_reg(LSR) & LSR_TX_IDLE ) == 0);
  return uart_write_reg(THR, ch);
}

void uart_puts(char *s)
{
  while(*s)
  {
    uart_putc(*s++);
  }
}
