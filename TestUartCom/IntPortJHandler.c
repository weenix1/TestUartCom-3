#include "inc/tm4c1294ncpdt.h"
#include "int_handler.h"
#include <stdint.h>
#include <stdio.h>

void IntHandlerUART0(void)
{
    if (UART0_MIS_R & (1 << 4)) // check whether UART0 Rx interrupt
    {
        UART0_ICR_R |= (1 << 4); // clear interrupt
        if (i >= 15 || UART0_DR_R == 0x0D) // do this if out of the size of array
        {
            cmdLineUart[15] = '\0';
            i = 0;
            gucNewData = 1;
        }
        else
        {
            cmdLineUart[i++] = UART0_DR_R;
        }
    }
}

void IntPortJHandler(void)
{

    if (GPIO_PORTJ_AHB_MIS_R & (1 << 0))
    {
        GPIO_PORTJ_AHB_ICR_R |= (1 << 0); // clear interrupt

        UART0_DR_R = '#';

        // check LDE values and print the numbers accordingly on the terminal
        if (GPIO_PORTN_DATA_R == 0x02)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '1';
        }
        if (GPIO_PORTN_DATA_R == 0x01)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '2';
        }
        if (GPIO_PORTN_DATA_R == 0x03)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '1';
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '2';
        }

        if (GPIO_PORTF_AHB_DATA_R == 0x10)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '3';
        }
        if (GPIO_PORTF_AHB_DATA_R == 0x01)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '4';
        }
        if (GPIO_PORTF_AHB_DATA_R == 0x11)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '3';
            while ((UART0_FR_R & (1 << 5)) != 0)
                ; // till transmit FIFO empty
            UART0_DR_R = '4';
        }
    }
}

