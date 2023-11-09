/*============================================================================*/
/*                               CAPGEMINI                                    */
/*============================================================================*/
/*                        OBJECT SPECIFICATION                                */
/*============================================================================*/
/*!
 * $Source: fanControl.c $
 * $Revision: 1 $
 * $Author: Bako Jozsef $
 * $Date: 2023.09.25 $
 */
/*============================================================================*/
/* DESCRIPTION :                                                              */
/** 
 *  Here are implemented the fan control functions and the fan RPM measuring functions
*/
/* Includes */
/*============================================================================*/
#include "fanControl.h"

/* Private functions */
/*============================================================================*/
//Count how many rotations does a fan during a period of time
void rotation1(){
    rotationCounter1++;
}
//Count how many rotations does a fan during a period of time
void rotation2(){
    rotationCounter2 = rotationCounter2 + 1;
}
/* Exported functions */
/*============================================================================*/
//Initialize the fans
void initPWMfans(){
    pinMode(fan1pwm,OUTPUT);
    pinMode(fan2pwm,OUTPUT);
    pinMode(fan1rpm,INPUT);
    pinMode(fan2rpm,INPUT);
    ledcSetup(1, 5000, PWM_RES);
    ledcSetup(2, 5000, PWM_RES);
    ledcAttachPin(fan1pwm, 1);
    ledcAttachPin(fan2pwm, 2);
    ledcWrite(1, pwmV1);
    ledcWrite(2, pwmV2);
    attachInterrupt(digitalPinToInterrupt(fan1rpm), rotation1 ,RISING);
    attachInterrupt(digitalPinToInterrupt(fan2rpm), rotation2 ,RISING);
}

//Update the speed of fans 
void changeFanSpeed(int newPwmValue, int fanNumber){
    if(fanNumber == 1){
        ledcWrite(1, newPwmValue);
        pwmV1 = newPwmValue;
    }
    if(fanNumber == 2){
        ledcWrite(2, newPwmValue);
        pwmV2 = newPwmValue;
    }
}
//Updates the revelations per minute variables 
void updateRPM(){ 
    // detach interrupt while calculating rpm
    detachInterrupt(digitalPinToInterrupt(fan1rpm)); 
    detachInterrupt(digitalPinToInterrupt(fan2rpm)); 
    // calculate rpm
    rpm1 = 1700 * (255 - pwmV1) / 255;
    rpm2 = 1700 * (255 - pwmV2) / 255;
    //Tresholding in case an unexpected value appears
    if(rpm1 < MINRPM){
        rpm1 = MINRPM;
    }
    if(rpm1 > MAXRPM){
        rpm1 = MAXRPM;
    }
    if(rpm2 < MINRPM){
        rpm2 = MINRPM;
    }
    if(rpm2 > MAXRPM){
        rpm2 = MAXRPM;
    }
    //Printing the values for debugging purposes
    Serial.println(rpm1);
    Serial.println(rpm2);
    Serial.println(rotationCounter2);
    Serial.println(rotationCounter1);
    // reset counters
    rotationCounter1 = 0;
    rotationCounter2 = 0;  
    // store milliseconds when tacho was measured the last time
    millisecondsLastFanMeasurement = millis();
    // attach interrupt again
    attachInterrupt(digitalPinToInterrupt(fan1rpm), rotation1, FALLING);
    attachInterrupt(digitalPinToInterrupt(fan2rpm), rotation2, FALLING);
  
}
//return the revelation per minute variables for the outside fan
float getRotation1(){
    return rpm1;
}
//return the revelation per minute variables for the inside fan
float getRotation2(){
    return rpm2;
}
//return the pwm variables for the outside fan
int getPwm1(){
    return pwmV1;
}
//return the pwm variables for the inside fan
int getPwm2(){
    return pwmV2;
}