#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Noi khai bao hang so
#define BUT0                4
#define BUT1                5
#define BUT2                6 
#define BUT3                7

#define BUT4                8
#define BUT5                9
#define BUT6                10
#define BUT7                11

#define RED_DURATION        5
#define YELLOW_DURATION     2
#define GREEN_DURATION      3


enum State { RED_GREEN, RED_YELLOW, GREEN_RED, 
            YELLOW_RED, RED_RED_1, RED_RED_2};
enum States {GREEN_1,  GREEN_2,
            YELLOW_1, YELLOW_2};
enum Mode {MODE1 = 1, MODE2, MODE3, MODE4, MODE5};

// Noi khai bao bien toan cuc
enum State state    = RED_GREEN;
enum Mode mode      = MODE1;

int green1          = GREEN_DURATION;
int green2          = GREEN_DURATION;
int yellow1         = YELLOW_DURATION;
int yellow2         = YELLOW_DURATION;

int green1_modify   = GREEN_DURATION;
int green2_modify   = GREEN_DURATION;
int yellow1_modify  = YELLOW_DURATION;
int yellow2_modify  = YELLOW_DURATION;

enum States states  = GREEN_2;
int time_in_state   = 0;

int counts          = 0;
int c               = 0;
// ------------ FUNCTION --------------- //
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);


// Ham nut nhan (button)
char numberOfPushButton = 0;
unsigned char isButtonUp();
unsigned char isButtonMotorDown();
void BaiTap_ChongRung();

// Ham hien thi
void displayTime(int a, int b);

// Den giao thong
void button_processing();
void mode_fsm();
void traffic_fsm(void);
void init_traffic();
// ------------------------------------- //
 
void Test_KeyMatrix();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	// unsigned int k = 0;
    init_system();
    init_traffic();
    
	while (1)
	{    
       if (flag_timer1) {
           flag_timer1 = 0;
           ledScan();
       }
	
      if(flag_timer3) {
        flag_timer3 = 0;
         // thuc hien cac cong viec duoi day
        scan_key_matrix();
        button_processing();
        mode_fsm();
        DisplayLcdScreen();
       }
	}
}

// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i = 0; i < value; i++)
		for(j = 0; j < 238; j++);
}

void init_system(void)
{
    LcdClearS();
    TRISB = 0x00;		//setup PORTB is output
    TRISD = 0x00;
    TRISA = 0x00; // SU DUNG PORT A
    TRISE = 0x00;
    PORTD = 0x00;

    init_lcd();
    setPortAo2(~0);
    //LED = 0x00;
    init_interrupt();
    delay_ms(1000);
    setPortAo(0x00);
    
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(4695);//dinh thoi 2ms
    init_timer3(46950);//dinh thoi 10ms
    
    SetTimer0_ms(1000);
    SetTimer1_ms(1000);//TIME RED_GREEN KHI INIT
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    
    init_key_matrix();
}

void init_traffic() {
    LcdClearS();
    
    time_in_state       = 0;
    
    green1_modify       = GREEN_DURATION;
    yellow1_modify      = YELLOW_DURATION;
    green2_modify       = GREEN_DURATION;
    yellow2_modify      = YELLOW_DURATION;
    
    states              = GREEN_2;
    
    SetTimer0_ms(1000);
    setPortAo2(~0b100001);
    
    LcdPrintStringS(0,0,"Mode 1 DO XANH");
    displayTime(green2 + yellow2 - time_in_state, green2 - time_in_state);
}

