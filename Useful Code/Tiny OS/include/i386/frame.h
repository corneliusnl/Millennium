#ifndef _FRAME_H_
#define _FRAME_H_

/*
 * stack frame structures
 */

#include <types.h>

/*
 * standard stack frame (IRQs, call gates, CPU traps with no error code )
 */
struct frame {
   ulong esds;
   ulong edi;
   ulong esi; 
   ulong ebp; 
   ulong _esp;
   ulong ebx;
   ulong edx; 
   ulong ecx; 
   ulong eax;  

   ulong eip;
   ulong ecs;
   ulong eflags;
   ulong esp;
   ulong ess;
};

/*
 * stack frame with error code ( some CPU traps )
 */
struct eframe {
   ulong esds;
   ulong edi;
   ulong esi; 
   ulong ebp; 
   ulong _esp;
   ulong ebx;
   ulong edx; 
   ulong ecx; 
   ulong eax;  

   ulong ecode;

   ulong eip;
   ulong ecs;
   ulong eflags;
   ulong esp;
   ulong ess;
};

#endif /* _FRAME_H_ */
