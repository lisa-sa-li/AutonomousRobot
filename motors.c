/*
 * File:   motors.c
 * Author: lisali
 *
 * Created on February 21, 2019, 5:37 PM
 */

#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "motors.h"

void nemaControl(motorDirection dir) { 
    if (dir == AB) {
        LATAbits.LATA4 = 1; 
        LATBbits.LATB0 = 1;
        LATBbits.LATB2 = 0;
        LATBbits.LATB3 = 0;
    } else if (dir == BC) {
        LATAbits.LATA4 = 0; 
        LATBbits.LATB0 = 1;
        LATBbits.LATB2 = 1;
        LATBbits.LATB3 = 0;
    } else if (dir == CD) {
        LATAbits.LATA4 = 0; 
        LATBbits.LATB0 = 0;
        LATBbits.LATB2 = 1;
        LATBbits.LATB3 = 1;
    } else if (dir == DAD) {
        LATAbits.LATA4 = 1; 
        LATBbits.LATB0 = 0;
        LATBbits.LATB2 = 0;
        LATBbits.LATB3 = 1;
    }
}  

void nemaRotate(motorDirection dir) {
    LATAbits.LATA3 = 1; // LED
    unsigned int j;
    if (dir == FORWARD) {  
        LATDbits.LATD0 = 1; 
        LATDbits.LATD1 = 0; 
        for(j=0;j<1100;j++) {         //Rotate forward
            nemaControl(DAD);
            __delay_ms(_NEMA);
            nemaControl(CD);
            __delay_ms(_NEMA);
            nemaControl(BC);
            __delay_ms(_NEMA);
            nemaControl(AB);
            __delay_ms(_NEMA);
        }
        LATDbits.LATD0 = 1; 
        LATDbits.LATD1 = 1;
    } else if (dir == BACKWARD) {
        for(j=0;j<3000;j++) {         //Rotate forward
            nemaControl(AB);
            __delay_ms(_NEMA);
            nemaControl(BC);
            __delay_ms(_NEMA);
            nemaControl(CD);
            __delay_ms(_NEMA);
            nemaControl(DAD);
            __delay_ms(_NEMA);
        }
    }
    LATAbits.LATA4 = 0; 
    LATBbits.LATB0 = 0;
    LATBbits.LATB2 = 0;
    LATBbits.LATB3 = 0;
    LATAbits.LATA3 = 0; //LED
}

void servoRotate(void) {
    LATAbits.LATA1 = 1; 
    unsigned int i;
    unsigned int j;
    for(i=0; i<20; i++) {
        LATEbits.LATE0 = 1;  // Left door
        LATEbits.LATE1 = 1;  // Right door
        __delay_us(1080);
        LATEbits.LATE0 = 0;
        __delay_us(700);
        LATEbits.LATE1 = 0;
        __delay_us(18220);
    }
    __delay_ms(800);
    for(j=0; j<20; j++) {
        LATEbits.LATE0 = 1;  // Left door
        LATEbits.LATE1 = 1;  // Right door
        __delay_us(800);
        LATEbits.LATE1 = 0;
        __delay_us(1300);
        LATEbits.LATE0 = 0;
        __delay_us(17900);
    }
    __delay_ms(800);
    for(i=0; i<20; i++) {
        LATEbits.LATE0 = 1;  // Left door
        LATEbits.LATE1 = 1;  // Right door
        __delay_us(1080);
        LATEbits.LATE0 = 0;
        __delay_us(700);
        LATEbits.LATE1 = 0;
        __delay_us(18220);
    }
    LATAbits.LATA1 = 0; 
}

void rackPinion(int pinNum) {
    LATAbits.LATA2 = 1; 
    unsigned int i;
    unsigned int j;
    if (pinNum == 0){
        while (!PORTCbits.RC0){
            LATCbits.LATC1 = 1;  
            __delay_us(1200);
            LATCbits.LATC1 = 0;
            __delay_us(18800);
        }
        __delay_ms(500);
        while (!PORTCbits.RC5) {
            LATCbits.LATC1 = 1;  
            __delay_us(1800);
            LATCbits.LATC1 = 0;
            __delay_us(18200);
        }
    } else if (pinNum == 1){
        while (!PORTAbits.RA5) {
            LATCbits.LATC2 = 1;
            __delay_us(1200);
            LATCbits.LATC2 = 0;
            __delay_us(18800);
        }
        __delay_ms(500);
        while (!PORTEbits.RE2) {
            LATCbits.LATC2 = 1; 
            __delay_us(1800);
            LATCbits.LATC2 = 0;
            __delay_us(18200);
        }
    }
    LATAbits.LATA2 = 0; 
}

void deploy_tires(int num_tires) {
    //printf("%s","hi");
    if (num_tires == 0) {
        return;
    } else if (num_tires == 1) {
        deploy_one();   
    } else if (num_tires == 2) {
        deploy_one();   
        deploy_one();   
    } else {
        printf("%s","uh oh");
    }
    return;
}

void deploy_one(void) {  
    rackPinion(0);
    __delay_ms(100);
    servoRotate();   
    __delay_ms(100);
    if (!even_odd_cnt) {   // IF ODD, PUSH STACKS UP AFTER DISPENSING
        nemaRotate(FORWARD);
    } else { // If even, push stack 2 onto stack 1
        rackPinion(1);
    }
    even_odd_cnt = ~even_odd_cnt;
}        
 