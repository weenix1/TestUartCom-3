/*
 * int_handler.h
 */

#ifndef INT_HANDLER_H_
#define INT_HANDLER_H_

void IntHandlerUART0(void);
void IntPortJHandler(void);

extern volatile unsigned char gucNewData;
extern volatile unsigned char cmdLineUart[16];
extern volatile unsigned short int i;
extern volatile int switchPressed;

#endif /* INT_HANDLER_H_ */
