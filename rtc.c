/*
 * File:   rtc.c
 * Author: lisali
 *
 * Created on February 21, 2019, 10:41 PM
 */

#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "rtc.h"
#include "lcd.h"
#include "i2c.h"


/** @brief Writes the happynewyear array to the RTC memory */
void rtc_set_time(void){
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11011000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    
    // Write array
    for(char i=0; i < 7; i++){
        I2C_Master_Write(happynewyear[i]);
    }
    
    I2C_Master_Stop(); //Stop condition
}

void timeDifCalc(void){
    timeDifference = finalSeconds - initSeconds;
    //lcd_clear();
    //printf("time difference");
    //lcd_set_ddram_addr(LCD_LINE2_ADDR);
    //printf("%d", timeDifference);
    minuteFinal = (timeDifference / 60);
    secondFinal = (timeDifference % 60);  
    finalTime = 0;
}

void RTC_loop(void){
    int tS, tM, tH;
    int TS, TM, TH;
    // Reset RTC memory pointer
    
    
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    I2C_Master_Stop(); // Stop condition

    // Read current time
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
    for(unsigned char i = 0; i < 6; i++){
        time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
    }
    time[6] = I2C_Master_Read(NACK); // Final Read with NACK
    I2C_Master_Stop(); // Stop condition
    
    //Converting char HH:MM:SS into integer seconds
    tS = (int)time[0];
    tM = (int)time[1];
    tH = (int)time[2];
    TS = 10*(tS/16) + (tS%16);
    TM = 10*(tM/16) + (tM%16);
    TH = 10*(tH/16) + (tH%16);
    TH = TH - 12; 
   
    //Converting time into seconds
    if (initialTime == 1){
        initSeconds = TS + TM * 60 + TH * 3600;
        initialTime = 0;
    } else if (finalTime == 1){
        finalSeconds = TS + TM * 60 + TH * 3600;
    } else {
    // Print received data on LCD
//        lcd_home();
//        lcd_clear();
        
        
        lcd_set_ddram_addr(LCD_LINE3_ADDR);
        printf("%02x/%02x/%02x", time[6],time[5],time[4]); // Print date in YY/MM/DD
        lcd_set_ddram_addr(LCD_LINE4_ADDR);
        printf("%02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS
        __delay_ms(1000);
    }
}


///* 
// * File:   rtc.c
// * Author: lisali
// *
// * Created on February 21, 2019, 10:41 PM
// */
//
//#include <xc.h>
//#include <stdio.h>
//#include "configBits.h"
//#include "rtc.h"
//#include "lcd.h"
//
//typedef struct time { // Struct of the time
//    int h;
//    int m;
//    int s;
//} Time;
//
//static long opTime;
//static Time startTime;
//static Time endTime;
//
///*
//void main(void) {
//    rtc_run();
//    rtc_startOperation();
//    __delay_ms(2000);
//    rtc_getOperatingTime();
//} */
//    /*
//void rtc_run(void) {
//    // Initialize I2C Master with 100 kHz clock
//    I2C_Master_Init(100000);
//    
//    // Set the time in the RTC. To see the RTC keep time, comment this line out
//    // after programming the PIC directly before with this line included
//    //rtc_set_time();
//    
//    // Main loop
//    unsigned char time[7]; // Create a byte array to hold time read from RTC
//    
//    rtc_startOperation();
//    
//    while(1){
//        // Reset RTC memory pointer
//        I2C_Master_Start(); // Start condition
//        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
//        I2C_Master_Write(0x00); // Set memory pointer to seconds
//        I2C_Master_Stop(); // Stop condition
//
//        // Read current time
//        I2C_Master_Start(); // Start condition
//        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
//        for(unsigned char i = 0; i < 6; i++){
//            time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
//        }
//        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
//        I2C_Master_Stop(); // Stop condition
//        
//        // Print received data on LCD
//        lcd_home();
//        lcd_set_ddram_addr(LCD_LINE3_ADDR);
//        printf("%02x/%02x/%02x", time[6],time[5],time[4]); // Print date in YY/MM/DD
//        lcd_set_ddram_addr(LCD_LINE4_ADDR);
//        printf("%02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS
//        __delay_ms(1000);
//        if (key_was_pressed){
//            key_was_pressed=false;
//            break;}
//       
//        
//    }
//}*/
//
///** @brief Writes the happynewyear array to the RTC memory */
//void rtc_set_time(void){
//    I2C_Master_Start(); // Start condition
//    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
//    I2C_Master_Write(0x00); // Set memory pointer to seconds
//    
//    // Write array
//    for(char i=0; i < 7; i++){
//        I2C_Master_Write(happynewyear[i]);
//    }
//    
//    I2C_Master_Stop(); //Stop condition
//}
//
//void rtc_getTime(unsigned char * time) {
//    /* Reset RTC memory pointer */
//    I2C_Master_Start(); // Start
//    I2C_Master_Write(0b11010000); // 7 bit RTC address + write
//    I2C_Master_Write(0x00); // Set memory pointer to seconds
//    I2C_Master_Stop(); // Stop
//    
//    /* Read current time */
//    char i;
//    
//    I2C_Master_Start(); // Start
//    I2C_Master_Write(0b11010001); // 7 bit RTC address + read
//    
//    for (i = 0; i < 6; i++) {
//        time[i] = I2C_Master_Read(ACK); // Read with ACK (0) to continue reading
//    }
//    time[6] = I2C_Master_Read(NACK); // Read with NACK (1) to stop reading
//    I2C_Master_Stop(); // Stop
//}
//
//long rtc_getSeconds(void) {
//    /*  Returns: The number of seconds since the start of the day 
//     */
//    
//    char time[7];
//    //RTC_main(time);
//    
//    long seconds = __bcd_to_num(time[0]) + 60*__bcd_to_num(time[1]) + 60*60*__bcd_to_num(time[2]);
//    //long seconds = __bcd_to_num(time[0]) + 60*__bcd_to_num(time[1]) + 60*60*__bcd_to_num(time[2]) + 60*60*24*__bcd_to_num(time[3]);
//    //long seconds = __bcd_to_num(time[0]);
//    return seconds;
//}
//
//void rtc_startOperation(void) {
//    /* Starts timing the operation */
//    char time[7];
//    //RTC_main(time);
//    startTime.h = __bcd_to_num(time[2]);
//    startTime.m = __bcd_to_num(time[1]);
//    startTime.s = __bcd_to_num(time[0]);
//}
//
//int rtc_getOperatingTime(void) {
//    /* Returns: the number of seconds the operation took*/
//    char time[7];
//    //RTC_main(time);
//    endTime.h = __bcd_to_num(time[2]);
//    endTime.m = __bcd_to_num(time[1]);
//    endTime.s = __bcd_to_num(time[0]);
//    
//    Time opTimeLocal;
//    opTimeLocal.h = endTime.h - startTime.h;
//    opTimeLocal.m = endTime.m - startTime.m;
//    opTimeLocal.s = endTime.s - startTime.s;
//    
//    int res = 60*60*opTimeLocal.h + 60*opTimeLocal.m + opTimeLocal.s;
//    return res;
//}
//
///*
//void rtc_print(void){
//    rtc_getOperatingTime();
//    char month[] = "MMM";
//    int INT_time[7];
//    for(int i = 0; i < 7; i++){
//        INT_time[i] = __bcd_to_num(time[i]);
//    }
//    int RTC_month = INT_time[5];
//    
//    switch(RTC_month){
//        case 1 :
//            strcpy(month, "JAN");
//            break;
//        case 2:
//            strcpy(month, "FEB");
//            break;
//        case 3:
//            strcpy(month, "MAR");
//            break;
//        case 4:
//            strcpy(month, "APR");
//            break;
//        case 5:
//            strcpy(month, "MAY");
//            break;
//        case 6:
//            strcpy(month, "JUN");
//            break;    
//        case 7:
//            strcpy(month, "JUL");
//            break;  
//        case 8:
//            strcpy(month, "AUG");
//            break;    
//        case 9:
//            strcpy(month, "SEP");
//            break;    
//        case 10:
//            strcpy(month, "OCT");
//            break;    
//        case 11:
//            strcpy(month, "NOV");
//            break;    
//        case 12:
//            strcpy(month, "DEC");
//            break;
//    }
//    __lcd_home();
//    printf("%02d%s | %02d:%02d:%02d", INT_time[4],month,INT_time[2],INT_time[1],INT_time[0]);
//}*/
