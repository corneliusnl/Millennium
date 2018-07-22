/*
 * keyboard driver
 * based on VSTa kbd server
 * Copyright (c) 1996 Andy Valencia
 *
 * $Id: KB.C 1.2 1996/12/15 08:35:16 bart Exp $
 *
 */

#include <types.h>
#include <sys/thread.h>
#include <i386/inlines.h>
#include <sys/kb.h>

#define  KB_BUFSZ 	(32)

static char kb_buf[KB_BUFSZ] = {0};
static uint kb_count = 0;

static char* pkbin = kb_buf;
static char* pkbout = kb_buf;

static int shift = 0;
static int ctrl = 0;
static int alt = 0;
static int caps = 0;
static int num = 0;

static char normal[] = {
  0x00,0x1B,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
'q','w','e','r','t','y','u','i','o','p','[',']',0x0D,0x80,
'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
0134,'z','x','c','v','b','n','m',',','.','/',0x80,
'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,'0',0177
};


static char shifted[] = {
  0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
'|','Z','X','C','V','B','N','M','<','>','?',0x80,
'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
'1','2','3','0',177
};

static struct tcb* kb_thread = NULL; /* thread waiting for key */
static uint kb_wait = 0;

void
kb_waiter(struct tcb* t)
{
   kb_thread = t;
   kb_wait = 1;
}

static void
kb_service(uchar ch)
{
   if(ch) {
      kb_wait = 0;
      kb_thread->state = TS_READY;
   }
}

static int
kb_special(uchar key)
{
   switch(key) {
      case 0x36:
      case 0x2A: 
         shift = 1;
         break;
      case 0xB6:
      case 0xAA:
         shift = 0;
         break;
      case 0x1D:
         ctrl = 1;
         break;
      case 0x9D:
         ctrl = 0;
         break;
      case 0x38:
         alt = 1;
         break;
      case 0xB8:
         alt = 0;
         break;
      case 0x3A:
      case 0x45:
         break;
      case 0xBA:
         caps = !caps;
         break;
      case 0xC5:
         num = !num;
         break;
      case 0xE0:
         break;
      default:
         return(0);
   }
   return (1);
}

void
kb_qkey(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   uchar k = inb(KB_DATA);

   /*
    * check if special key 
    */
   if(kb_special(k))
      return;

   if(k >= 0x80)
      return;

   /* 
    * Ctl-C pressed
    */
   if(ctrl && (k == 0x2E))
      killme(f);

   /* 
    * check if any thread waiting for key 
    */
   if(kb_wait && k) 
      kb_service(k);
   
   /*
    * enqueue key
    */
   if( kb_count < KB_BUFSZ ) {
      *pkbin++ = k;
      kb_count++;
      if( pkbin >= kb_buf+KB_BUFSZ ) {
         pkbin = kb_buf;
      }
   } else {
      pkbin = kb_buf;
   }
}

static char 
kb_readbuf()
{
   char ch;

   cli();

   if( kb_count == 0 ) {
      ch = 0;
   } else {
      kb_count--;
   
      ch = *pkbout++;
      if( pkbout >= kb_buf + KB_BUFSZ)
         pkbout = kb_buf;
   }

   sti();   
   return(ch);
}

char
kb_getch()
{
   char ch;

   ch = kb_readbuf();

   if( ch == 0 )
      return (0);

   return (shift ? shifted[ch] : normal[ch]);
}
