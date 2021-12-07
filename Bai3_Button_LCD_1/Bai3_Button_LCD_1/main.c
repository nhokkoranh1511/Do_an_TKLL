#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Noi khai bao hang so
#define BUT5                4
#define BUT6                5
#define BUT7                6 
#define BUT8                7

#define BUT9                8
#define BUT10               9
#define BUT11               10
#define BUT12               11


#define RED_NORMAL          5
#define YELLOW_NORMAL       2
#define GREEN_NORMAL        3

#define RED_CAODIEM         7
#define YELLOW_CAODIEM      2
#define GREEN_CAODIEM       5

#define RED_THAPDIEM        10
#define YELLOW_THAPDIEM     2
#define GREEN_THAPDIEM      8

enum TRAFFIC_STATE {
            GREEN_1,  GREEN_2,
            YELLOW_1, YELLOW_2
};

#define NORMAL              1
#define MODIFY_GREEN_1      2
#define MODIFY_YELLOW_1     3
#define MODIFY_GREEN_2      4
#define MODIFY_YELLOW_2     5
#define CAO_DIEM            6
#define THAP_DIEM           7
#define ONLY_RED            8
#define ONLY_YELLOW         9
#define ONLY_GREEN          10
#define FLASH_YELLOW        11

// Noi khai bao bien toan cuc
int mode                    = NORMAL;

int GREEN1_DURATION         = GREEN_NORMAL;
int GREEN2_DURATION         = GREEN_NORMAL;
int YELLOW1_DURATION        = YELLOW_NORMAL;
int YELLOW2_DURATION        = YELLOW_NORMAL;

int green1_modify           = GREEN_NORMAL;
int green2_modify           = GREEN_NORMAL;
int yellow1_modify          = YELLOW_NORMAL;
int yellow2_modify          = YELLOW_NORMAL;

