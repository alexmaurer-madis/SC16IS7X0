This file (which I do not guarantee error free) is a quick reference guide to help me writing this library.
The base document I used is the *NXP PDF Rev.7 - 9 June 2011.*

The author : Alexandre Maurer
---

# Power-up sequence
- After power is applied, the device is reset by the internal POR. The host must wait at least
3us before initializing a communication with the device.

- The host processor must wait until the crystal is generating a stable clock before accessing
the UART transmitter or receiver.

- Normally, if an external clock such as a system clock or an external oscillator is used to
supply a clock to XTAL1 pin, the clock should be stable at this time.

- When using a crystal, the start-up time depends on the crystal being used, VCC ramp-up time and the
loading capacitor values. The start-up time can be as long as a few milliseconds.

# Sleep mode
**NOT PROGRAMMED YET*
- PDF Page 17
- Sleep mode is enabled with IER[4]
  - IER Register is only accessible when Enhanced Functions EFR[4] is enabled.
    - EFR Register is only accessible when LCR = 0xBF.
- In sleep mode the clock to the UART is stopped
- Sleep Mode is entered when :
  - The serial data input line, RX, is idle
  - The TX FIFO and TX shift register are empty
  - There are no interrupts pending except THR
- The UART will wake up :
  - When any change is detected on the RX line
  - Any change in the state of the modem inputs pins
  - Or data is written to the TX FIFO

# Programmable baudrate generator
- Clock input can be divided in the range between 1 and 65535
- An additional divide-by-4 prescaler is also available and can be selected by MCR[7]
  - MCR[7] is only writable when EFR[4] is set
    - EFR is only accessible when LCR is set to 0xBF
- Divisor formula is (XTAL1 freq / prescaler) / (Desired baudrate x 16)
  - Example for 115'200bps with a clock of 1.8432MHz
    - (1843200 / 1) / (115200 x 16) = 1
    - Divisor 1, no remainder, baudrate frequency is bang-on !

## Division Register (DLL, DLH)
- PDF Page 31
- These are two 8-bit registers which store the 16-bit divisor
- DLL and DLH accessible only when LCR[7] is logic 1.
- DLL and DLH can only be written to before Sleep mode is enabled (before IER[4] is set)

## Changing prescaler
- Set LCR to 0xBF, now EFR Register is accessible
- Set EFR[4] to 1, enhanced functions are now accessible
- Set MCR[7]
  - To 0 for divide-by-1
  - To 1 for divide-by-4

# Mode of operation
## Interrupt mode
- Any bit of IER[3:0] is set to 1
- Interrupt mode by signal with IRQ pin

## Polled mode
- IER[3:0]=0000 (default on POR)
- Polling the Line Status Register (LSR)
  - For example LSR[0] data in receiver

# Interrupts
**NOT PROGRAMMED YET**
- The SC16IS740/750/760 has interrupt generation and 7 prioritized levels of interrupts
- IIR[5:0] (PDF Page 15, table 6)

## Interrupt Enable Register (IER)
- PDF Page 29, table 19
- All Interrupts are disabled by default on POR
  - IER[0] RHR Receive Holding Register Interrupt
  - IER[1] THR Transmit Holding Register Interrupt
  - IER[2] Receive Line Status Interrupt
  - IER[3] Modem Status Interrupt
  - IER[4] Sleep mode
  - IER[5] XOFF Interrupt
  - IER[6] RTS Interrupt
  - IER[7] CTS Interrupt

## Interrupt Identification Register (IIR)
- PDF Page 30, table 20
- The IIR is a read-only 8-bit register which provides the source of the interrupt in a
prioritized manner
- All bits cleared on POR except bit 0 is set
  - IIR[0] Interrupt Pending (0=an interrupt is pending, 1=no interrupt is pending, default on POR)
  - IIR[5:1] 5-bit encoded interrupt
  - IIR[7:6] mirror the content of FCR[0] (FIFO Enable)

# Break and time-out conditions
- PDF Page 17

# FIFO
## FIFO Control Register (FCR)
- PDF Page 23, table 11
- All bits cleared on POR
  - FCR[0] TX/RX FIFO Enable (1) or Disabled (0 default)
  - FCR[1] Reset RX FIFO
  - FCR[2] Reset TX FIFO
  - FCR[5:4] TX Trigger level for interrupt (discarded If TLR has non-zero trigger level value)
  - FCR[6:7] RX Trigger level for interrupt (discarded If TLR has non-zero trigger level value)
- Trigger levels :
  - 8 / 16 / 56 / 60 chars

## Receive Holding Register (RHR)
- RHR = 64 bytes RX FIFO
- pointer logic cleared on POR
- RSR Receiver Shift Register
- If FIFO is disabled, location 0 of the FIFO is used to store the characters on reception
  
## Transmit Holding Register (THR)
- THR = 64 bytes TX FIFO
- pointer logic cleared on POR
- TSR Transmit Shift Register
- If FIFO is disabled, the FIFO is still used to store the byte
- Characters are lost if overflow occurs

# Line Control Register (LCR)
- PDF Page 24
- Controls the data communication format (Word length, Stop bits, Parity)
- Reset to 0b0001 1101 on POR
  - LCR[1:0] Word length
  - LCR[2] Number of stop bits
  - LCR[3] No Parity (0 default), Parity Enabled (1)
  - LCR[4] Parity type (0=odd, 1=even)
  - LCR[5] Forced parity format (0=parity not forced) (1=force parity, logic 1 if LCR[4]=0 or logic 0 if LCR[4]=1)
  - LCR[6] Break control bit
  - LCR[7] Divisor Latch Disabled (0 default), Enabled (1)
  