void traffic_fsm(void)
{
        switch (states)
        {
            case GREEN_2: {
           
                if (flag_timer0)
                {
                    // puts string into buffer
                    time_in_state++;
                    flag_timer0 = 0;
                    
                    LcdPrintStringS(0,0,"Mode 1 DO XANH");
                    displayTime(green2 + yellow2 - time_in_state, 
                                green2 - time_in_state);
                    
                    if (time_in_state == green2)
                    {
                        
                        setPortAo2(~0b010001);
                        states = YELLOW_2;
                        time_in_state = 0;
                        
                        LcdPrintStringS(0,0,"Mode 1 DO VANG");
                        displayTime(yellow2 - time_in_state, 
                                    yellow2 - time_in_state);
                    }
                }
                    break;
            }
            case YELLOW_2: {         
                if (flag_timer0)
                {
                    time_in_state++;
                    flag_timer0 = 0;
                    
                    LcdPrintStringS(0,0,"Mode 1 DO VANG");
                    displayTime(yellow2 - time_in_state, 
                                yellow2 - time_in_state);

                    if (time_in_state == yellow2)
                    {                
                        time_in_state = 0;
                        states = GREEN_1;
                        
                        setPortAo2(~0b001100);
                        LcdPrintStringS(0,0,"Mode 1 XANH DO");
                        displayTime(green1 - time_in_state, 
                                    green1 + yellow1 - time_in_state);
                     }
                }
                    break;
            }
             case GREEN_1: {
              
                if (flag_timer0)
                {
                    time_in_state++;
                    flag_timer0=0;
                     
                    LcdPrintStringS(0,0,"Mode 1 XANH DO");
                    displayTime(green1 - time_in_state, 
                                green1 + yellow1 - time_in_state);
                    
                    if (time_in_state == green1)
                    {
                        setPortAo2(~0b001010);
                        states=YELLOW_1;
                        time_in_state=0;
                        
                        LcdPrintStringS(0,0,"Mode 1 VANG DO");
                        displayTime(yellow1 - time_in_state, 
                                    yellow1 - time_in_state);
                     }
                }
                    break;
            }
             
              case YELLOW_1: {
          
                if (flag_timer0)
                {
                    time_in_state++;
                    flag_timer0 = 0;

                     
                    LcdPrintStringS(0,0,"Mode 1 VANG DO");
                    displayTime(yellow1 - time_in_state, 
                                yellow1 - time_in_state);
                    
                    if (time_in_state == yellow1)
                    {
                    
                        time_in_state = 0;
                        states = GREEN_2;
                        
                        setPortAo2(~0b100001);
                        LcdPrintStringS(0,0,"Mode 1 DO XANH");
                        displayTime(green2 + yellow2 - time_in_state, 
                                    green2 - time_in_state);
                    }
                }
                    break;
            }
        }
}
void displayTime(int a, int b) {
    UpdateBuffer(a, b);
    
    LcdPrintNumS(1, 7,  a/10);
    LcdPrintNumS(1, 8,  a%10);
    LcdPrintNumS(1, 12, b/10);
    LcdPrintNumS(1, 13, b%10);
}
void displayTime2(int a, int b, int c) {
    LcdPrintNumS(1, 2   ,a/10);
    LcdPrintNumS(1, 3   ,a%10);
    LcdPrintNumS(1, 7   ,b/10);
    LcdPrintNumS(1, 8   ,b%10);
    LcdPrintNumS(1, 12  ,c/10);
    LcdPrintNumS(1, 13  ,c%10);
}
void displayTime3(int a, int b, int c, int d) {
    LcdPrintNumS(1, 0,  a/10);
    LcdPrintNumS(1, 1,  a%10);
    LcdPrintNumS(1, 4,  b/10);
    LcdPrintNumS(1, 5,  b%10);
    LcdPrintNumS(1, 8,  c/10);
    LcdPrintNumS(1, 9,  c%10);
    LcdPrintNumS(1, 12, d/10);
    LcdPrintNumS(1, 13, d%10);
}

void display_when_changing_mode(int changed_mode) {
    switch (changed_mode)
    {
    case MODE1:
        yellow2_modify = yellow2;
        setPortAo2(~0b0);
        init_traffic();

        break;

    case MODE2:
        LcdClearS();
        LcdPrintStringS(0,0,"Mode 2 SET XANH1");
        displayTime3( green1, yellow1, green2, yellow2);
        UpdateBuffer(green1,(int)2);
        SetTimer0_ms(500);
        setPortAo2(~0b0);
        
        break;

    case MODE3:
        LcdClearS();
        green1_modify = green1;
        LcdPrintStringS(0,0,"Mode 3 SET VANG1");
        displayTime3( green1, yellow1, green2, yellow2);
        setPortAo2(~0b0);
        UpdateBuffer(yellow1, (int)3);
        
        break;

    case MODE4:
        yellow1_modify = yellow1;

        LcdClearS();
        LcdPrintStringS(0,0,"Mode 4 SET XANH2");
        displayTime3(green1, yellow1, green2, yellow2);
        setPortAo2(~0b0);
        UpdateBuffer(green2,(int)4);
        
        break;
    
    case MODE5:
        green2_modify = green2;

        LcdClearS();
        LcdPrintStringS(0,0,"Mode 4 SET VANG2");
        displayTime3(green1, yellow1, green2, yellow2);
        setPortAo2(~0b0);
        UpdateBuffer(yellow2,(int)5);
        
        break;

    default:
        break;
    }
}

