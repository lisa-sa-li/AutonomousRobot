/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  

#ifndef RTC_H
#define RTC_H

#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "lcd.h"


unsigned char time[7]; // Create a byte array to hold time read from RTC
int initSeconds = 0;
int finalSeconds = 0;
int initialTime = 0;
int finalTime = 0;
char opDate[3] = {'d','d','d'};
int timeDifference = 0;
int minuteFinal;
int secondFinal;

const char happynewyear[7] = {
    0x00, // 45 Seconds 
    0x05, // 59 Minutes
    0x16, // 24 hour mode, set to 23:00
    0x01, // Sunday
    0x22, // 31st
    0x04, // December
    0x19  // 2018
};

void rtc_set_time(void);
void timeDifCalc(void);
void RTC_loop(void);


//void rtc_run(void);
//void rtc_main(unsigned char * time);
//long rtc_getSeconds(void);
//void rtc_startOperation(void);
//int rtc_getOperatingTime(void);
//void rtc_getTime(unsigned char * time);


#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

#endif	/* RTC_H */