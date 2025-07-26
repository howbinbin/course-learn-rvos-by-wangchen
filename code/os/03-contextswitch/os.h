#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>


/* uart */
extern void uart_init(void);
extern void uart_puts(char *s);

/* printf */
extern int printf(const char* s, ...);
extern void panic(char *s);

/* memory management */
extern void *page_alloc(int npages);
extern void page_free(void *p);


/*task management*/

struct context 
{
  /* ignore x0 */
  reg_t ra; //return address
  reg_t sp; //stack pointer
  reg_t gp; //global pointer
  reg_t tp; //thread pointer
  reg_t t0; //temporary
  reg_t t1; //temporary
  reg_t t2; //temporary
  reg_t s0; //saved register, frame pointer
  reg_t s1; //saved register
  reg_t a0; //funcation argument, return value 
  reg_t a1; //funcation argument, return value 
  reg_t a2; //funcation argument 
  reg_t a3; //funcation argument 
  reg_t a4; //funcation argument 
  reg_t a5; //funcation argument 
  reg_t a6; //funcation argument 
  reg_t a7; //funcation argument 
  reg_t s2; //saved register
  reg_t s3; //saved register 
  reg_t s4; //saved register 
  reg_t s5; //saved register 
  reg_t s6; //saved register 
  reg_t s7; //saved register 
  reg_t s8; //saved register 
  reg_t s9; //saved register 
  reg_t s10; //saved register 
  reg_t s11; //saved register 
  reg_t t3; //temporary
  reg_t t4; //temporary
  reg_t t5; //temporary
  reg_t t6; //temporary
};

extern int  task_create(void (*task)(void));
extern void task_delay(volatile int count);

#endif // !__OS_H__
