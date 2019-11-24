#include <stdio.h>
#include "main.h"
#include "PORT.H"
#include "sEOS.h"



main()
{
   AUXR=0x0c;   // make all of XRAM available, ALE always on
   if(OSC_PER_INST==6)
   {
       CKCON0=0x01;  // set X2 clock mode
   }
   else if(OSC_PER_INST==12)
   {
       CKCON0=0x00;  // set standard clock mode
   }

   sEOS_init(MS_PER_TICK);
   while(1);
}
