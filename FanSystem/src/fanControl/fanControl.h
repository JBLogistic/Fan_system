/*============================================================================*/
/*                               CAPGEMINI                                    */
/*============================================================================*/
/*                        OBJECT SPECIFICATION                                */
/*============================================================================*/
/*!
 * $Source: fanControl.h $
 * $Revision: 1 $
 * $Author: Bako Jozsef $
 * $Date: 2023.09.25 $
 */
/*============================================================================*/
/* DESCRIPTION :                                                              */
/** This file contains the variables and constants needed for fan control
*/
/*============================================================================*/
#ifndef FANCONTROL_H
#define FANCONTROL_H
/* Includes */
/*============================================================================*/
#include <Arduino.h>
/* Constants and types */
/*============================================================================*/
#define fan1pwm 14
#define fan1rpm 27
#define fan2pwm 13
#define fan2rpm 12
// update cycle in milliseconds
#define updateCycle 60000
#define PWM_RES 8
//Maximum revelations per minute value for the fans given by the manufacturer
#define MAXRPM 1700
//Minimum revelations per minute value for the fans given by the manufacturer
#define MINRPM 200
/* Exported Variables */
/*============================================================================*/
//fan speed in rotation per minute
static float rpm1 = 0.0;
static float rpm2 = 0.0;
//Coutns how many rotations did the fan motor made in a period of time
static int rotationCounter1 = 0;
//Coutns how many rotations did the fan motor made in a period of time
static int rotationCounter2 = 0;
// setting PWM properties
static volatile int pwmV1 = 0;
static volatile int pwmV2 = 0;
//Stores the previous time when  measuring happend
static unsigned long millisecondsLastFanMeasurement = 0;
/* Exported functions prototypes */
/*============================================================================*/
void initPWMfans();
void changeFanSpeed(int newPwmValue, int fanNumber);
void updateRPM();
static void rotation1();
static void rotation2();
float getRotation1();
float getRotation2();
int getPwm1();
int getPwm2();

#endif FANCONTROL_H