void button_processing() {
    // --------- BUTTON1 WAS PRESSED -----------------
    if (key_code[BUT0] == 1 || key_code[BUT0] >= 20) {
        if (mode == MODE5) {
            mode = MODE1;
        }
        else {
            mode++;
        }

        display_when_changing_mode(mode);

        return;
    }

    if (key_code[BUT1] == 1 || key_code[BUT1] >= 20) {
        switch (mode)
        {
        case MODE2:
            if (green1_modify++ >= 99) {
                green1_modify = 1;
            }
            displayTime3( green1_modify, yellow1, green2, yellow2);
            UpdateBuffer(green1_modify, (int)2);

            break;

        case MODE3:
            if (yellow1_modify++ >= 99) {
                yellow1_modify = 1;
            }            
            displayTime3(green1, yellow1_modify, green2, yellow2);
            UpdateBuffer(yellow1_modify, (int)3);

            break;
        
        case MODE4:
            if (green2_modify++ >= 99) {
                green2_modify = 1;
            }
            displayTime3( green1,yellow1,green2_modify, yellow2);
            UpdateBuffer(green2_modify,(int)4);

            break;
        
        case MODE5:
            if (yellow2_modify++ >= 99) {
                yellow2_modify = 1;
            }
            UpdateBuffer(yellow2_modify, (int)5);
            displayTime3(green1, yellow1, green2, yellow2_modify);

            break;

        default:
            break;
        }

        return;
    }

    // --------- BUTTON2 WAS PRESSED -----------------
    if (key_code[BUT2] == 1 || key_code[BUT2] >= 20) {
        switch (mode)
        {
        case MODE2:
            if (green1_modify-- <= 1) {
                green1_modify = 99;
            }

            displayTime3(green1_modify, yellow1, green2, yellow2);
            UpdateBuffer(green1_modify,(int)2);

            break;

        case MODE3:
            if (yellow1_modify-- <= 1) {
                yellow1_modify = 99;
            }
            displayTime3(green1, yellow1_modify, green2, yellow2);
            UpdateBuffer(yellow1_modify,(int)3);

            break;
        
        case MODE4:
            if (green2_modify-- <= 1) {
                green2_modify = 99;
            }
            displayTime3(green1, yellow1, green2_modify, yellow2);
            UpdateBuffer(green2_modify, (int)4);

            break;
        
        case MODE5:
            if (yellow2_modify-- <= 1) {
                yellow2_modify = 99;
            }
            UpdateBuffer(yellow2_modify, (int)5);
            displayTime3(green1, yellow1, green2, yellow2_modify);

            break;

        default:
            break;
        }

        return;        
    }

    // --------- BUTTON3 WAS PRESSED -----------------
    if (key_code[BUT3] == 1 || key_code[BUT3] >= 20) {
        switch (mode)
        {
        case MODE2:
            green1 = green1_modify;

            LcdClearS();
            LcdPrintStringS(0,0,"DA SET XANH1");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(green1, yellow1, green2, yellow2);
            break;

        case MODE3:
            yellow1 = yellow1_modify;

            LcdClearS();
            LcdPrintStringS(0,0,"DA SET VANG1");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(green1, yellow1, green2, yellow2);
            break;
        
        case MODE4:
            green2 = green2_modify;
            LcdClearS();
            LcdPrintStringS(0,0,"DA SET XANH2");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(green1, yellow1, green2, yellow2);
            break;
        
        case MODE5:
            yellow2 = yellow2_modify;

            LcdClearS();
            LcdPrintStringS(0,0,"DA SET VANG2");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(green1, yellow1, green2, yellow2);
            break;

        default:
            break;
        }

        return;
    }

    // --------- BUTTON4 WAS PRESSED -----------------
    if (key_code[BUT4]) {
        return;
    }

    // --------- BUTTON5 WAS PRESSED -----------------
    if (key_code[BUT5]) {
        return;
    }

    // --------- BUTTON6 WAS PRESSED -----------------
    if (key_code[BUT6]) {
        return;
    }

    // --------- BUTTON7 WAS PRESSED -----------------
    if (key_code[BUT7]) {
        return;
    }
}

void mode_fsm() {
     switch(mode) {
        case MODE1 :
            traffic_fsm();           
            
            break;
        case MODE2 :
            if (flag_timer0) {
                LcdPrintStringS(0,0,"Mode 2 SET XANH1");
                ReverseOutputPortAo2(2);
                SetTimer0_ms(500);
            }

            break;
        case MODE3 :
            if (flag_timer0) {
                LcdPrintStringS(0,0,"Mode 3 SET VANG1");
                ReverseOutputPortAo2(1);
                SetTimer0_ms(500);
            }

            break;
        case MODE4 :
            if (flag_timer0) {
                LcdPrintStringS(0,0,"Mode 4 SET XANH2");
                ReverseOutputPortAo2(5);
                SetTimer0_ms(500);
            }

            break;
            
            case MODE5:
            if (flag_timer0) {
                LcdPrintStringS(0,0,"Mode 5 SET VANG2");
                ReverseOutputPortAo2(4);
                SetTimer0_ms(500);
            }

            break;   
    }
}


unsigned char isButtonUp()
{
    if (key_code[BUT0] == 1 || (key_code[BUT0] >= 10 && key_code[BUT0]%2 == 1))
        return 1;
    else
        return 0;
}
unsigned char isButtonDown()
{
    if (key_code[BUT1] == 1)
        return 1;
    else
        return 0;
}
void BaiTap_ChongRung()
{
    if (isButtonUp())
        numberOfPushButton ++;
    if (isButtonDown())
        numberOfPushButton --;
    LcdPrintStringS(0,0,"    ");
    LcdPrintNumS(0,0,numberOfPushButton);
}

void Test_KeyMatrix()
{
    int temp_i;
    for (temp_i=0; temp_i<=15; temp_i++)
    {
        if(key_code[temp_i] != 0)
            LcdPrintStringS(1,temp_i,"_");
        else
            LcdPrintStringS(1,temp_i," ");
    }
}