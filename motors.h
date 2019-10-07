/* 
 * File:   
 * Author: LISA LI
 * Comments: feb 21st
 * Revision history: 
 */

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "configBits.h"

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MOTORS_H
#define	MOTORS_H

typedef enum motorDirection {A, B, C, D, AB, BC, CD, DAD, FORWARD, BACKWARD} motorDirection;

void motorControl(motorDirection dir, int lowHigh);
void nemaControl(motorDirection dir);
//void rackPinion(int highLow);
void rackPinion(int pinNum);
void deploy_tires(int num_tires);
void deploy_one(void);
void servoRotate(void); 
void nemaRotate(motorDirection dir);
volatile bool even_odd_cnt = true;  // True = even, False = odd

#endif	/* MOTORS_H*/

