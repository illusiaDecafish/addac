#ifndef READLN_H
#define READLN_H


//  read char from stdin until LF char receives
//  readln() returns a pointer to a line buffer
void    initReadln(void);
char    *readln(void);


/*  character command:
    cxxxx\n
    c: 'w' or 'a'
        'w' for PWM level,
        'a' for ADC over sampling count
    xxxx:   hex characters for its argument
    example:
        w100\n  PWM level register value is set to 0x1000
        afff\n  over-sampling count for ADC is set to 0xFFF
*/

bool    isForPWM(char c);
bool    isForADC(char c);
bool    isForStart(char c);
bool    isForStop(char c);
bool    isForGUI(char c);
bool    isForAppl(char c);


//  utility function hex chars to uint32_t value
int32_t uintFromHex(char *hex);

#endif  //  READLN_H
    
