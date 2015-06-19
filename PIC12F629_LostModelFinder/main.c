/*
 * Lost model finder hack
 * https://github.com/ladipro/pic_projects/wiki/Lost-model-finder-hack
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

#define PWM_DEADBAND 50
#define MIN_SAMPLES_OUT_OF_DEADBAND 5
#define TIMEOUT_LIMIT_IN_SECONDS 900
#define NODATA_LIMIT_IN_SECONDS 15

volatile union { // shadow copy of GPIO
unsigned port;
    struct {
    unsigned GP0 : 1;
    unsigned GP1 : 1;
    unsigned GP2 : 1;
    unsigned GP3 : 1;
    unsigned GP4 : 1;
    unsigned GP5 : 1;
    };
} sGPIO;

volatile enum _GLOBAL_STATE {
    OFF,
    ON_EXPLICIT,
    ON_TIMEOUT,
    ON_NODATA,
} sGLOBAL_STATE = OFF;

volatile unsigned short sLAST_PWM_US = 0;
volatile unsigned short sSAMPLES_OUT_OF_DEADBAND = 0;
volatile unsigned long sTIMEOUT_COUNTER = 0;
volatile unsigned long sNODATA_COUNTER = 0;

int process_pwm_change(unsigned short pwm) {
    unsigned short diff;
    if (pwm > sLAST_PWM_US) {
        diff = pwm - sLAST_PWM_US;
    } else {
        diff = sLAST_PWM_US - pwm;
    }
    if (diff >= PWM_DEADBAND) {
        // don't reset the timeout counters right away - wait for
        // at least a few samples with big enough diff
        if (++sSAMPLES_OUT_OF_DEADBAND >= MIN_SAMPLES_OUT_OF_DEADBAND) {
            sSAMPLES_OUT_OF_DEADBAND = 0;
            sTIMEOUT_COUNTER = 0;
            sLAST_PWM_US = pwm;
        }
    } else {
        sSAMPLES_OUT_OF_DEADBAND = 0;
        sLAST_PWM_US = pwm;
    }
    sNODATA_COUNTER = 0;
}

void interrupt tc_int(void) {
    if (INTCONbits.GPIF) {
        int pwm_on = (GPIO & 0x08);
        if (pwm_on) {
            if (sTIMEOUT_COUNTER < 1000000 * TIMEOUT_LIMIT_IN_SECONDS) {
                sTIMEOUT_COUNTER += (unsigned)TMR1;
            }
            // reset the timer
            TMR1H = 0;
            TMR1L = 0;
        } else {
            unsigned short pwm_us = TMR1;
            if (sLAST_PWM_US & 0x8000) {
                // ignore this one, we just re-enabled interrupts
                sLAST_PWM_US &= 0x7FFF;
            } else if (pwm_us > 2500 || pwm_us < 500) {
                // this is odd, do nothing
            } else {
                process_pwm_change(pwm_us);
                if (pwm_us < 1250) {
                    sGLOBAL_STATE = ON_EXPLICIT;
                    sTIMEOUT_COUNTER = 0;
                } else if (sTIMEOUT_COUNTER >= 1000000 * TIMEOUT_LIMIT_IN_SECONDS) {
                    sGLOBAL_STATE = ON_TIMEOUT;
                } else {
                    sGLOBAL_STATE = OFF;
                }
            }
        }
        INTCONbits.GPIF = 0;
    }
    if (PIR1bits.TMR1IF) {
        // ~15 overflows of TMR1 per second
        if (sNODATA_COUNTER >= 15 * NODATA_LIMIT_IN_SECONDS) {
            sGLOBAL_STATE = ON_NODATA;
        } else {
            sNODATA_COUNTER++;
        }
        PIR1bits.TMR1IF = 0;
    }
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0;
    }
    if (INTCONbits.T0IF) {
        INTCONbits.T0IF = 0;
    }
    if (PIR1bits.EEIF) {
        PIR1bits.EEIF = 0;
    }
}

void delay_ms(int ms) {
    while (ms--) {
        __delay_us(999);
    }
}

void beeper_on(void) {
    sGPIO.GP5 = 1;
    GPIO = sGPIO.port;
}

void beeper_off(void) {
    sGPIO.GP5 = 0;
    GPIO = sGPIO.port;
}

void chirp(int ms_on, int ms_off) {
    di();
    ms_on *= 2;
    while (ms_on > 0) {
        beeper_on();
        __delay_us(160);
        beeper_off();
        __delay_us(160);
        ms_on--;
    }
    sLAST_PWM_US |= 0x8000;
    ei();

    delay_ms(ms_off);
}

int main(void) {
    // make GP5 an output
    TRISIO = 0xFF ^ 0x20;

    // set up timer1
    TMR1H = 0;
    TMR1L = 0;
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1CKPS1 = 0; // 1:1 pre-scaler
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;

    // set up interrupts
    IOCbits.IOC3 = 1;
    INTCONbits.INTF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GPIE = 1;
    INTCONbits.GPIF = 0;
    PIE1bits.TMR1IE = 1;
    ei();

    // the initial chirp-chirp and delay
    chirp(30, 50);
    chirp(30, 0);
    __delay_ms(1000);

    while (1) {
        switch (sGLOBAL_STATE) {
            case ON_EXPLICIT:
                chirp(1000, 50);
                break;
            case ON_TIMEOUT:
                chirp(100, 1900);
                break;
            case ON_NODATA:
                chirp(750, 750);
                break;
            default:
                beeper_off(); // just in case
                break;
        }
    }
    return 0;
}
