/* Based on msp430g2xx3_wdt_02.c from the TI Examples */
#include <msp430.h>

int ticks_since_last_rev = 0;
int mm_wheel_diameter = 622;
float current_speed = 0;
static int flag = 0;
int asleep = 0;

unsigned int data_idx = 0;

int main(void)
{
    BCSCTL3 |= LFXT1S_2;                        // ACLK = VLO
    WDTCTL = WDT_ADLY_1_9;                       // WDT 250ms, ACLK, interval timer
    IE1 |= WDTIE;                               // Enable WDT interrupt

    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // Set the control mask for the ADC
    ADC10CTL1 = INCH_0;                        // Set the channel of the ADC
    ADC10AE0 |= 0x02;                           // Enable the analog input
    //    P1OUT = 0x00;


    UCB0CTL1 |= UCSWRST; // **Initialize USCI state machine**

    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master

    UCB0CTL1 |= UCSSEL_2; // SMCLK
    UCB0BR0 |= 0; //
    UCB0BR1 = 0; //

    P1DIR |= BIT5 + BIT7; // could be a third for receive
    P1SEL = BIT5 + BIT7;
    P1SEL2 = BIT5 + BIT7;

    UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

    P1DIR |= BIT2;                              // Set the direction of the port
    P1SEL |= BIT2;                              // P1.2 to TA0.1
    P1SEL2 &= ~BIT2;                    // From the datasheet, must be disabled for PWM
    TA0CCTL1 = OUTMOD_7;
    TA0CCR0 = 100;                              // Set the period of the PWM
    TA0CCR1 = 100;
    TA0CTL = TASSEL_1 + MC_1;                   // Enable the ACLK and Up Mode

    //    P2OUT = 0x00;
    P2SEL |= BIT4 + BIT1;                       // From the datasheet, must be enabled for PWM
    P2SEL2 &= ~BIT4 + ~BIT1;                    // From the datasheet, must be disabled for PWM
    P2DIR |= BIT4 + BIT1;                       // Set the outgoing pin of Port2

    TA1CCTL1 = OUTMOD_7;                        // Set the reset properties of the CCR1 for PWM
    TA1CCTL2 = OUTMOD_7;
    TA1CCR0 = 100;                              // Set the period of the PWM
    TA1CCR1 = 100;
    TA1CCR2 = 0;                                // Set the duty of the PWM, where TA1CCR1/TA1CCR0 is duty cycle
    TA1CTL = TASSEL_1 + MC_1;                   // Enable the ACLK and Up Mode

    IE2 |= UCB0TXIE; // Enable USCI0 TX interrupt
    __bis_SR_register(GIE);                     // Enable interrupts

    ADC10CTL0 |= ENC + ADC10SC;                 // Enable and start the conversion
    __bis_SR_register(LPM3_bits);               // Enter LPM3 w/interrupt

    int refTemp = 330;                     // Grab the initial temperature value of the sensor
    int led_num = 144;
    int data_len = 8 + (4 * led_num);
    int counter = 0;
    int revs = 0;
    int counter_enable = 1;
    int polling_counter = 400;
    float highest = 0;
    float lower_bound = .00001;
    float upper_bound = .0029;
    int bike_state = 2;
    int rotate = 0;

    while (1) {
       if (highest < current_speed) highest = current_speed;
       while (flag == 1);
       flag = 1;
       if (data_idx < 4) UCB0TXBUF = 0x00;
       else if (data_idx >= data_len - 4) UCB0TXBUF = 0xFF;
       else if (data_idx % 4 == 0) UCB0TXBUF = 0xE9;
       else {
           int mod1 = (ticks_since_last_rev % 250);
           int mod3 = (ticks_since_last_rev % 333);
           int data_mod4 = data_idx % 4;
           if (data_mod4 == 1) {
                if (bike_state == 0) {
                    if (current_speed < lower_bound) UCB0TXBUF = 0x0F;
                    else if (current_speed > (upper_bound - lower_bound) / 2) UCB0TXBUF = 0x0F;
                    else UCB0TXBUF = 0x0E - 0x0E * (2 / (upper_bound - lower_bound)) * (current_speed - lower_bound);
                } else if (bike_state == 2) {
                  if (mod3 <= 111) {
                      if (rotate == 0) UCB0TXBUF = 0x0F;
                      else UCB0TXBUF = 0x00;
                  } else if (mod3 > 111 && mod3 <= 222) {
                      if (rotate == 1) UCB0TXBUF = 0x0F;
                      else UCB0TXBUF = 0x00;
                  } else {
                      if (rotate == 2) UCB0TXBUF = 0x0F;
                      else UCB0TXBUF = 0x00;
                  }
                } else {
                  UCB0TXBUF = 0x00;
                }
           }
           else if (data_mod4 == 2) {
               if (bike_state == 0) {
                  if (current_speed < lower_bound) UCB0TXBUF = 0x00;
                  else if (current_speed > upper_bound) UCB0TXBUF = 0x0F;
                  else UCB0TXBUF = 0x0F * (3 / (upper_bound - lower_bound)) * (current_speed - lower_bound);
               } else if (bike_state == 2) {
                   if (mod3 <= 111) {
                       if (rotate == 1) UCB0TXBUF = 0x0F;
                       else UCB0TXBUF = 0x00;
                   } else if (mod3 > 111 && mod3 <= 222) {
                       if (rotate == 2) UCB0TXBUF = 0x0F;
                       else UCB0TXBUF = 0x00;
                   } else {
                       if (rotate == 0) UCB0TXBUF = 0x0F;
                       else UCB0TXBUF = 0x00;
                   }
               } else {
                   UCB0TXBUF = 0x00;
               }
           }
           else if (data_mod4 == 3) {
                if (bike_state == 0) {
                   if (current_speed < (upper_bound - lower_bound) / 2) UCB0TXBUF = 0x00;
                   else if (current_speed > upper_bound) UCB0TXBUF = 0x0F;
                   else UCB0TXBUF = 0x0F * (6 / (upper_bound - lower_bound)) * (current_speed - lower_bound);
                } else if (bike_state == 1) {
                   if (mod1 <= 125) {
                       UCB0TXBUF = 0x00;
                   } else {
                       UCB0TXBUF = 0x0F;
                   }
                } else if (bike_state == 2) {
                    if (mod3 <= 111) {
                        if (rotate == 0) UCB0TXBUF = 0x0F;
                        else UCB0TXBUF = 0x00;
                    } else if (mod3 > 111 && mod3 <= 222) {
                        if (rotate == 1) UCB0TXBUF = 0x0F;
                        else UCB0TXBUF = 0x00;
                    } else {
                        if (rotate == 2) UCB0TXBUF = 0x0F;
                        else UCB0TXBUF = 0x00;
                    }
                } else {
                    UCB0TXBUF = 0x00;
                }
           }
       }

       if ((ticks_since_last_rev % 300) > 200) {
           if (rotate >= 2) {
               rotate = 0;
           } else {
               rotate++;
           }
       }

       data_idx++;

       if (ticks_since_last_rev > 1000 && bike_state != 2 && counter > polling_counter) {
          bike_state = 1;
       }

       if (data_idx >= data_len) {
           data_idx = 0;
       }

        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
       __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
       if (ADC10MEM < 0x0010 && (revs > -1 && (counter > polling_counter || counter_enable == 0))) {
           current_speed = (mm_wheel_diameter / (1900.0 * ticks_since_last_rev));
           ticks_since_last_rev = 0;
           revs++;
           counter = 0;
           bike_state = 0;
       }
       if (counter < polling_counter + 2) {
           counter++;
       }
    }
}



// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    ticks_since_last_rev++;
     __bic_SR_register_on_exit(LPM3_bits);     // Clear the LPM3 mask
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
   __bic_SR_register_on_exit(LPM3_bits);       // Clear the LPM3 mask
}

// Test for valid RX and TX character
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    flag = 0; // turn off critical flag
    IFG2 &= ~UCB0TXIFG; // CRITICAL HIT - NATURAL 20!!!!
}

// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
  P1IE  &= ~BIT2;   // Clear the interrupt enabled bits of P1.
  P1IFG &= ~BIT2;   // Necessary to clear the interrupt flag because of the GIE.
  if (asleep) {
    asleep = 0;
    P1IE  |= BIT2;
    __bic_SR_register_on_exit(LPM4_bits);  // Exit LPM4 and return to normal power mode.
  }
  else {
    P1OUT &= ~0xC0;  // Clear the LEDS associated with P1.
    P2OUT &= ~0xFF;  // Clear the LEDs associated with P2.
    asleep = 1;
    P1IE  |= BIT2;
    __bis_SR_register_on_exit(LPM4_bits + GIE); // Enter LPM4 and wait for interrupt.
  }
}

