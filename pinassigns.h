#ifndef PINASSIGNS_H
#define PINASSIGNS_H
#include <stdio.h>


//  implicit conversion uint32_t and float
typedef union {
    uint32_t    ui;
    float       fl;
} uifl;

static const uint       pwmpin  = 15;   //  PWM output
static const uint       adcpin  = 1;    //  ADC input, GP27 pin

//  wrap value for PWM, register TOP
//  bit order is the same as ADC resolution
#define ADRESOLUTION    12
#define WRAPBITS        ADRESOLUTION
static const uint16_t   wrap    = 1 << WRAPBITS;
static const uint16_t   wrapmsk = (1 << WRAPBITS) - 1;

#endif  //PINASSIGNS_H
