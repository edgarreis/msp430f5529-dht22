#include <msp430f5529.h>
#include "dht22.h"
#include <stdio.h>

char txbuf[256];

void uart_init() {
  
      // Setando os pinos para usar as Uart's
    P3SEL |= BIT3 + BIT4; 		    // P3.3,4 = USCI_A0 - Uart 0
    
    // Inicialização da Uart 0
    UCA0CTL1    |= UCSWRST;		    // Inicialização da Uart 0
    UCA0CTL1    |= UCSSEL_2;                // SMCLK
    UCA0BR0     = 0x6;                      // 1MHz 9600
    UCA0BR1     = 0;                        // 1MHz 9600
    UCA0MCTL    |= UCBRS_0 + UCBRF_13 + UCOS16;		// = 1 Modulação UCBRSx
    UCA0CTL1    &= ~UCSWRST;                 // UCS Set
    UCA0IE      |= UCRXIE;                   // Transmit interrupt enable
      
    /*
    UCA0CTL0 = 0;
    UCA0CTL1 |= UCSWRST;           // Put USCI in reset mode
    UCA0CTL1 = UCSSEL_2 | UCSWRST; // Use SMCLK, still reset
    UCA0MCTL = UCBRF_0 | UCBRS_6;
    UCA0BR0 = 131;                 // 9600 bauds
    UCA0BR1 = 6;
    UCA0CTL1 &= ~UCSWRST;          // Normal mode
    P1SEL2 |= (BIT1 + BIT2);       // Set pins for USCI
    P1SEL |= (BIT1 + BIT2);
    */
}

void uart_send(int len) {
  int i;
  for(i = 0; i < len; i++) {
    //while (UCA0STAT & UCBUSY);
    while(!(UCA0IFG & UCTXIFG));    // UCA0TXIFG up when buffer is full
    UCA0TXBUF = txbuf[i];
  }
}



void main() {
    
    WDTCTL = WDTPW | WDTHOLD;
    //DCOCTL = 0;
    //BCSCTL1 = CALBC1_16MHZ;
    //DCOCTL = CALDCO_16MHZ;
    uart_init();
    // Config. UART
    _BIS_SR(GIE);

    while(1) {
        dht_start_read();
        int t = dht_get_temp();
        int h = dht_get_rh();
        uart_send(sprintf(txbuf, "%3d.%1d C; %3d.%1d %%RH\r\n", t/10, t%10, h/10, h%10));
        //  %3d.%1d C means the value has fewer characters than the field width, 000,00 C 
        __delay_cycles(5000000);        
        // 0.3125 s with 16Mhz (G2)
    }  
}