enum TRAFFIC_STATE states  = GREEN_2;
int time_in_state   = 0;

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
void modify_fsm();
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
            modify_fsm();
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
    
    time_in_state           = 0;
    
    switch (mode)
    {
    case NORMAL:
        LcdPrintStringS(0,0,"NORMAL DO XANH");
        break;
    
    case CAO_DIEM:
        LcdPrintStringS(0,0,"C.DIEM DO XANH");

        GREEN1_DURATION     = GREEN_CAODIEM;
        GREEN2_DURATION     = GREEN_CAODIEM;
        YELLOW1_DURATION    = YELLOW_CAODIEM;
        YELLOW2_DURATION    = YELLOW_CAODIEM;
        break;

    case THAP_DIEM:
        LcdPrintStringS(0,0,"T.DIEM DO XANH");

        GREEN1_DURATION     = GREEN_THAPDIEM;
        GREEN2_DURATION     = GREEN_THAPDIEM;
        YELLOW1_DURATION    = YELLOW_THAPDIEM;
        YELLOW2_DURATION    = YELLOW_THAPDIEM;
        break;        
            
    default:
        LcdPrintStringS(0,0,"NORMAL DO XANH");

        GREEN1_DURATION     = GREEN_NORMAL;
        GREEN2_DURATION     = GREEN_NORMAL;
        YELLOW1_DURATION    = YELLOW_NORMAL;
        YELLOW2_DURATION    = YELLOW_NORMAL;
        break;
    }

    green1_modify       = GREEN1_DURATION;
    yellow1_modify      = YELLOW1_DURATION;
    green2_modify       = GREEN2_DURATION;
    yellow2_modify      = YELLOW2_DURATION;

    states = GREEN_2;
    
    SetTimer0_ms(1000);
    setPortAo2(~0b100001);
    
    displayTime(GREEN2_DURATION + YELLOW2_DURATION - time_in_state, 
                GREEN2_DURATION - time_in_state);
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
                
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"NORMAL DO XANH");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"C.DIEM DO XANH");
                    break;

                case THAP_DIEM:
                    LcdPrintStringS(0,0,"T.DIEM DO XANH");
                    break;
                
                default:
                    break;
                }

                displayTime(GREEN2_DURATION + YELLOW2_DURATION - time_in_state, 
                            GREEN2_DURATION - time_in_state);
                
                if (time_in_state == GREEN2_DURATION)
                {
                    
                    setPortAo2(~0b010001);
                    states = YELLOW_2;
                    time_in_state = 0;
                    
                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"NORMAL DO VANG");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"C.DIEM DO VANG");
                        break;

                    case THAP_DIEM:
                        LcdPrintStringS(0,0,"T.DIEM DO VANG");
                        break;
                    
                    default:
                        break;
                    }
                    displayTime(YELLOW2_DURATION - time_in_state, 
                                YELLOW2_DURATION - time_in_state);
                }
            }

            break;
        }
        case YELLOW_2: {         
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0 = 0;
                
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"NORMAL DO VANG");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"C.DIEM DO VANG");
                    break;

                case THAP_DIEM:
                    LcdPrintStringS(0,0,"T.DIEM DO VANG");
                    break;
                
                default:
                    break;
                }

                displayTime(YELLOW2_DURATION - time_in_state, 
                            YELLOW2_DURATION - time_in_state);

                if (time_in_state == YELLOW2_DURATION)
                {                
                    time_in_state = 0;
                    states = GREEN_1;
                    
                    setPortAo2(~0b001100);

                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"NORMAL XANH DO");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"C.DIEM XANH DO");
                        break;

                    case THAP_DIEM:
                        LcdPrintStringS(0,0,"T.DIEM XANH DO");
                        break;
                    
                    default:
                        break;
                    }
                    displayTime(GREEN1_DURATION - time_in_state, 
                                GREEN1_DURATION + YELLOW1_DURATION - time_in_state);
                    }
            }

            break;
        }
            case GREEN_1: {
            
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0=0;
                    
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"NORMAL XANH DO");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"C.DIEM XANH DO");
                    break;

                case THAP_DIEM:
                    LcdPrintStringS(0,0,"T.DIEM XANH DO");
                    break;
                
                default:
                    break;
                }
                displayTime(GREEN1_DURATION - time_in_state, 
                            GREEN1_DURATION + YELLOW1_DURATION - time_in_state);
                
                if (time_in_state == GREEN1_DURATION)
                {
                    setPortAo2(~0b001010);
                    states=YELLOW_1;
                    time_in_state=0;
                    
                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"NORMAL VANG DO");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"C.DIEM VANG DO");
                        break;

                    case THAP_DIEM:
                        LcdPrintStringS(0,0,"T.DIEM VANG DO");
                        break;
                    
                    default:
                        break;
                    }
                    displayTime(YELLOW1_DURATION - time_in_state, 
                                YELLOW1_DURATION - time_in_state);
                    }
            }

            break;
        }
            
            case YELLOW_1: {
        
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0 = 0;

                    
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"NORMAL VANG DO");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"C.DIEM VANG DO");
                    break;

                case THAP_DIEM:
                    LcdPrintStringS(0,0,"T.DIEM VANG DO");
                    break;
                
                default:
                    break;
                }
                displayTime(YELLOW1_DURATION - time_in_state, 
                            YELLOW1_DURATION - time_in_state);
                
                if (time_in_state == YELLOW1_DURATION)
                {
                
                    time_in_state = 0;
                    states = GREEN_2;
                    
                    setPortAo2(~0b100001);
                
                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"NORMAL DO XANH");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"C.DIEM DO XANH");
                        break;

                    case THAP_DIEM:
                        LcdPrintStringS(0,0,"T.DIEM DO XANH");
                        break;
                    
                    default:
                        break;
                    }
                    displayTime(GREEN2_DURATION + YELLOW2_DURATION - time_in_state, 
                                GREEN2_DURATION - time_in_state);
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

