#include <msp430.h>
//Michael Johns and Ben Zalewski
//Introduction to Embedded Systems
//Milestone 1
//10/24/19

void Timers(void);          //Declare Timers function
void LED(void);             //Declare LED function
void UART(void);            //Declare UART function

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               //stop watchdog timer

    Timers();               //Call Timers function
    LED();                  //Call LED function
    UART();                 //Call UART function

    __bis_SR_register(LPM0_bits + GIE);     //low power mode and interrupt enabled
}

void LED(void)              //LED function
{
    P1DIR |= BIT6;          //set LED 1.6 to output direction (Red LED)
    P1SEL |= BIT6;          //sets PWM for pin 1.6
    P2DIR |= (BIT1 + BIT5); //set LED 2.1 (Green) and 2.5 (Blue) to output direction
    P2SEL |= (BIT1 + BIT5); //sets PWM for pins 2.1 and 2.5

}

void Timers(void)           //Timers function
{
    //timer A0
    TA0CTL |= TASSEL_2 + MC_1;     //set smclk, up mode
    TA0CCTL1 |= OUTMOD_7;   //set/reset output
    TA0CCR0 = 255;          //pwm period
    TA0CCR1 = 0;            //initialize red

    //timer A1
    TA1CTL |= TASSEL_2 + MC_1;     //set smclk, up mode
    TA1CCTL1 |= OUTMOD_7;          //set/reset output
    TA1CCTL2 |= OUTMOD_7;          //set/reset output
    TA1CCR0 = 255;                 //pwm period
    TA1CCR1 = 0;                   //initialize green
    TA1CCR2 = 0;                   //initialize blue

}

void UART(void)             //UART function
{
    P1SEL |= BIT1 + BIT2;   //P1.1 = RXD P1.2 = TXD
    P1SEL2 |= BIT1 + BIT2;  //P1.1 = RXD P1.2 = TXD

    UCA0CTL1 |= UCSSEL_2;   //smclk
    UCA0BR0 = 104;          //1MHz 9600 baud rate
    UCA0BR1 = 0;            //1MHz 9600 baud rate
    UCA0MCTL = UCBRS0;      //modulation UBRSx = 1
    UCA0CTL1 &= ~UCSWRST;   //initialize usci state machine

    IE2 |= UCA0RXIE;        //enable RX interrupt
}

int length = 0;             //initialize length variable
int counter=1;              //initialize counter variable
int red=0;                  //initialize red variable
int green=0;                //initialize green variable
int blue=0;                 //initialize blue variable

#pragma vector=USCIAB0RX_VECTOR     //Interrupt routine
__interrupt void TESTRUN(void)
{
  while (!(IFG2&UCA0TXIFG));        //if there's information to be received
  if(counter==1)                    //if information received, buffer registers initialized
  {
      UCA0TXBUF=UCA0RXBUF-3;        //output is three less than input
      length=UCA0RXBUF-3;           //set length to output
  }
  else if(counter<5)                //RGB cases are 2,3, and 4 respectively so check if counter is less than 5
  {
      switch(counter)               //switch case to check the value counter is at
      {
      case 2:                       //if counter is 2
          red=UCA0RXBUF;            //register for Red LED receives data
          break;                    //break out of case
      case 3:                       //if counter is 3
          green=UCA0RXBUF;          //register for Green LED receives data
          break;                    //break out of case
      case 4:                       //if counter is 4
          blue=UCA0RXBUF;           //register for Blue LED receives data
          break;                    //break out of case
      default:
          break;
      }

  }
  else
    UCA0TXBUF = UCA0RXBUF;
  if(counter==length+3)        //if counter is greater than the length
  {
      counter=0;               //reset counter
      TA0CCR1=red;             //set red variable to timer A0
      TA1CCR1=green            //set green variable to timer A1
      TA1CCR2=blue;            //set blue variable to timer A1
  }

  counter++;                        //increment counter by 1
}
