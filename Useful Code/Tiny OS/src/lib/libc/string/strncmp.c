/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
int 
strncmp(const char* s1, const char* s2, int nbytes)
{
   while(nbytes && (*s1++ == *s2)) {
      if(*s2++ == '\0') {
         return(0);
      }
      nbytes -= 1;
   }
   if(nbytes == 0 ) {
      return(0);
   }
   return(s1[-1] - s2[0]);
}
