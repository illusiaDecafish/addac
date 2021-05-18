#include <stdio.h>
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "pinassigns.h"
#include "core1.h"
#include "readln.h"


//  on-board LED will blink by a peculiar stype to notify this code working. 
static uint     ledgpio = 25;   //  GPIO number of LED on board

//  call back for a repeating timer to blink LED
static bool timercb(repeating_timer_t *rt);

static uint slice;
static uint chan;

static void initCore0(void);
static void core0loop(void);

int main(void)
{
    stdio_init_all();
    //stdio_set_translate_crlf(&stdio_usb, true);
    
    initCore0();
    multicore_launch_core1(core1loop);
    core0loop();
}


//  core0 handles PWM output

static void initCore0(void)
{
    gpio_set_function(pwmpin, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(pwmpin);
    chan  = pwm_gpio_to_channel(pwmpin);
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, chan, pwmpin);
    pwm_set_phase_correct(slice, false);    //  no phase adjust
    //  pwm_set_clkdiv(slice, 2.0f);

    gpio_init(ledgpio);
    gpio_set_dir(ledgpio, true);
}

static void core0loop(void) {
    initReadln();
    pwm_set_enabled(slice, true);
    
    struct repeating_timer rt;
    uint32_t    onoff = 0;
    add_repeating_timer_ms(100, timercb, &onoff, &rt);

    uint32_t    count = 0;
    int32_t     lastms = 0;
    bool        running = false;
    bool        guiMode = true;

    while (true) {
        char    *bp;
        bool    yn;
        if ((bp = readln()) != NULL) {
            if (guiMode)
                printf("\nread one line: %s\n", bp);
            int32_t arg = uintFromHex(bp + 1);
            if (isForADC(*bp)) {
                if (arg >= 0)
                    yn = multicore_fifo_push_timeout_us((uint32_t)arg, 0);
                else if (guiMode) {
                    printf("\n%s command needs argument\n");
                }
            }
            else if (isForPWM(*bp)) {
                if (arg >= 0)
                    pwm_set_chan_level(slice, chan, (uint16_t)arg);
                else if (guiMode) {
                    printf("\n%s command needs argument\n");
                }
            }

            else if (isForStart(*bp)) {
                running = true;
            }
            else if (isForStop(*bp)) {
                running = false;
            }
            else if (isForGUI(*bp)) {
                guiMode = true;
            }
            else if (isForAppl(*bp)) {
                guiMode = false;
            }
            else if (guiMode)
                printf("\nInvalid command line: %s\n", bp);
        }

        uint32_t    out;
        uifl        uf;
        if (multicore_fifo_pop_timeout_us(0, &out) && running) {
        //  if not running, incomming fifo data are just ignored
           uf.ui = out;
            uint32_t    mxmn = multicore_fifo_pop_blocking();
            uint32_t    errorCount = multicore_fifo_pop_blocking();
            uint32_t    max = (mxmn >> 16) & 0xFFFF;
            uint32_t    min = mxmn & 0xFFFF;
            absolute_time_t at = get_absolute_time();
            uint32_t    ms = to_ms_since_boot(at);
            if (guiMode)
                printf("A/D readout:0x%03X [0x%03X-0x%03X E:%05x] priod:%4dmsec [%6d:%7.3f sec]\r",
                    out, min, max, errorCount, ms - lastms, count, ms * 0.001);
            else 
                printf("%\t%u\t%u\t%u\t%u\t%u\n",
                    out, min, max, errorCount, ms, count);
            lastms = ms;
            count ++;
        }
    }
}

static bool timercb(repeating_timer_t *rt) {
    uint32_t    *onoff = (uint32_t *)(rt->user_data);
    bool        nf = (((*onoff) & 0x0008) != 0)
                    && (((*onoff) & 0x0001) != 0);
    gpio_put(ledgpio, nf);
    (*onoff) ++;
}


