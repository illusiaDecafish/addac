#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/multicore.h"

#include "core1.h"

static const float conversion_factor = 3.3f / (1 << ADRESOLUTION);

static void    initCore1(void)
{
    adc_init();
    adc_select_input(adcpin);
    adc_fifo_setup(true, false, 0, true, false);
}

void    core1loop(void) {
    initCore1();

    adc_run(true);  //  start free run
    
    uint32_t    maxCount = 0x01000;
    uint32_t    currentMaxCount = maxCount;
    //float       cmc = conversion_factor / currentMaxCount;
    while (true) {
        uint32_t    errorCount = 0;
        uint32_t    dataCount = 0;
        uint32_t    count = 0;
        uint32_t    sum = 0;
        uint16_t    max = 0;
        uint16_t    min = 0xFFFF;
        for (count = 0 ; count < currentMaxCount ; count ++) {
            uint16_t tmp = adc_fifo_get_blocking();
            if (tmp & 0x8000 != 0) {
                errorCount ++;
            } else {
                dataCount ++;
                sum += tmp;
                if (tmp < min)
                    min = tmp;
                else if (max < tmp)
                    max = tmp;
            }
            uint32_t    out;
            if (multicore_fifo_pop_timeout_us(0, &out)) {
                maxCount = out;
            }
        }
/*
        uifl    uf;
        uf.fl = sum * conversion_factor / dataCount;
        //  average value in the first 32bits in FIFO
        bool yn = multicore_fifo_push_timeout_us(uf.ui, 0);
*/
        bool yn = multicore_fifo_push_timeout_us(sum / dataCount, 0);
        //  min and max raw values in the second in FIFO
        yn = multicore_fifo_push_timeout_us((max << 16) | min, 0);
        //  error count in the third
        yn = multicore_fifo_push_timeout_us(errorCount, 0);
        currentMaxCount = maxCount;
        //cmc = conversion_factor / currentMaxCount;
    }
}
