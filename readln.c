#include <stdio.h>
#include "pico/stdlib.h"
#include "readln.h"

const char   lfchar      = 0x0D;
const char   pwmchars    = 'w';
const char   pwmcharl    = 'W';
const char   adcchars    = 'a';
const char   adccharl    = 'A';
const char   startchars  = 's';
const char   startcharl  = 'S';
const char   stopchars   = 'e';
const char   stopcharl   = 'E';
const char   guichars    = 'g';
const char   guicharl    = 'G';
const char   applchars   = 'p';
const char   applcharl   = 'P';


//  boudble buffered
#define buflen  64
static char     buf0[buflen];
static char     buf1[buflen];
static char     *bp, *br;

static void bufcopy(char *src, char *dst);
static char *swapAndResetPointer(void);

void    initReadln(void)
{
    bp = buf0;
    br = buf1;
}

char    *readln(void)
{
    int c = getchar_timeout_us(0);
    if (c != PICO_ERROR_TIMEOUT) {
        if (c != lfchar) {
            *bp++ = c;
        } else {
            *bp = '\0';
            return swapAndResetPointer();
        }
    }
    return NULL;
}

int32_t uintFromHex(char *hex) {
    char        c;
    uint32_t    result = 0;
    if ((hex == NULL) || (*hex == '\n'))
        return -1;

    while ((c = *(hex ++)) != '\0') {
        result <<= 4;
        uint32_t    tmp = 0;
        if (('0' <= c) && (c <= '9'))
            tmp = c - '0';
        else if (('a' <= c) && (c <= 'f'))
            tmp = c - 'a' + 10;
        else if (('A' <= c) && (c <= 'F'))
            tmp = c - 'A' + 10;
        else 
            result >>= 4;   //  if ignored, put back for one hex char
        result += tmp;
    }
    return result;
}

bool    isForPWM(char c)
{
    return (c == pwmchars) || (c == pwmcharl);
}

bool    isForADC(char c)
{
    return (c == adcchars) || (c == adccharl);
}

bool    isForStart(char c)
{
    return (c == startchars) || (c == startcharl);
}

bool    isForStop(char c)
{
    return (c == stopchars) || (c == stopcharl);
}

bool    isForGUI(char c)
{
    return (c == guichars) || (c == guicharl);
}

bool    isForAppl(char c)
{
    return (c == applchars) || (c == applcharl);
}


static void bufcopy(char *src, char *dst)
{
    while (*src != '\0')
        *(dst ++) = *(src ++);
    *dst = '\0';
}

static char *swapAndResetPointer()
{
    if (br == buf0) {
        bp = buf0;
        br = buf1;
    } else {
        bp = buf1;
        br = buf0;
    }
    return br;
}