void setting_when_changing_mode(int changed_mode) {
    switch (changed_mode)
    {
    case NORMAL:
        setPortAo2(~0b0);
        init_traffic();
        break;

    case MODIFY_GREEN_1:
        LcdClearS();
        LcdPrintStringS(0,0,"MODIFY DEN XANH1");
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        UpdateBuffer(GREEN1_DURATION, GREEN1_DURATION);
        setPortAo2(~0b0);
        
        break;

    case MODIFY_YELLOW_1:
        LcdClearS();
        LcdPrintStringS(0,0,"MODIFY DEN VANG1");
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        setPortAo2(~0b0);
        UpdateBuffer(YELLOW1_DURATION, YELLOW1_DURATION);
        
        break;

    case MODIFY_GREEN_2:
        LcdClearS();
        LcdPrintStringS(0,0,"MODIFY DEN XANH2");
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        setPortAo2(~0b0);
        UpdateBuffer(GREEN2_DURATION, GREEN2_DURATION);
        
        break;
    
    case MODIFY_YELLOW_2:
        LcdClearS();
        LcdPrintStringS(0,0,"MODIFY DEN VANG2");
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        setPortAo2(~0b0);
        UpdateBuffer(YELLOW2_DURATION, GREEN2_DURATION);
        
        break;

    case CAO_DIEM:
        setPortAo2(~0b0);
        init_traffic();
        break;

    case THAP_DIEM:
        setPortAo2(~0b0);
        init_traffic();
        break;

    case FLASH_YELLOW:
        LcdClearS();
        LcdPrintStringS(0,0,"CANH BAO");
        UpdateBuffer((int)0, (int)0);
        setPortAo2(~0b0);
        break;
    
    case ONLY_RED:
        LcdClearS();
        LcdPrintStringS(0,0,"DEN DO");
        UpdateBuffer((int)0, (int)0);
        setPortAo2(~0b0);
        break;
    
    case ONLY_YELLOW:
        LcdClearS();
        LcdPrintStringS(0,0,"DEN VANG");
        UpdateBuffer((int)0, (int)0);
        setPortAo2(~0b0);
        break;
    
    case ONLY_GREEN:
        LcdClearS();
        LcdPrintStringS(0,0,"DEN XANH");
        UpdateBuffer((int)0, (int)0);
        setPortAo2(~0b0);
        break;

    default:
        break;
    }
}

