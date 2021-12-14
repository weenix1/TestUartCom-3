//===========================================================
// Test program UART0 TX 8/N/1 @ 9600 bit/s
// LTL 17.5.2020
//==========================================================

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>
#include "int_handler.h"
#include <string.h>

volatile unsigned char cmdLineUart[16];
volatile unsigned short int i = 0;
volatile unsigned char gucNewData = 0;
volatile int switchPressed = 0;

void configPortA(void){
    // initialize Port A
    SYSCTL_RCGCGPIO_R |= (1<<0); // switch on clock for Port A
    while((SYSCTL_PRGPIO_R & (1<<0)) == 0) // wait for stable clock
        ;

    GPIO_PORTA_AHB_DEN_R |= 0x03; // digital I/O enable for pin PA(1:0)
    GPIO_PORTA_AHB_AFSEL_R |= 0x03; // PA(1:0) set to alternate function
    GPIO_PORTA_AHB_PCTL_R |= 0x00000011; // alternate function is U0Rx+Tx
}

void configUart0(void){
    // initialize UART0
    SYSCTL_RCGCUART_R |= (1<<0); // switch on clock for UART0
    while((SYSCTL_PRUART_R & (1<<0))==0) // wait for stable clock
        ;
    UART0_CTL_R &= ~(1<<0); // disable UART0 for config

    // initialize bitrate of 9600 bit per second
    UART0_IBRD_R = 104; // set DIVINT of BRD floor(16MHz/16/9600Hz)
    UART0_FBRD_R = 11; // set DIVFRAC of BRD
    UART0_LCRH_R = 0x00000040; // serial format 7N1
    UART0_CTL_R |= 0x0301; // re-enable UART0

    // UART0 interrupts
    UART0_ICR_R = 0xE7FF;
    UART0_IM_R = 1 << 4;
    NVIC_EN0_R |= 1 << 5;
}

void configLEDSW(void)
{
    SYSCTL_RCGCGPIO_R |= 0x00001121; // => port a, j, f, n gets a clock

    // wait until the port is available
    while ((SYSCTL_RCGCGPIO_R & 0x00001121) == 0)
        ;
    GPIO_PORTN_DEN_R = 0x03; // => enable LED lights D1&D2
    GPIO_PORTN_DIR_R = 0x03; // => set LED lights D1&D2 as output
    GPIO_PORTF_AHB_DEN_R = 0x11; // => enable LED lights D3&D4
    GPIO_PORTF_AHB_DIR_R = 0x11; // => set LED lights D3&D4 as output

    //Digital input & WPU
    GPIO_PORTJ_AHB_DEN_R = 0x01;  // enable SW1( PIN PJ(0))
    GPIO_PORTJ_AHB_DIR_R = 0x00; // set SW1 input
    GPIO_PORTJ_AHB_PUR_R = 0x01; // enable WPU for SW1

    //PORTJ interrupts setting
    GPIO_PORTJ_AHB_IEV_R |= 0x01;

    GPIO_PORTJ_AHB_ICR_R = 0x01;
    GPIO_PORTJ_AHB_IM_R |= 0x01;

    NVIC_EN1_R |= 1 << (51 - 32); // enable UART0 IRQ in NVIC(IRQ 5)
}

int check_leds() {
    return cmdLineUart[0] == 'l' && cmdLineUart[1] == 'e' && cmdLineUart[2] == 'd' && cmdLineUart[3] == 's' && cmdLineUart[4] == ' ';
}

void cmdToLed(void)
{
    int j;

    if (check_leds()) {
        for (j = 5; j < 16; j++)
        {
            if (cmdLineUart[j] == '+')
            {
                switch (cmdLineUart[++j])
                {
                case '1':
                    GPIO_PORTN_DATA_R |= 0x02;
                    break;
                case '2':
                    GPIO_PORTN_DATA_R |= 0x01;
                    break;
                case '3':
                    GPIO_PORTF_AHB_DATA_R |= 0x10;
                    break;
                case '4':
                    GPIO_PORTF_AHB_DATA_R |= 0x01;
                    break;
                }
            }
            else if (cmdLineUart[j] == '-')
            {
                switch (cmdLineUart[++j])
                {
                case '1':
                    GPIO_PORTN_DATA_R &= ~0x02;
                    break;
                case '2':
                    GPIO_PORTN_DATA_R &= ~0x01;
                    break;
                case '3':
                    GPIO_PORTF_AHB_DATA_R &= ~0x10;
                    break;
                case '4':
                    GPIO_PORTF_AHB_DATA_R &= ~0x01;
                    break;
                }
            }
            else if (cmdLineUart[j] == '~')
            {
                switch (cmdLineUart[++j])
                {
                case '1':
                    GPIO_PORTN_DATA_R ^= 0x02;
                    break;
                case '2':
                    GPIO_PORTN_DATA_R ^= 0x01;
                    break;
                case '3':
                    GPIO_PORTF_AHB_DATA_R ^= 0x10;
                    break;
                case '4':
                    GPIO_PORTF_AHB_DATA_R ^= 0x01;
                    break;
                }

            }
        }
    }
}


void main(void){
    // config Port A
    configPortA();
    // config UART0 Rx+Tx
    configUart0();
    //config LDE and SW
    configLEDSW();

    int inputWait = 1;

    while(1){
        while((UART0_FR_R & (1<<5)) !=0); // till transmit FIFO empty

        if (inputWait) // do this when input wait flag is on
        {
            UART0_DR_R = '\n';
            UART0_DR_R = '>'; // send '>'
            inputWait = 0;
        }

        if (gucNewData) // if there was new input, print it on console and clear the cmdLineUart array
        {
            gucNewData = 0;
            cmdToLed();
            printf("%s\n", cmdLineUart);
            memset(cmdLineUart, '\0', sizeof(cmdLineUart));
            fflush(stdout);
            inputWait = 1;
        }
    }
}

