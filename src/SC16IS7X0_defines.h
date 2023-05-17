#ifndef SC16IS7X0_DEFINES_H
#define SC16IS7X0_DEFINES_H

#include <inttypes.h>

// General register set

// Read, Receive Holding Register
#define SC16IS7X0_RHR 0x00
// Write, Transmit Holding Register
#define SC16IS7X0_THR 0x00
// R/W, Interrupt Enable Register
#define SC16IS7X0_IER 0x01
// Read, Interrupt Identification Register
#define SC16IS7X0_IIR 0x02
// Write, FIFO Control Register
#define SC16IS7X0_FCR 0x02
// R/W, Line Control Register
#define SC16IS7X0_LCR 0x03
// R/W, Modem Control Register
#define SC16IS7X0_MCR 0x04
// Read, Line Status Register
#define SC16IS7X0_LSR 0x05
// Read, Modem Status Register
#define SC16IS7X0_MSR 0x06
// R/W, Scratchpad register
#define SC16IS7X0_SPR 0x07
// R/W, Transmission Control Register (These registers are accessible only when
// MCR[2] = 1 and EFR[4] = 1)
#define SC16IS7X0_TCR 0x06
// R/W, Trigger Level Register (These registers are accessible only when MCR[2]
// = 1 and EFR[4] = 1)
#define SC16IS7X0_TLR 0x07
// Read, Transmit FIFO Level Register
#define SC16IS7X0_TXLVL 0x08
// Read, Receive FIFO Level Register
#define SC16IS7X0_RXLVL 0x09
// R/W, I/O pin Direction Register (Only available on the SC16IS750/SC16IS760)
#define SC16IS7X0_IODIR 0x0A
//!< R/W, I/O pin States Register (Only available on the SC16IS750/SC16IS760)
#define SC16IS7X0_IOSTATE 0x0B
// R/W, I/O Interrupt Enable Register (Only available on the
// SC16IS750/SC16IS760)
#define SC16IS7X0_IOINTENA 0x0C
// R/W, I/O pins Control Register (Only available on the SC16IS750/SC16IS760)
#define SC16IS7X0_IOCONTROL 0x0E
// R/W, Extra Features Register (Accessible only when LCR is set to 1011 1111b
// (0xBF))
#define SC16IS7X0_EFCR 0x0F

// Special Register Set (only accessible when LCR[7] is logic 1 and not 0xBF)

// R/W, Divisor Latch LSB
#define SC16IS7X0_DLL 0x00
// R/W, Divisor Latch MSB
#define SC16IS7X0_DLH 0x01

// Enhanced register set (only accessible when LCR = 0xBF)

// R/W, Enhanced Feature Register
#define SC16IS7X0_EFR 0x02
// R/W, XON1 word
#define SC16IS7X0_XON1 0x04
// R/W, XON2 word
#define SC16IS7X0_XON2 0x05
// R/W, XOFF1 word
#define SC16IS7X0_XOFF1 0x06
// R/W, XOFF2 word
#define SC16IS7X0_XOFF2 0x07

#define SC16IS7X0_READ_FLAG 0x80

//============================================
// Some defines needed by the ESP32 platforms
//============================================

#ifdef ESP32

using SerialConfig = uint32_t;

#define UART_PARITY_MASK 0b00000011
#define UART_NB_BIT_MASK 0b00001100
#define UART_NB_STOP_BIT_MASK 0b00110000

#define UART_NB_BIT_5 0x0000
#define UART_NB_BIT_6 0x0004
#define UART_NB_BIT_7 0x0008
#define UART_NB_BIT_8 0x000c

#define UART_PARITY_NONE 0x0000
#define UART_PARITY_EVEN 0x0002
#define UART_PARITY_ODD 0x0003

#define UART_NB_STOP_BIT_1 0x0010
#define UART_NB_STOP_BIT_2 0x0030

#endif

#endif // SC16IS7X0_DEFINES_H