void button_processing() {
    // --------- BUTTON9 WAS PRESSED -----------------
    if (key_code[BUT9] == 1 || key_code[BUT9] >= 20) {
        if (mode == CAO_DIEM) {
            mode = THAP_DIEM;
        } 
        else if (mode == THAP_DIEM) {
            mode = FLASH_YELLOW;
        }
        else if (mode == FLASH_YELLOW) {
            mode = CAO_DIEM;
        }
        else {
            mode = CAO_DIEM;
        }

        setting_when_changing_mode(mode);

        return;
    }

    // --------- BUTTON10 WAS PRESSED -----------------
    if (key_code[BUT10] == 1 || key_code[BUT10] >= 20) {
        mode = ONLY_RED;

        setting_when_changing_mode(mode);

        return;
    }

    // --------- BUTTON11 WAS PRESSED -----------------
    if (key_code[BUT11] == 1 || key_code[BUT11] >= 20) {
        mode = ONLY_YELLOW;

        setting_when_changing_mode(mode);

        return;
    }

    // --------- BUTTON10 WAS PRESSED -----------------
    if (key_code[BUT12] == 1 || key_code[BUT12] >= 20) {
        mode = ONLY_GREEN;

        setting_when_changing_mode(mode);

        return;
    }

    // --------- BUTTON5 WAS PRESSED -----------------
    if (key_code[BUT5] == 1 || key_code[BUT5] >= 20) {
        if (mode == MODIFY_YELLOW_2) {
            mode = NORMAL;

            
        } else if (mode > MODIFY_YELLOW_2) {
            mode = NORMAL;
            GREEN1_DURATION     = GREEN_NORMAL;
            GREEN2_DURATION     = GREEN_NORMAL;
            YELLOW1_DURATION    = YELLOW_NORMAL;
            YELLOW2_DURATION    = YELLOW_NORMAL;
        }
        else {
            mode++;
        }

        setting_when_changing_mode(mode);

        return;
    }

    // --------- BUTTON6 WAS PRESSED -----------------
    if (key_code[BUT6] == 1 || key_code[BUT6] >= 20) {
        switch (mode)
        {
        case MODIFY_GREEN_1:
            if (green1_modify++ >= 99) {
                green1_modify = 1;
            }
            displayTime3(   green1_modify, YELLOW1_DURATION, 
                            GREEN2_DURATION, YELLOW2_DURATION);
            UpdateBuffer(green1_modify, green1_modify);

            break;

        case MODIFY_YELLOW_1:
            if (yellow1_modify++ >= 99) {
                yellow1_modify = 1;
            }            
            displayTime3(   GREEN1_DURATION, yellow1_modify, 
                            GREEN2_DURATION, YELLOW2_DURATION);
            UpdateBuffer(yellow1_modify, yellow1_modify);

            break;
        
        case MODIFY_GREEN_2:
            if (green2_modify++ >= 99) {
                green2_modify = 1;
            }
            displayTime3(   GREEN1_DURATION,YELLOW1_DURATION,
                            green2_modify, YELLOW2_DURATION);
            UpdateBuffer(green2_modify,green2_modify);

            break;
        
        case MODIFY_YELLOW_2:
            if (yellow2_modify++ >= 99) {
                yellow2_modify = 1;
            }
            UpdateBuffer(yellow2_modify, yellow2_modify);
            displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                            GREEN2_DURATION, yellow2_modify);

            break;

        default:
            break;
        }

        return;
    }

    // --------- BUTTON7 WAS PRESSED -----------------
    if (key_code[BUT7] == 1 || key_code[BUT7] >= 20) {
        switch (mode)
        {
        case MODIFY_GREEN_1:
            if (green1_modify-- <= 1) {
                green1_modify = 99;
            }

            displayTime3(   green1_modify, YELLOW1_DURATION, 
                            GREEN2_DURATION, YELLOW2_DURATION);
            UpdateBuffer(green1_modify,green1_modify);

            break;

        case MODIFY_YELLOW_1:
            if (yellow1_modify-- <= 1) {
                yellow1_modify = 99;
            }
            displayTime3(   GREEN1_DURATION, yellow1_modify, 
                            GREEN2_DURATION, YELLOW2_DURATION);
            UpdateBuffer(yellow1_modify,yellow1_modify);

            break;
        
        case MODIFY_GREEN_2:
            if (green2_modify-- <= 1) {
                green2_modify = 99;
            }
            displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                            green2_modify, YELLOW2_DURATION);
            UpdateBuffer(green2_modify, green2_modify);

            break;
        
        case MODIFY_YELLOW_2:
            if (yellow2_modify-- <= 1) {
                yellow2_modify = 99;
            }
            UpdateBuffer(yellow2_modify, yellow2_modify);
            displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                            GREEN2_DURATION, yellow2_modify);

            break;

        default:
            break;
        }

        return;        
    }

    // --------- BUTTON8 WAS PRESSED -----------------
    if (key_code[BUT8] == 1 || key_code[BUT8] >= 20) {
        switch (mode)
        {
        case MODIFY_GREEN_1:
            GREEN1_DURATION = green1_modify;
            green1_modify = GREEN1_DURATION;

            LcdClearS();
            LcdPrintStringS(0,0,"DA THAY DOI");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(GREEN1_DURATION, YELLOW1_DURATION, GREEN2_DURATION, YELLOW2_DURATION);
            break;

        case MODIFY_YELLOW_1:
            YELLOW1_DURATION = yellow1_modify;
            yellow1_modify = YELLOW1_DURATION;

            LcdClearS();
            LcdPrintStringS(0,0,"DA THAY DOI");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(GREEN1_DURATION, YELLOW1_DURATION, GREEN2_DURATION, YELLOW2_DURATION);
            break;
        
        case MODIFY_GREEN_2:
            GREEN2_DURATION = green2_modify;
            green2_modify = GREEN2_DURATION;

            LcdClearS();
            LcdPrintStringS(0,0,"DA THAY DOI");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(GREEN1_DURATION, YELLOW1_DURATION, GREEN2_DURATION, YELLOW2_DURATION);
            break;
        
        case MODIFY_YELLOW_2:
            YELLOW2_DURATION = yellow2_modify;
            yellow2_modify = YELLOW2_DURATION;

            LcdClearS();
            LcdPrintStringS(0,0,"DA THAY DOI");
            SetTimer0_ms(1000);//delay hieu ung
            displayTime3(GREEN1_DURATION, YELLOW1_DURATION, GREEN2_DURATION, YELLOW2_DURATION);
            break;

        default:
            break;
        }

        return;
    }
}

