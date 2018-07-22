#ifndef _INLINES_H_
#define _INLINES_H_

inline static uchar
inb(int port)
{
   register uchar r;
   
   __asm__ __volatile__( "inb %%dx,%%al\n\t" : "=a" (r) : "d" (port));
   return(r);
}

inline static void
outb(int port, uchar data)
{
   __asm__ __volatile__("outb %%al,%%dx\n\t" :: "a" (data), "d" (port));
}

inline static ushort
inw(int port)
{
   register ushort r;
   
   __asm__ __volatile__("inw %%dx,%%ax\n\t" : "=a" (r) : "d" (port));
   return(r);
}

inline static void
outw(int port, ushort data)
{
   __asm__ __volatile__("outw %%ax,%%dx\n\t" :: "a" (data), "d" (port));
}

inline static void
repinsw(int port, void* va, int count)
{
   __asm__ __volatile__ ("rep\n\t" \
               "insw" :: "d" (port), "D" (va), "c" (count));
}  

inline static void
repoutsw(int port, void* va, int count)
{
   __asm__ __volatile__ ("rep\n\t" \
               "outsw" :: "d" (port), "D" (va), "c" (count));
}

inline static void
sti()
{
   __asm__ __volatile__ ("sti"::);
}

inline static void
cli()
{
   __asm__ __volatile__ ("cli"::);
}

inline static void
fast_copy(void* src, void* dest, int count)
{
   __asm__ __volatile__ ("rep\n\t" \
               "movsl"   \
               :: "S" (src), "D" (dest), "c" (count));
}

inline static void
ldcr3(ulong val)
{
   __asm__ __volatile__ ("movl %0, %%cr3\n\t" : : "r" (val));
}

inline static ulong
get_cr3(void)
{
   register ulong res;
   
   __asm__ __volatile__( "movl %%cr3, %0\n\t" : "=r" (res) :); 
   return(res);
}

#endif /* _INLINES_H_ */
