/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include "rtc.h"

int rtc_gettime (RTC *rtc)
{

//   BYTE buf[8];

//   rtc_read() //This is where you would read the clock.
   
//   rtc->sec = (buf[0] & 0x0F) + ((buf[0] >> 4) & 7) * 10;
//   rtc->min = (buf[1] & 0x0F) + (buf[1] >> 4) * 10;
//   rtc->hour = (buf[2] & 0x0F) + ((buf[2] >> 4) & 3) * 10;
//   rtc->mday = (buf[4] & 0x0F) + ((buf[4] >> 4) & 3) * 10;
//   rtc->month = (buf[5] & 0x0F) + ((buf[5] >> 4) & 1) * 10;
//   rtc->year = 2000 + (buf[6] & 0x0F) + (buf[6] >> 4) * 10;


   //This code is just to provide some kind of a valid response.
   rtc->sec = 1;
   rtc->min = 2;
   rtc->hour = 3;
   rtc->mday = 4;
   rtc->month = 5;
   rtc->year = 2006;


   return 1;
}

int rtc_settime (const RTC *rtc)
{
   BYTE buf[8];

   buf[0] = rtc->sec / 10 * 16 + rtc->sec % 10;
   buf[1] = rtc->min / 10 * 16 + rtc->min % 10;
   buf[2] = rtc->hour / 10 * 16 + rtc->hour % 10;
   buf[3] = 0;
   buf[4] = rtc->mday / 10 * 16 + rtc->mday % 10;
   buf[5] = rtc->month / 10 * 16 + rtc->month % 10;
   buf[6] = (rtc->year - 2000) / 10 * 16 + (rtc->year - 2000) % 10;

/*This is where you would set the new time to the clock*/

   return 1;
}

int rtc_init (void)
{
   BYTE buf[8];   /* RTC R/W buffer */
//   UINT n;

   /* Read RTC */
//   rtc_read() //This is where you would read the clock the first time.

   if (/*SOMETHING_IS_WRONG*/0) {   /* When RTC data has been broken, set default time */
      /* Reset time to Jan 1, '08 */
      memset(buf, 0, 8);
      buf[4] = 1; buf[5] = 1; buf[6] = 8;
//      rtc_write(buf);
      /* Clear data memory */
      memset(buf, 0, 8);
//      for (n = 8; n < 64; n += 8)
//         rtc_write(buf);
      return 0;
   }
   return 1;
} 
