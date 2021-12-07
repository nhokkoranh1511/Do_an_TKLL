/* 
 * File:   seg7.h
 * Author: HP
 *
 * Created on November 26, 2021, 10:43 PM
 */

#ifndef SEG7_H
#define	SEG7_H
#include <p18f4620.h>

void display7SEG(int num);
void update7SEG(int index);
void display7SEG2(int num);
void UpdateBuffer();
void ledScan();// quét led s? d?ng rd3-rd6
void resetScan(); 





#endif	/* SEG7_H */

