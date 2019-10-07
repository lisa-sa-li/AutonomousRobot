#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "stdutils.h"
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <string.h>
#include <stdbool.h>
#include "motors.h"
#include "i2c.h"
#include "rtc.h"

volatile bool key_was_pressed = false; //interrupt for key on keypad being pressed

void main(void) {
    //Write outputs to LATx, read inputs from PORTx depending if pin has been
    //configured through TRISx to be input (=1) or output (=0). Write all pins
    // to logic low (0) initially to prevent startup issues.
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0b00000011;
    LATE = 0x00;
  
    //Set the data direction registers 
    TRISA = 0b00100000; // RA5 (switch 3)
    TRISB = 0b11110111; // Inputs, necessary for keypad
    TRISC = 0b00100001; // RC0 (switch 1), RC5 (switch 2). RC4 and RC5 used for I2C communication.
    TRISD = 0x00; // Outputs initially
    TRISE = 0b00000100; // RE2 (switch 4)
    
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0x0F;  // Digital pins
    
    // RB1, RB4, RB5, RB6, RB7 as inputs (for keypad)
    TRISB = 0b11110010;
    
    // Enable RB1 (keypad data available) interrupt
    INT1IE = 1;
    
    initLCD(); 
    UART_Init();
    I2C_Master_Init(100000); // Initialize I2C Master with 100 kHz clock
    
    // Enable interrupts
    ei();
    INTCONbits.PEIE_GIEL = 1;
    
    // Main loop
    bool run_complete = false;
    
    // When run is complete, set this to true so you can see the run data
    bool see_run_data = false;
    
    // Ticks to change LCD screen
    unsigned long tick_after_complete = 0;
    unsigned long data_tick = 0;
    
    
    int counttttt = 0;

    // Set the time in the RTC. To see the RTC keep time, comment this line out
    // after programming the PIC directly before with this line included
    //rtc_set_time();
   
    // Data to be stored delayed
    unsigned long operation_time = 0;
    unsigned char num_tires_dispensed[10];// = {1, 2, 2, 1, 1, 2};
    unsigned char num_tires_loaded[10];// = {2, 2, 2, 2, 1, 2};
    unsigned long num_supplied = 8; // Need to decide this
    unsigned long num_dispensed = 0; // Need to decide this
    unsigned long num_poles = 0;
    unsigned long distances[10];// = {4, 6, 9, 12, 15, 21};
    
    bool commandReceived = false;
    int commandLength = 0;
    unsigned char command [25];
    int index = 0;
    int numTires;
    bool nanoFlag = false;
    
    while(1){ 
        while (!key_was_pressed){
            if (tick_after_complete == display_interval * 1) {
                lcd_home();
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("%s", msg_press1_1);
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "START"); 
            }
            if (tick_after_complete == display_interval * 2) {
                lcd_home();
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("%s", msg_press2_1);
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "RTC");
                if (!see_run_data) { tick_after_complete = 0; }
            }
////            if (tick_after_complete == 3) {
//            if (tick_after_complete == display_interval * 3) {
//                lcd_home();
//                lcd_clear();
//                lcd_set_ddram_addr(LCD_LINE1_ADDR);
//                printf("%s", msg_press3_1);
//                lcd_set_ddram_addr(LCD_LINE2_ADDR);
//                printf("%s", "TRAP DOOR");
//                //__delay_ms(display_interval);   
//            }
////            if (tick_after_complete == 4) {
//            if (tick_after_complete == display_interval * 4) {
//                lcd_home();
//                lcd_clear();
//                lcd_set_ddram_addr(LCD_LINE1_ADDR);
//                printf("%s", msg_press4_1);
//                lcd_set_ddram_addr(LCD_LINE2_ADDR);
//                printf("%s", "LEADSCREW UP");
//                //__delay_ms(display_interval);   
//
//            }
////            if (tick_after_complete == 5) {
//            if (tick_after_complete == display_interval * 5) {
//                lcd_home();
//                lcd_clear();
//                lcd_set_ddram_addr(LCD_LINE1_ADDR);
//                printf("%s", msg_press5_1);
//                lcd_set_ddram_addr(LCD_LINE2_ADDR);
//                printf("%s", "LEADSCREW DOWN");
//                //__delay_ms(display_interval);   
//            }
////            if (tick_after_complete == 6) {
//            if (tick_after_complete == display_interval * 6) {
//                lcd_home();
//                lcd_clear();
//                lcd_set_ddram_addr(LCD_LINE1_ADDR);
//                printf("%s", msg_press6_1);
//                lcd_set_ddram_addr(LCD_LINE2_ADDR);
//                printf("%s", "FAN");
//                //__delay_ms(display_interval);   
//            }   
//            //if (tick_after_complete == 7) {
//            if (tick_after_complete == display_interval * 7) {
//                lcd_home();
//                lcd_clear();
//                lcd_set_ddram_addr(LCD_LINE1_ADDR);
//                printf("%s", msg_press7_1);
//                lcd_set_ddram_addr(LCD_LINE2_ADDR);
//                printf("%s", "RACK AND PINION");
//                //__delay_ms(display_interval); 
//                if (!see_run_data) { tick_after_complete = 0; }
//            }
            if (tick_after_complete == display_interval * 3) {
                lcd_home();
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("%s", msg_press8_1);
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "RUN DATA");
                tick_after_complete = 0; 
            }  
            tick_after_complete += 1;
          //  RTC_loop();
            __delay_ms(1);
        } 
        if (key_was_pressed) {
            key_was_pressed = false; // Clear the flag                
            unsigned char keypress = (PORTB & 0xF0) >> 4;

            if (keypress == 0) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR); // drive
                printf("%s", "START ROBOT");
                
                UART_send_string("start\n");

                while(!run_complete){
                    while (!nanoFlag){
                        if (UART_available()){
                            command[index] = UART_read();
                            index++;
                        }
                        if (index > 0 && command[index-1] == '\n'){
                            commandReceived = true;
                            commandLength = index;
                            index = 0;
                        } 
                        if (commandReceived) {
                            commandReceived = false;
                            switch(command[0]){
                                case '2':
                                    numTires = 2; nanoFlag = true; 
                                    break;
                                case '1':
                                    numTires = 1; nanoFlag = true; 
                                    break;
                                case '0':
                                    numTires = 0; nanoFlag = true;
                                    break;
                                case 'k':
                                    while (!nanoFlag){
                                        if (UART_available()){
                                            command[index] = UART_read();
                                            index++;
                                        }
                                        if (index > 0 && command[index-1] == '\n'){
                                            commandReceived = true;
                                            commandLength = index;
                                            index = 0;
                                        } 
                                        if (commandReceived) {
                                            commandReceived = false;
                                            for (int j = 0; j < (commandLength-1); j++) {
                                                operation_time = operation_time*10 + (command[j] - '0');
                                            }
                                            nanoFlag = true;
                                            run_complete = true;
                                        }
                                    } 
                                    break;
                                default:
                                    break;
                            }
                        }
                    }    
                    if (!run_complete) { 
                        distances[num_poles] = 0;
                        for (int i = 3; i < (commandLength-1); i++) {
                            distances[num_poles] = distances[num_poles]*10 + (command[i] - '0');
                        }

                        num_tires_dispensed[num_poles] = command[0];
                        num_tires_loaded[num_poles] = command[1];

                        deploy_tires(numTires);
                        nanoFlag = false;
                        num_poles += 1;
                        num_dispensed += numTires;
                        numTires = 0;
                        UART_send_string("PICdone\n");
                        __delay_ms(100);
                    } 
                }
                lcd_clear();
                printf("%s", "Finished Run");    
                see_run_data = true;
                __delay_ms(3000);
            }
            if (keypress == 1) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "RTC");
                int rtc_count = 0;
                while (rtc_count < 10) {
                    RTC_loop();    
                    rtc_count ++;
                }
            }
            if (keypress == 2) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "TRAP DOOR");
                servoRotate();
            }
            if (keypress == 4) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "LEADSCREW UP"); 
                nemaRotate(FORWARD);
            }
            if (keypress == 5) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "LEADSCREW DOWN");
                nemaRotate(BACKWARD);
            }
            if (keypress == 6) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "FAN");
                LATDbits.LATD0 = 1; 
                LATDbits.LATD1 = 0; 
                __delay_ms(5000);
                LATDbits.LATD0 = 1; 
                LATDbits.LATD1 = 1; 
            } 
            if (keypress == 8) {
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("%s", "RACK AND PINION");
                rackPinion(1);
                rackPinion(0);
                //see_run_data = true;
            }
            if ((keypress == 9) && (see_run_data)) {
                lcd_clear();
                while(1){
                    while (!key_was_pressed) {
                        if ((data_tick == display_interval * 1) && (num_poles > 1)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press1_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 1 info");
                            if (num_poles == 1) { data_tick=0; } 
                        } 
                        else if ((data_tick == display_interval * 2) && (num_poles > 2)){
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press2_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 2 info");
                            if (num_poles == 2) { data_tick=0; }
                        } 
                        else if ((data_tick == display_interval * 3) && (num_poles > 3)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press3_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 3 info");
                            if (num_poles == 3) { data_tick=0; }  
                        } 
                        else if ((data_tick == display_interval * 4) && (num_poles > 4)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press4_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 4 info");
                            if (num_poles== 4) { data_tick=0; }
                        } 
                        else if ((data_tick == display_interval * 5) && (num_poles >= 5)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press5_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 5 info");
                            if (num_poles == 5) { data_tick=0; }  
                        }  
                        else if ((data_tick == display_interval * 6) && (num_poles >= 6)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press6_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 6 info");
                            if (num_poles == 6) { data_tick=0; }
                        } 
                        else if ((data_tick == display_interval * 7) && (num_poles >= 7)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press7_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 7 info");
                            if (num_poles == 7) { data_tick=0; }
                        } 
                        else if ((data_tick == display_interval * 8) && (num_poles >= 8)) {
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press8_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 8 info");
                            if (num_poles == 8) { data_tick=0; }
//                            __delay_ms(display_interval);   
                        } 
                        else if ((data_tick == display_interval * 9) && (num_poles >= 9)){
                            lcd_home();
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press9_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 9 info");
                            if (num_poles == 9) { data_tick=0; }
//                            __delay_ms(display_interval);   
                        } 
                        else if ((data_tick == display_interval * 10) && (num_poles >= 10)) {
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", msg_press0_1);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Pole 10 info");
                            data_tick = 0;
//                            __delay_ms(display_interval);   
                        } 
                        data_tick ++;
                        __delay_ms(1);
                    }
                    if (key_was_pressed) {
                        key_was_pressed = false; // Clear the flag                
                        unsigned char keypress = (PORTB & 0xF0) >> 4;
                        
                        if ((keypress == 0) && (num_poles >= 1)) { // 1
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 1");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[0]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[0]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[0]);
                            __delay_ms(run_data_interval);
                        }
                        else if ((keypress == 1) && (num_poles >= 2)) { // 2
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 2");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[1]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[1]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[1]);
                            __delay_ms(run_data_interval);
                        } 
                        else if ((keypress == 2) && (num_poles >= 3)) { // 3
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 3");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[2]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[2]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[2]);
                            __delay_ms(run_data_interval);
                        } else if ((keypress == 4) && (num_poles > 4)) { // 4
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 4");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[3]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[3]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[3]);
                            __delay_ms(run_data_interval);
                        } 
                        else if ((keypress == 5) && (num_poles > 5)) { // 5
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 5");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[4]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[4]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[4]);
                            __delay_ms(run_data_interval);
                        } 
                        else if ((keypress == 6) && (num_poles > 6)) { // 5
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 6");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[5]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[5]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[5]);
                            __delay_ms(run_data_interval);
                        }
                        else if ((keypress == 8) && (num_poles > 7)) { // 7
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 7");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[6]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[6]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[6]);
                            __delay_ms(run_data_interval);
                        }
                        else if ((keypress == 9) && (num_poles > 8)) { // 8
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 8");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[7]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[7]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[7]);
                            __delay_ms(run_data_interval);
                        }
                        else if ((keypress == 10) && (num_poles > 9)) { // 9
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 9");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[8]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[8]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[8]);
                            __delay_ms(run_data_interval);
                        }
                        else if (keypress == 13) { // 0
                            lcd_clear();
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Pole 10");
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%s", "Dispensed: ");printf("%c", num_tires_dispensed[9]);
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Total : ");printf("%c", num_tires_loaded[9]);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Distance: ");printf("%d", distances[9]);
                            __delay_ms(run_data_interval);
                        }
                        else if (keypress == 14) { // #
                            lcd_clear(); 
                            lcd_set_ddram_addr(LCD_LINE1_ADDR);
                            printf("%s", "Op time: ");printf("%d", operation_time);
                            lcd_set_ddram_addr(LCD_LINE2_ADDR);
                            printf("%d", operation_time/60); printf("%s", "Min ");  printf("%d", operation_time%60); printf("%s", "Sec");
                            lcd_set_ddram_addr(LCD_LINE3_ADDR);
                            printf("%s", "Dispensed: ");printf("%d", num_dispensed);
                            lcd_set_ddram_addr(LCD_LINE4_ADDR);
                            printf("%s", "Supplied: ");printf("%d", num_supplied);
                            __delay_ms(run_data_interval);
                        }
                    }        
                } 
            }
        }
    }
    __delay_ms(10);
}

void __interrupt(high_priority) high_isr(void){
    di();
    
    // Interrupt on change handler for RB1
    if(INT1IF && INT1IE){
        key_was_pressed = true;
        INT1IF = 0; // Clear interrupt flag bit to signify it's been handled
    }
     if(RCIE && RCIF){     
        _rx_buffer[_rx_buffer_head] = RCREG;
        _rx_buffer_head = (unsigned char)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
        RCIF = 0; // Clear interrupt flag bit to signify it's been handled
    }
    if(TXIE && TXIF){
        if((_tx_buffer_tail != _tx_buffer_head)){
            TXREG = _tx_buffer[_tx_buffer_tail];
            _tx_buffer_tail = (unsigned char)(_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;
            TXIF = 0; //Clear interrupt flag bit to signify it's been handled
        }else{
            TXIF = 0;
            TXIE = 0;
        }
    }

    ei();
}

