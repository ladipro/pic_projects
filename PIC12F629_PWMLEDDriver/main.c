/*
 * 8 channel PWM LED driver
 * https://github.com/ladipro/pic_projects/wiki/8-channel-PWM-LED-driver
 *
 * Target microcontroller: PIC12F629
 *
 */
#include <htc.h>
#include "pwm_timeline.h"

#if defined(WDTE_OFF)
__CONFIG(WDTE_OFF & LVP_OFF);
#elif defined (WDTDIS)
__CONFIG(WDTDIS & LVPDIS);
#endif

// TIMING: 4 MHz clock, 1 M instructions per second
//         3906 ISR invocations per second (256 us)
//         44 Hz LED refresh (22.5 ms)
//         5.5 Hz all LED serial refresh (worst case)
#define _XTAL_FREQ 4000000
#pragma config config WDTE=OFF,PWRTE=OFF,CP=OFF,BOREN=OFF,MCLRE=OFF,CPD=OFF,FOSC=INTRCIO

#define PWM_MAX_ERROR 50

volatile union { // shadow copy of GPIO
unsigned char port;
    struct {
    unsigned char GP0 : 1;
    unsigned char GP1 : 1;
    unsigned char GP2 : 1;
    unsigned char GP3 : 1;
    unsigned char GP4 : 1;
    unsigned char GP5 : 1;
    };
} sGPIO;

void led_on(unsigned char i) {
    // address
    sGPIO.port = 3; // latch enable = off, data = 1
    sGPIO.GP2 = !!(i & 1);
    sGPIO.GP4 = !!(i & 2);
    sGPIO.GP5 = !!(i & 4);
    GPIO = sGPIO.port;
    sGPIO.GP0 = 0; // latch enable = on
    GPIO = sGPIO.port;
    sGPIO.GP0 = 1; // latch enable = off
    GPIO = sGPIO.port;
}

void led_off(unsigned char i) {
    // address
    sGPIO.port = 1; // latch enable = off, data = 0
    sGPIO.GP2 = !!(i & 1);
    sGPIO.GP4 = !!(i & 2);
    sGPIO.GP5 = !!(i & 4);
    GPIO = sGPIO.port;
    sGPIO.GP0 = 0; // latch enable = on
    GPIO = sGPIO.port;
    sGPIO.GP0 = 1; // latch enable = off
    GPIO = sGPIO.port;
}

// Modulation cycle position counter
volatile unsigned char sCOUNTER = 0;

