//
// Programmable Interrupt Controller (82C59)
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//
// Reference:
//  Intel Platform Components (order number: 296467-009)
//

#define PIC_MASTER 	0x20
#define PIC_SLAVE	 	0xA0

unsigned short	IrqMask;

//
// Description:
//  Initializes the programmable interrupt controller
//  by sending Interrupt Control Words to master and
//  slave controllers and remaps their interrupts to
//  32+ in the interrupt table.
//
void PIC_Initialize()
{
	// Reprogram master PIC
	outb(PIC_MASTER, 0x11);
   outb(PIC_MASTER+1, 0x20);
   outb(PIC_MASTER+1, 0x04);
   outb(PIC_MASTER+1, 0x01);

   // Reprogram slave PIC
   outb(PIC_SLAVE, 0x11);
   outb(PIC_SLAVE+1, 0x28);
   outb(PIC_SLAVE+1, 0x02);
   outb(PIC_SLAVE+1, 0x01);
};

//
// Description:
//  Enables a specified Interrupt Request Line (IRQ)
//  by manipulating the IRQ Mask and sending the data
//  to the PIC's.
//
void PIC_EnableIRQ(unsigned short Irq)
{
	IrqMask &= ~(1 << Irq);

	outb(PIC_MASTER+1, IrqMask & 0xFF);
	outb(PIC_SLAVE+1, (IrqMask >> 8) & 0xFF);
};

//
// Description:
//  Disables a specified Interrupt Request Line (IRQ)
//  by manipulating the IRQ Mask and sending the data
//  to the PIC's.
//
void PIC_DisableIRQ(unsigned short Irq)
{
	IrqMask |= (1 << Irq);

	outb(PIC_MASTER+1, IrqMask & 0xFF);
	outb(PIC_SLAVE+1, (IrqMask >> 8) & 0xFF);
};