# Line Status Register (LSR)
- PDF Page 26, table 16
- LSR[0] Data in receiver
- LSR[1] Overrun error
- LSR[2] Parity error
- LSR[3] Framing error
- LSR[4] Break Interrupt
- LSR[5] THR Empty (0=not empty, 1=empty default on reset)
- LSR[6] THR and TSR Empty (0=not empty, 1=empty default on reset)
- LSR[7] FIFO data error

# Modem Control Register (MCR)
- PDF Page 27, table 17
- All bits cleared on POR
  - MCR[0] Force DTR output (1 force active, output low) (0=force inactive, output high)
  - MCR[1] Force RTS output (1 force active, output low) (0=force inactive, output high)
  - MCR[2] TCR and TLR enable
  - MCR[3] reserved
  - MCR[4] enable loopback
  - MCR[5] XON Any
  - MCR[6] IrDA Mode
  - MCR[7] Clock divisor prescaler (0 divide-by-1), (1 divide-by-4)

# Modem Status Register (MSR)
- PDF Page 28, table 8
- Bits 0:3 are cleared on POR, bits 4:7 input signals
  - MSR[0] CTS has changed state (cleared on read)
  - MSR[1] DSR has changed state (cleared on read)
  - MSR[2] RI has changed state from LOW to HIGH (cleared on read)
  - MSR[3] CD has changed state (cleared on read)
  - MSR[4] CTS state
  - MSR[5] DSR state
  - MSR[6] RI state
  - MSR[7] CD state

# Scratchpad register (SPR)
- This 8-bit register is used as a temporary data storage register.
- User’s program can write to or read from this register without any effect on the operation of the device.

# Transmission Control Register (TCR)
- PDF Page 32, table 23
- Used to stop/start transmission during hardware/software flow control
- Trigger levels from 0 to 60 with a granularity of 4
- TCR can only be written to when EFR[4]=1 and MCR[2]=1
- All bits cleared on POR
  - TCR[3:0] RX FIFO trigger level to halt transmission
    - TCR[3:0] must be greater than TCR[7:4] (There is no buit-in hardware check)
  - TCR[7:4] RX FIFO trigger level to resume

# Trigger Level Register (TLR)
- PDF Page 32, table 24
- Used for interrupt generation
- Trigger levels from 4 to 60 with a granularity of 4
- TLR can only be written to when EFR[4]=1 and MCR[2]=1
- If TLR has non-zero trigger level value, the trigger level defined in FCR is discarded
  - TLR[3:0] TX FIFO trigger level (4 to 60), number of spaces available
  - TLR[7:4] RX FIFO trigger level (4 to 60), number of characters available
- If TLR[3:0] or TLR[7:4] are logical 0, the selectable trigger levels via the FIFO Control Register (FCR) are used for the transmit and receive FIFO trigger levels. This applies to both transmit FIFO and receive FIFO trigger level setting

# FIFO Level Register (TXLVL or RXLVL)
- Value from 0 to 64 (0x00 to 0x40)
- TXLVL[7] not used, set to 0
- TXLVL[6:0] reports the number of spaces available in the transmit FIFO
  - reset to 0x40 on POR.
- RXLVL[6:0] reports the number of characters in the RX FIFO
  - reset to 0x00 on POR.

# GPIO (only available on the SC16IS750 and SC16IS760)
## I/O pins Direction Register (IODir)
- All bits cleared on POR
  - IODir[7:0] GPIO 7:0 (0=input, 1=output)

## I/O pins State Register (IOState)
- IOState[7:0] GPIO 7:0 (0=set output pin to zero, 1=set output pin to one)
- When read, return states of all pins

## I/O Interrupt Enable Register (IOIntEna)
- All bits cleared on POR
- IOIntEna[7:0] Input Interrupt Enable (1=enabled)

## I/O Control register (IOControl)
- PDF Page 34, table 30
- All bits cleared on POR
  - IOControl[0] IOLATCH Enable of disable inputs latching
  - IOControl[1] Set GPIO[7:4] as I/O pins (0 default) or MODEM pins (1)
  - IOControl[2] reserved
  - IOControl[3] Software reset
  - IOControl[7:4] reserved

# Extra / Enhanced Features
## Enhanced Features Register (EFR)
- PDF Page 31, table 22
- EFR Register is accessible only when LCR=0xBF
- All bits cleared on POR
  - EFR[3:0] Combinations of software flow control (see table 3)
  - EFR[4] Enhanced function enable bit (enable writing to IER[7:4] FCR[5:4] MCR[7:5])
  - EFR[5] Special character detect (0=disable, default)
  - EFR[6] RTS Flow control enable (0=disable, default)
  - EFR[7] CTS Flow control enable (0=disable, default)

## Extra Features Control Register (EFCR)
- PDF Page 35, table 31

## RS485 features
- PDF Page 35-36

### RTS used to control an RS485 line driver
- RTS output can be used to control an RS485 driver.
  - EFCR[4] set to 1=Transmit control RTS pin (take precedence over hardware flow control)
  - EFCR[5] set the polarity of RTS pin
    - EFCR[5] 0 = during transmission RTS=0, during reception RTS=1
    - EFCR[5] 1 = during transmission RTS=1, during reception RTS=0

### 9-bit mode or multidrop mode
- EFCR[0] 0=normal RS232 mode, 1=RS485 mode 
- In 9-bit mode the receiver is set to force parity 0 in order to detect address byte
  - Read paragraph 9.3.1 for more details on how to work with address byte detection
  - Auto address detection can be used if special character detect is enabled
    - EFR[5] set and XOFF2 register contains the address byte

## IrDA mode
- EFCR[7] 0 = Pulse width 3/16 bit time, data rate up to 115.2 kbit/s
- EFCR[7] 1 = Pulse width 1/4 bit time, data rate up to 1.152Mbit/s (For SC16IS760 only)
