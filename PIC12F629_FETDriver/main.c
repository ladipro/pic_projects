/*
 * FET driver
 * https://github.com/ladipro/pic_projects/wiki/FET-driver
 *
 * Target microcontroller: PIC12F629
 *
 */
#include <htc.h>

#if defined(WDTE_OFF)
__CONFIG(WDTE_OFF & LVP_OFF);
#elif defined (WDTDIS)
__CONFIG(WDTDIS & LVPDIS);
#endif

#define _XTAL_FREQ 4000000
#pragma config config WDTE=OFF,PWRTE=OFF,CP=OFF,BOREN=OFF,MCLRE=OFF,CPD=OFF,FOSC=INTRCIO

int main(void) {
    // make GP1 an output
    TRISIO1 = 0;

    // set up timer1
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1CKPS1 = 0; // 1:1 pre-scaler
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;

    unsigned short pwm_raw = 1000;
    unsigned short skip_counter = 0;
    while (1) {
        while (!GP3);
        T1CONbits.TMR1ON = 0;
        unsigned short pulse = TMR1;
        T1CONbits.TMR1ON = 1;
        while (GP3);
        T1CONbits.TMR1ON = 0;
        pulse = TMR1 - pulse;
        T1CONbits.TMR1ON = 1;

        // step 1 - dampen changes
        if (pulse > pwm_raw) {
            pwm_raw += pwm_raw / 128;
            if (pwm_raw > pulse) {
                pwm_raw = pulse;
            }
        } else {
            pwm_raw -= pwm_raw / 128;
            if (pwm_raw < pulse) {
                pwm_raw = pulse;
            }
        }

        if (skip_counter > 0) {
            // implements flashing, see below
            skip_counter--;
        } else {
            // step 2 - quantize to get rid of noise
            unsigned short pwm = pwm_raw - 1050;
            pwm /= 4;

            if (pwm <= 10) {
                // always on
                GP1 = 1;
            }
            else if (pwm <= 70) {
                // convert pwm to delay ~N^2
                pwm = 70 - pwm;
                unsigned short delay = pwm * ((pwm + 2) / 2);
                GP1 = 1;
                while (delay--) {
                    // nop
                }
                GP1 = 0;
            } else if (pwm <= 80) {
                // always off
                GP1 = 0;
            } else if (pwm <= 120) {
                // flashing, variable frequency
                unsigned short delay = 1800;
                GP1 = 1;
                while (delay--) {
                    // nop
                }
                GP1 = 0;
                skip_counter = (120 - pwm);
            }
        }
    }
    return 0;
}
