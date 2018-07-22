/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
int 
strcmp(const char* s1, const char* s2)
{
   while(*s1 == *s2) {
      if(!*s1++)
         return 0;
      s2++;
   }
   return 1;
}
