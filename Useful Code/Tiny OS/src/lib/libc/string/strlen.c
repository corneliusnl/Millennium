/*
 * Copyright (c) 1996 Andy Valencia
 *
 */

int
strlen(const char *p)
{  
      int x = 0;
                     
   if (p == 0) {
      return(0);
   }            
                     
   while (*p++) 
      ++x; 
   return(x);   
}                    
