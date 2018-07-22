/*
 * programmable interrupt controller
 * 
 * $Id: PIC.C 1.1 1996/11/26 15:42:26 bart Exp $
 *
 */

#include <sys/pic.h>
#include <i386/inlines.h>

#define SLAVE_IRQ	8
#define MASTER_SLAVE	2

static ushort irq_mask = 0xFFFF;

void
init_pic(ushort master, ushort slave)
{
	outb(ICU0, ICU_RESET);
	outb(ICU0+1, master);
	outb(ICU0+1, 0x04);
	outb(ICU0+1, 0x01);
	outb(ICU0+1, 0xFF);
	outb(ICU0, 0x02);

	outb(ICU1, ICU_RESET);
	outb(ICU1+1, slave);
	outb(ICU1+1, 0x02);
	outb(ICU1+1, 0x01);
	outb(ICU1+1, 0xFF);
	outb(ICU1, 0x02);
}

void
enable_irq(ushort irq_no)
{
	irq_mask &= ~(1 << irq_no);
	if(irq_no >= SLAVE_IRQ)
		irq_mask &= ~(1 << MASTER_SLAVE);
	
	outb(ICU0+1, irq_mask & 0xFF);
	outb(ICU1+1, (irq_mask >> 8) & 0xFF);
}

void
disable_irq(ushort irq_no)
{
	irq_mask |= (1 << irq_no);
	if(irq_no >= SLAVE_IRQ)
		irq_mask |= (1 << MASTER_SLAVE);
	
	outb(ICU0+1, irq_mask & 0xFF);
	outb(ICU1+1, (irq_mask >> 8) & 0xFF);
}


#define TRES_PORT	0x40

void
set_res()
{
	outb(TRES_PORT, 0x9C);
	outb(TRES_PORT, 0x2E);
}
