/*
 * Project1.c
 *
 * 
 * Team: LAB41140573
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#define FOSC 8000000
#define BAUD 9600
#define UBBRVALUE ((FOSC / 16) / BAUD) - 1
#define BUFFERSIZE 16										/* static buffer size for communication */
#define BOARDSIZE 64
void init_Tx(unsigned int ubbr);
void writeByte(char c);
/*void writeString(char s[]);*/
void readByte(char c);
void init_Rx(void);
uint8_t asciiToInteger(char c);
char intToAscii(uint8_t i);
void examineInstr(char c[]);
void updateRxBuffer(char c[]);
void writeMemory(uint8_t letter, uint8_t j, char val) ;
char readMemory(uint8_t letter, uint8_t j) ;
void timerInit(void) ;
void MyMove(void) ;
void MyPass(void) ;
void Win(void) ;
void Lose(void) ;
void Tie(void) ;
void IllegalMove(void) ;
void IllegalTime(void) ;
void quit(void) ;
void initMemory(void) ;

char TX_buffer[BUFFERSIZE] ;									/* Buffer for transmission */
char RX_buffer[BUFFERSIZE] ;									/* Buffer for receiving */
char Table[BOARDSIZE] ;

uint8_t TxreadPos = 0 ;											/* initial values = 0 for the r/w pointers of each buffer */
uint8_t TxwritePos = 0 ;
uint8_t RxwritePos = 0 ;
uint8_t seconds = 0 ;
uint8_t timelimit = 2 ;


ISR(USART_TXC_vect)											/* Each time a transmission completes, send the next character from the Tx buffer until we finished with the all inserted characters */
{
	if (UCSRA & (1 << UDRE))
	{
		if (TxreadPos < TxwritePos)
		{
			UDR = TX_buffer[TxreadPos] ;
			TxreadPos++ ;
		}
		if (TxreadPos == TxwritePos)
		{
			TxreadPos = 0 ;
			TxwritePos = 0 ;
		}
	}
}

ISR(USART_RXC_vect)
{
	char c = UDR ;
	if ( c != '\r')
	{
		readByte(c) ;
	}
	else
	{
		/* Call Examine function */
		updateRxBuffer(RX_buffer) ;
		examineInstr(RX_buffer) ;
	}
}

ISR(TIMER1_COMPA_vect)
{
	seconds ++ ;
	if (seconds >= timelimit)
	{
		seconds = 0 ;
		PORTB ^= (1 << PB5) ;
	}
}






int main(void)
{
    /* Replace with your application code */
	initMemory() ;
	init_Tx(UBBRVALUE) ;
	init_Rx() ;
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB5) ;
	PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB5) ;
	timerInit() ;
	sei() ;
    while (1) 
    {
		
    }
}

void init_Tx(unsigned int ubbr)
{
	UBRRH = (unsigned char) (ubbr >> 8);							/* set baud rate */
	UBRRL = (unsigned char) ubbr;
	UCSRA = (1 << UDRE);											/* initial flag empty UDR register */
	UCSRB = (1 << TXCIE) | (1 << TXEN);							/* Enable Transmitter and TX complete interrupt */
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);				/* Set 8-bit frame data */
}

void init_Rx(void)
{
	UCSRB |= (1 << RXCIE) | (1 << RXEN) ;			/* Enable receiver and Rx complete interrupt */
}

void writeByte(char c)									/* This function writes one character at the time in the Transmission buffer each time is called */
{
	TX_buffer[TxwritePos] = c ;
	TxwritePos++ ;
	
	if(TxwritePos == BUFFERSIZE)						/* if we reached the maximum amount of  characters then reset pointer */
	{
		TxwritePos = 0;
	}
	
	if (UCSRA & (1 << UDRE))								/* To begin the transmission, we must send something that will not interact with the communication protocol, that will trigger the TX complete interrupt */
	{
		UDR = TX_buffer[TxreadPos] ;											/* So if UDR is empty there will not be a call of TX_complete interrupt routine to send the next character */
		TxreadPos ++ ;
	}
	
}