void modify_fsm() {
    switch(mode) {
        case NORMAL :
            traffic_fsm();           
            
            break;
        case MODIFY_GREEN_1 :
            if (flag_timer0) {
                LcdPrintStringS(0,0,"MODIFY DEN XANH1");
                ReverseOutputPortAo2(2);
                SetTimer0_ms(500);
            }

            break;
        case MODIFY_YELLOW_1 :
            if (flag_timer0) {
                LcdPrintStringS(0,0,"MODIFY DEN VANG1");
                ReverseOutputPortAo2(1);
                SetTimer0_ms(500);
            }

            break;
        case MODIFY_GREEN_2 :
            if (flag_timer0) {
                LcdPrintStringS(0,0,"MODIFY DEN XANH2");
                ReverseOutputPortAo2(5);
                SetTimer0_ms(500);
            }
            break;
            
        case MODIFY_YELLOW_2:
            if (flag_timer0) {
                LcdPrintStringS(0,0,"MODIFY DEN VANG2");
                ReverseOutputPortAo2(4);
                SetTimer0_ms(500);
            }
            break;

        case CAO_DIEM:
            traffic_fsm();
            break;

        case THAP_DIEM:
            traffic_fsm();
            break;

        case FLASH_YELLOW:
            if (flag_timer0) {
                LcdClearS();
                LcdPrintStringS(0,0,"CANH BAO");
                Flash_yellow();
                SetTimer0_ms(500);
            }

        case ONLY_RED:
            if (flag_timer0) {
                LcdClearS();
                LcdPrintStringS(0,0,"DEN DO");
                CloseOutputPortAo2(0);
                CloseOutputPortAo2(3);
                SetTimer0_ms(500);
            }
        
        case ONLY_YELLOW:
            if (flag_timer0) {
                LcdClearS();
                LcdPrintStringS(0,0,"DEN VANG");
                CloseOutputPortAo2(1);
                CloseOutputPortAo2(4);
                SetTimer0_ms(500);
            }

        case ONLY_GREEN:
            if (flag_timer0) {
                LcdClearS();
                LcdPrintStringS(0,0,"DEN XANH");
                CloseOutputPortAo2(2);
                CloseOutputPortAo2(5);
                SetTimer0_ms(500);
            }
        
        default:
            traffic_fsm();   
            break;
    }
}


unsigned char isButtonUp()
{
    if (key_code[BUT5] == 1 || (key_code[BUT5] >= 10 && key_code[BUT5]%2 == 1))
        return 1;
    else
        return 0;
}
unsigned char isButtonDown()
{
    if (key_code[BUT6] == 1)
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