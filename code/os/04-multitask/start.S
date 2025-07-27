#include "platform.h"

#size of each hart's stack is 1024 bytes
.equ STACK_SIZE, 1024
.global _start

  .text
_start:
  #park harts with id !=0
  csrr t0, mhartid #read current hart id
  mv tp, t0 #keep CPU's hardid in its tp for later usage.
  bnez t0, park #if we'r not on the hart 0, we park the hart 

  #Setup staks, thr stack grows fomr bottom to top, so we put the
  #stack pointer to the end of the stack space

  slli t0, t0, 10 # shift left the hart id by 1024
  la sp, stacks + STACK_SIZE #set the initial stack pointer
                            #to its place in the stack space
  add sp, sp, t0 #move the current hart stack pointer
                #to its place in the stack space
  
  j start_kernel #hart 0 jump to c

park:
  wfi
  j park

# In the standard RISC-V calling covention, the stack pointer sp
# is always 16-byte aligned.

.balign 16
stacks:
  .skip STACK_SIZE * MAXNUM_CPU #allocate space for all the harts stacks
 
 .end #End of file