//void writeString(char s[])
//{
	//for(uint8_t i = 0; i < strlen(s); i++)					/* Copies the String into the Transmission buffer */
	//{
		//writeByte(s[i]) ;
	//}
	//
//} 

void readByte(char c)
{
	if (RxwritePos <= BUFFERSIZE)							/* The receiver reads each character */
	{														/* if the number of readed character is less than or equal with the length of the compared String */
		RX_buffer[RxwritePos] = c ;							/* then put each character into Receiver buffer */
		RxwritePos++ ;
	}
	
}

void timerInit(void)
{
	TCNT1 = 0 ;
	TCCR1B |= (1 << WGM12) | (1 << CS12) ;
	OCR1A = 31250 ;
	TIMSK |= (1 << OCIE1A) ;
}





uint8_t asciiToInteger(char c)
{
	uint8_t i = 0 ;
	switch(c)
	{
		case '0':
		i = 0 ;
		break;
		
		case '1':
		i = 1 ;
		break;
		
		case '2' :
		i = 2 ;
		break ;
		
		case '3' :
		i = 3 ;
		break ;
		
		case '4' :
		i = 4 ;
		break ;
		
		case '5' :
		i = 5 ;
		break ;
		
		case '6' :
		i = 6 ;
		break ;
		
		case '7' :
		i = 7 ;
		break ;
		
		case '8' :
		i = 8 ;
		break ;
		
		default:
		i = 9 ;
		break;
	}
	return i ;
	
}

void updateRxBuffer(char c[])
{
	for(int i = RxwritePos; i < BUFFERSIZE; i++)
	{
		RX_buffer[i] = 0 ;
	}
	RxwritePos = 0;
}

char intToAscii(uint8_t i)
{
	char c = '-' ;
	switch(i)
	{
		case 0:
		c = '0' ;
		break;
		
		case 1:
		c = '1' ;
		break;
		
		case 2 :
		c = '2' ;
		break ;
		
		case 3 :
		c = '3' ;
		break ;
		
		case 4 :
		c = '4' ;
		break ;
		
		case 5 :
		c = '5' ;
		break ;
		
		case 6 :
		c = '6' ;
		break ;
		
		case 7 :
		c = '7' ;
		break ;
		
		case 8 :
		c = '8' ;
		break ;
		
		default:
		c = '9' ;
		break;
	}
	return c ;
}

