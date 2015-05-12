#ifndef _serial_hw_h_
#define _serial_hw_h_

#if 0
#define	UART01x_ADDR		0x101f1000
#else
#define	UART01x_ADDR		0x20201000
#endif

#define UART01x_DR              0x000
#define UART01x_FR_BUSY         0x008
#define	UART01x_FR_RXFE		0x010
#define UART01x_FR              0x018
#define UART01x_FR_TXFF         0x020
 
#endif