// Desired LED brightness, all writes go through set_led()
volatile unsigned char sLEDS[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

void interrupt tc_int(void) {
    if (INTCONbits.T0IF) {
        unsigned char data = EEPROM_READ(sCOUNTER);
        if ((data & 0x80) == 0) {
            unsigned char led = data & 0x07;
            if (data & 0x08) {
                // always either on or off
                if (sLEDS[led] > 0) {
                    led_on(led);
                } else {
                    led_off(led);
                }
            } else if (data >> 4 == sLEDS[led]) {
                // off only if this is the desired intensity
                led_off(led);
            }
        }
        if (++sCOUNTER == MODULATION_CYCLE_LENGTH) {
            sCOUNTER = 0;
        }
        INTCONbits.T0IF = 0;
    }
}

// Simple GP3 PWM polling logic
volatile unsigned short sLAST_0_TIME = 0;
volatile unsigned short sLAST_1_TIME = 0;
volatile unsigned short sINPUT_PWM = 0;
volatile bit sLAST_GP3_STATE = 0;
void poll_input() {
    if (GPIObits.GP3) {
        if (sLAST_GP3_STATE) {
            sLAST_1_TIME = TMR1;
        } else {
            // rememeber this as last 1 time if we're
            // resonably close to the time we last saw 0
            if (TMR1 - sLAST_0_TIME < PWM_MAX_ERROR) {
                sLAST_1_TIME = TMR1;
                sLAST_GP3_STATE = 1;
            }
        }
    } else {
        if (!sLAST_GP3_STATE) {
            sLAST_0_TIME = TMR1;
        } else {
            // use the pulse duration if we're reasonably close
            // to the time we last saw 1
            if (TMR1 - sLAST_1_TIME < PWM_MAX_ERROR) {
                sINPUT_PWM = TMR1 - sLAST_0_TIME;
            }
            sLAST_0_TIME = TMR1;
            sLAST_GP3_STATE = 0;
        }
    }
}

void set_led(unsigned char led, unsigned char intensity) {
    // sync points immediately follow the timeline in data eeprom
    unsigned char sync_point = EEPROM_READ(MODULATION_CYCLE_LENGTH + led);
    while (sCOUNTER != sync_point || TMR0 > 224) {
        poll_input();
    }
    sLEDS[led] = intensity;
}

void delay(int cycles) {
    // delay loop is based on TMR0
    while (--cycles) {
        while (sCOUNTER != 0) {
            poll_input();
        }
        while (sCOUNTER == 0) {
            poll_input();
        }
    }
}

//////////// Business logic - actual light effect code starts here ////////////

// Lights swipe across LEDs in one direction
void swipe_loop() {
    for (unsigned char i = 0; i < 8; i++) {
        for (unsigned char j = i; j < i + 8; j++) {
            set_led(j & 0x07, j - i);
        }
        delay(2);
    }
}

// All LEDs pulse from off to on to off again
void pulse_loop() {
    for (unsigned char intensity = 0; intensity < 8; intensity++) {
        for (unsigned char led = 0; led < 8; led++) {
            set_led(led, intensity);
        }
        delay(2);
    }
    for (unsigned char intensity = 0; intensity < 8; intensity++) {
        for (unsigned char led = 0; led < 8; led++) {
            set_led(led, 7 - intensity);
        }
        delay(2);
    }
    delay(4);
}

unsigned char step_cylon_led(unsigned char n) {
    if (n & 0x80) {
        if (n == 0x80) {
            return 1;
        }
        return n - 1;
    } else {
        if (n == 7) {
            return 6 | 0x80;
        }
        return n + 1;
    }
}

// Cylon-style search light with a bit of hesitation at the endpoints
void cylon_loop() {
    unsigned char led1 = 2;
    unsigned char led2 = 1;
    unsigned char led3 = 0;
    for (unsigned char i = 0; i < 16; i++) {
        for (unsigned char led = 0; led < 8; led++) {
            if (led == (led1 & 0x7f)) {
                set_led(led, 7);
            } else if (led == (led2 & 0x7f)) {
                set_led(led, 4);
            } else if (led == (led3 & 0x7f)) {
                set_led(led, 1);
            } else {
                set_led(led, 0);
            }
        }
        if (i == 5 || i == 13) {
            // the light slows down at endpoints
            led2 = step_cylon_led(led2);
            led3 = step_cylon_led(led3);
        } else {
            led1 = step_cylon_led(led1);
            if (i != 6 && i != 14) {
                led2 = step_cylon_led(led2);
                led3 = step_cylon_led(led3);
            }
        }
        delay(4);
    }
}

// All LEDs off
void off_loop() {
    for (unsigned char led = 0; led < 8; led++) {
        set_led(led, 0);
    }
}

int main(void) {
    // make all outputs
    TRISIO = 0x00;

    // set up timer0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 1;

    // set up timer1
    TMR1H = 0;
    TMR1L = 0;
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1CKPS1 = 0; // 1:1 pre-scaler
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;

    // set up interrupts
    INTCONbits.T0IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.T0IF = 0;
    ei();

    unsigned char effect = 0;
    unsigned char last_on = 0;
    while (1) {
        if (sINPUT_PWM > 1500) {
            if (!last_on) {
                last_on = 1;
                if (++effect > 2) {
                    effect = 0;
                }
            }
            switch (effect) {
                case 0: swipe_loop(); break;
                case 1: pulse_loop(); break;
                case 2: cylon_loop(); break;
            }
        } else {
            last_on = 0;
            off_loop();
        }
    }
    return 0;
}