void examineInstr(char c[])
{
	if (strlen(c) == 2 && c[0] == 'A' && c[1] == 'T')
	{
		/* Simple answer ok */
		/*writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
		
	}
	else if (strlen(c) == 3 && c[0] == 'R' && c[1] == 'S' && c[2] == 'T')
	{
		/* Reset (warm start) */
		/* writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
	}
	else if (strlen(c) == 4 && c[0] == 'S' && c[1] == 'P' && c[2] == ' ' && (c[3] == 'B' || c[3] == 'W'))
	{
		/* Set players to black or white */
		if (c[3] == 'B')
		{
			/* Set player to black */
			/*writeString("\r") ; */
			writeByte('O') ;
			writeByte('K') ;
			writeByte('\r') ;
		}
		else
		{
			/* Set player to white */
			/*writeString("\r") ; */
			writeByte('O') ;
			writeByte('K') ;
			writeByte('\r') ;
		}
	}
	else if (strlen(c) == 2 && c[0] == 'N' && c[1] == 'G')
	{
		/* New game */
		/*writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
	}
	else if (strlen(c) == 2 && c[0] == 'E' && c[1] == 'G')
	{
		/* End game */
		/*writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
	}
	else if (strlen(c) == 4 && c[0] == 'S' && c[1] == 'T' && c[2] == ' ' && (asciiToInteger(c[3]) >= 1 && asciiToInteger(c[3]) <=9))
	{
		/* set time limit */
		TCCR1B &= 0 ;												/* Disable timer */
		timelimit = asciiToInteger(c[3]) ;							/* Change timing limit */
		TCCR1B |= (1 << WGM12) | (1 << CS12) ;						/* Renable timer */
		
	}
	else if (strlen(c) == 2 && c[0] == 'O' && c[1] == 'K')
	{
		/* begin timer */
		TIMSK |= (1 << TOIE1) ;
		MyMove() ;
		MyPass() ;
		
	}
	else if (strlen(c) == 5 && c[0] == 'M' && c[1] == 'V' && c[2] == ' ' && (c[3] >= 'A' && c[3] <= 'H') && (c[4] >= '1' && c[4] <= '8'))
	{
		/* Opponent move*/
		/*writeString("OK\r"); */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
	}
	else if (strlen(c) == 2 && c[0] == 'P' && c[1] == 'S')
	{
		/* Pass PC doesn't have any move μC will play again */
		/*writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
	}
	else if (strlen(c) == 2 && c[0] == 'P' && c[1] == 'L')
	{
		/* Reject */
	}
	else if (strlen(c) == 2 && c[0] == 'W' && c[1] == 'N')
	{
		/* You win */
		/*writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
	}
	else
	{
		Tie() ;	
	}
}

void MyMove(void)
{
	/* MyMove */
	/*writeString("MM B5\r") ; */
	writeByte('M') ;
	writeByte('M') ;
	writeByte(' ') ;
	writeByte('B') ;
	writeByte('5') ;
	writeByte('\r') ;
}

void MyPass(void)
{	/* I go pass */
	/*writeString("MP\r") ; */
	writeByte('M') ;
	writeByte('P') ;
	writeByte('\r') ;
}

void Win(void)
{
	/* I won the game */
	PORTB &= 0b11111110 ;										/* Enable LED0 */
	/*writeString("WN\r") ; */
	writeByte('W') ;
	writeByte('N') ;
	writeByte('\r') ;
}

void Lose(void)
{
	/* I lost the game */
	PORTB &= 0b11111101 ;
	/*writeString("LS\r") ; */
	writeByte('L') ;
	writeByte('S') ;
	writeByte('\r') ;
}

void Tie(void)
{
	/* Draw */
	PORTB &= 0b11111011 ;
	/*writeString("TE\r") ; */
	writeByte('T') ;
	writeByte('E') ;
	writeByte('\r') ;
}

void IllegalMove(void)
{
	/* the enemy made illegal move */
	/*writeString("IL\r") ;*/
	writeByte('I') ;
	writeByte('L') ;
	writeByte('\r') ;
}

void IllegalTime(void)
{
	/* The enemy has surpassed the timing limit */
	/*writeString("IT\r") ; */
	writeByte('I') ;
	writeByte('T') ;
	writeByte('\r') ;
	
}

void quit(void)
{
	/* Surrender */
	writeByte('Q') ;
	writeByte('T') ;
	writeByte('\r') ; 
}

void writeMemory(uint8_t letter, uint8_t j, char val)
{
	if (letter >= 65 && letter <= 72 && j > 0 && j < 9)
	{
		uint8_t i = letter - 65 ;
		uint8_t addr = (i << 3) + j - 1 ;
		Table[addr] = val ;
	}
}

char readMemory(uint8_t letter, uint8_t j)
{
	char c = ' ' ;
	if (letter >= 65 && letter <= 72 && j > 0 && j < 9)
	{
		uint8_t i = letter - 65 ;
		uint8_t addr = (i << 3) + j - 1 ;
		c = Table[addr] ;
	}
	
	return c ;
}

void initMemory(void)												/* '-' represents empty ceil */
{
	for(uint8_t i = 0; i < BOARDSIZE ; i++)							/* 'B' black token */
	{
		Table[i] = '-' ;											/* 'W' white token */
	}
}

