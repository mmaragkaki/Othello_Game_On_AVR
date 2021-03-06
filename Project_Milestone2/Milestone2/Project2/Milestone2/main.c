/*
 * Project1.c
 *
 * 
 * Team: LAB41140573
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#define FOSC 10000000
#define BAUD 9600
#define UBBRVALUE ((FOSC / 16) / BAUD) - 1
#define BUFFERSIZE 16										/* static buffer size for communication */
#define BOARDSIZE 64
#define SIZE 16
void init_Tx(unsigned int ubbr);
void writeByte(char c);
/*void writeString(char s[]);*/
void readByte(char c);
void init_Rx(void);
uint8_t asciiToInteger(char c);
char intToAscii(uint8_t i);
void examineInstr(char c[]);
void updateRxBuffer(char c[]);
void timerInit(void) ;
void MyMove(char c,uint8_t column) ;
void MyPass(void) ;
void Win(void) ;
void Lose(void) ;
void Tie(void) ;
void IllegalMove(void) ;
void IllegalTime(void) ;
void quit(void) ;
void tableInit(void);
void setChecker(char c,uint8_t column,char player);
uint8_t scanChessboard(char mine);
char readCell(char c,uint8_t column);
void checkAvailable(char c,uint8_t j,char player,char mine);
void insertMineValid(char c,uint8_t column);
void insertEnemyValid(char c,uint8_t column);
void firstValid(char mine);
void TurnOtherCheckers(char c,uint8_t column,char enemy,char player) ;
uint8_t checkEnemyMove(char c, uint8_t column);
void clearValidMoves(void) ;

char TX_buffer[BUFFERSIZE] ;									/* Buffer for transmission */
char RX_buffer[BUFFERSIZE] ;									/* Buffer for receiving */
char Table[BOARDSIZE] ;

uint8_t table[SIZE] ;
uint8_t mineValidtable[SIZE/2] ;
uint8_t enemyValidtable[SIZE/2] ;

uint8_t TxreadPos = 0 ;											/* initial values = 0 for the r/w pointers of each buffer */
uint8_t TxwritePos = 0 ;
uint8_t RxwritePos = 0 ;
uint8_t seconds = 0 ;
uint8_t timelimit = 2 ;
char enemy = ' ' ;
char mine = ' ' ;
char myLine = 'A' ;
uint8_t myColumn = 1 ; 

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
	//if (seconds >= timelimit)
	//{
		//seconds = 0 ;
		//PORTB ^= (1 << PB5) ;
	//}
}


int main(void)
{
    /* Replace with your application code */
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
	TCCR1B |= (1 << WGM12) ;
	OCR1A = 39062 ;
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
		/* Begin timer */
		//TCCR1B |= (1 << CS12) ;
		
	}
	else if (strlen(c) == 3 && c[0] == 'R' && c[1] == 'S' && c[2] == 'T')
	{
		/* Reset (warm start) */
		/* writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
		/*Begin timer */
		//TCCR1B |= (1 << CS12) ;
	}
	else if (strlen(c) == 4 && c[0] == 'S' && c[1] == 'P' && c[2] == ' ' && (c[3] == 'B' || c[3] == 'W'))
	{
		/* Set players to black or white */
		if (c[3] == 'B')
		{
			/* Set player to black */
			enemy = 'B';
			mine = 'W' ;
			/*writeString("\r") ; */
			writeByte('O') ;
			writeByte('K') ;
			writeByte('\r') ;
			/* Begin timer */
			//TCCR1B |= (1 << CS12) ;
		}
		else
		{
			/* Set player to white */
			enemy = 'W';
			mine = 'B';
			/*writeString("\r") ; */
			writeByte('O') ;
			writeByte('K') ;
			writeByte('\r') ;
			/* Begin timer */
			//TCCR1B |= (1 << CS12) ;
		}
	}
	else if (strlen(c) == 2 && c[0] == 'N' && c[1] == 'G')
	{
		/* New game */
		tableInit();
		scanChessboard(mine) ;
		/*writeString("OK\r") ; */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
		/* Begin timer */
		//TCCR1B |= (1 << CS12) ;
		
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
		TCCR1B &= 0b11111011 ;										/* Disable timer */
		timelimit = asciiToInteger(c[3]) ;							/* Change timing limit */
		writeByte('O') ;
		writeByte('K') ;
		writeByte('\r') ;
		TCCR1B |= (1 << CS12) ;										/* Renable timer */
		
	}
	else if (strlen(c) == 2 && c[0] == 'O' && c[1] == 'K')
	{
		/* Enemy sends the "OK" and starts counting my time */
		///* Disable timer */
		//TCCR1B &= 0b11111011 ;
		//TCNT1 = 0 ;
		//if (seconds > timelimit)									/* if the opponents exceeds the allowable timelimit then send illegal time */
		//{
			//IllegalTime() ;
		//}
		//seconds = 0 ;
		firstValid(mine) ;
		TurnOtherCheckers(myLine,myColumn,enemy,mine);
		clearValidMoves() ;
		scanChessboard(mine) ;
		MyMove(myLine,myColumn) ;
		/* I played my move. So the enemy stops counting my time and i started counting his time */
		TCCR1B |= (1 << CS12) ;
		
	}
	else if (strlen(c) == 5 && c[0] == 'M' && c[1] == 'V' && c[2] == ' ' && (c[3] >= 'A' && c[3] <= 'H') && (c[4] >= '1' && c[4] <= '8'))
	{
		/* Opponent move*/
		/* I am stopping counting his time. Disable timer by clearing prescaler */
		TCCR1B &= 0b11111011 ;
		/* Reset timer's value */
		TCNT1 = 0 ;
		/* Then I check if he exceeds the allowable timelimit */
		if (seconds > timelimit)
		{
			PORTB &= 0b11111011 ;
			IllegalTime() ;
		}
		else
		{
			PORTB |= (1 << PB5) ;
		}
		/* reset opponents time - thinking */
		seconds = 0 ;
		
		if (checkEnemyMove(c[3],asciiToInteger(c[4])) == 1)
		{
			setChecker(c[3],asciiToInteger(c[4]),enemy);
			TurnOtherCheckers(c[3],asciiToInteger(c[4]),mine,enemy) ;
			clearValidMoves() ;
			scanChessboard(mine) ;
			
			writeByte('O') ;
			writeByte('K') ;
			writeByte('\r') ;
		}else
		{
			IllegalMove();
		}

	}
	else if (strlen(c) == 2 && c[0] == 'P' && c[1] == 'S')
	{
		/* Pass PC doesn't have any move ????C will play again */
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

void MyMove(char c, uint8_t column)
{
	/* MyMove */
	/*writeString("MM B5\r") ; */
	writeByte('M') ;
	writeByte('M') ;
	writeByte(' ') ;
	writeByte(c) ;
	writeByte(intToAscii(column)) ;
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


void tableInit(void)
{
	for (uint8_t i = 0; i < SIZE; i++)
	{
		table[i] = 0 ;
	}
	clearValidMoves();
	setChecker('D',4,'W') ;
	setChecker('D',5,'B') ;
	setChecker('E',4,'B') ;
	setChecker('E',5,'W') ;
}

void setChecker(char c,uint8_t column,char player)
{
	uint8_t index = ((c - 65) << 1) ;										/* initial index calculation for byte - array */
	uint8_t bit = column - 1 ;												/* initial bit that we must enable - disable chess-table cell */
	uint8_t buffer = 0 ;													/* buffer to read the specific byte from memory */
	uint8_t temp = 0 ;
	if (column > 4)
	{
		index++ ;
		bit = bit - 4 ;														/* if column > 4 jump to the next memory position */
	}
	buffer = table[index] ;
	switch(player)
	{
		case 'B':														   /* case black: enable cell and valid bit */
		buffer |= (1 << bit) | (1 << (4 + bit)) ;
		break;
		default:														  /* case white: reset cell and enable valid bit */
		temp |= (1 << bit) ;
		temp = ~ temp ;
		buffer = buffer & temp ;
		buffer |= (1 << (4 + bit)) ;
		break;
	}
	table[index] = buffer ;
}

uint8_t scanChessboard(char mine)								/* This function scans the whole chessboard for white checkers */
{
	char c = 0 ;											/* For every one white checker checks for my available moves */
	uint8_t j = 0 ;											/* and stores them in a parallel memory board the validtable */
	uint8_t buffer = 0 ;
	uint8_t temp = 0 ;
	uint8_t count = 0 ;
	for (uint8_t index = 0; index < SIZE ; index++)
	{
		c = 65 + (index >> 1) ;								/* Convert the index into specific ascii that we want to scan */
		buffer = table[index] ;								/* Read the data-byte and store it into a buffer */
		temp = buffer & 0b00010000 ;						/* Find the column which depends on the enabled-bits */
		if (temp == 0b00010000)
		{
			j = 1 ;
			if (index % 2 == 1)								/* In the odd number indexes there are the second half-bytes */
			{                                               /* Opposite, in the even number indexes there are the first half-bytes */
				j = j + 4 ;
			}
			/* scan for black player available moves */
			if (readCell(c,j) == 'W')
			{
				count++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				checkAvailable(c,j,'W',mine) ;
			}
			
		}
		temp = buffer & 0b00100000 ;
		if (temp == 0b00100000)
		{
			j = 2 ;
			if (index % 2 == 1)
			{
				j = j + 4 ;
			}
			/* scan for black player available moves */
			if (readCell(c,j) == 'W')
			{
				count++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				checkAvailable(c,j,'W',mine) ;
			}
		}
		temp = buffer & 0b01000000 ;
		if (temp == 0b01000000)
		{
			j = 3 ;
			if (index % 2 == 1)
			{
				j = j + 4 ;
			}
			/* scan for black player available moves */
			if (readCell(c,j) == 'W')
			{
				count++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				checkAvailable(c,j,'W',mine) ;
			}
		}
		temp = buffer & 0b10000000 ;
		if (temp == 0b10000000)
		{
			j = 4 ;
			if (index % 2 == 1)
			{
				j = j + 4 ;
			}
			/* scan for black player available moves */
			if (readCell(c,j) == 'W')
			{
				count++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				checkAvailable(c,j,'W',mine) ;
			}
		}
		
	}
	return count ;
}

char readCell(char c,uint8_t column)
{
	uint8_t index = ((c - 65) << 1) ;								/* find appropriate index from ascii code */
	uint8_t bit = column - 1 ;										/* find initial bit position from the column */
	uint8_t buffer = 0 ;
	char retC = 'E' ;
	if (column > 4)													/* if column > jump to the next position for the half byte */
	{
		index++ ;
		bit = bit - 4 ;
	}
	buffer = table[index] ;											/* read data from memory cell and store into buffer */
	if (!(buffer & (1 << (4+bit))))									/* if the enable bit of this bit position is zero then this is empty position */
	{
		retC = 'E' ;
	}
	else if (buffer & (1 << bit))								   /* if enable-bit is 1 and the bit-pos is 1 then this is black */
	{
		retC = 'B' ;
	}
	else                                                           /* if only the enable-bit is asserted then this is white */
	{
		retC = 'W' ;
	}
	return retC ;
}

void checkAvailable(char c,uint8_t j,char player,char mine)
{
	uint8_t found = 0 ;
	uint8_t i = j ;
	uint8_t coltemp = j ;
	switch (player)
	{
		case 'B':
		/* check right of the enemy - checker for other my own checkers */
		i++ ;
		while (i <= 8 && found == 0 && readCell(c,i) != 'E')
		{
			
			if (readCell(c,i) == 'B')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && j-1 > 0)
		{
			if (readCell(c,j-1) == 'E')
			{
				/* Available move c, j-1 */						/* The valid move is one left */
				if (mine == player)								/* If the colour is mine */
				{												/* then save this available move as mine */
					insertMineValid(c,j-1) ;
				}
				else                                           /* else save this move as enemy available move */
				{
					insertEnemyValid(c,j-1) ;
				}
				
			}
		}
		
		found = 0 ;												/* check left of the enemy - checker for other my own checkers */
		i = j ;
		i-- ;
		while (i > 1 && found == 0 && readCell(c,i) != 'E')
		{
			
			if (readCell(c,i) == 'B')
			{
				found = 1 ;
			}
			i-- ;
			
		}
		if (found == 1 && j+1 < 9)
		{
			if (readCell(c,j+1) == 'E')
			{
				/* Available move c, j+1 */							/* The valid move is one right */
				if (mine == player)									/* If the colour is mine */
				{                                                   /* then save this move as mine available */
					insertMineValid(c,j+1) ;
				}
				else                                               /* else save it as opponents available move */
				{
					insertEnemyValid(c,j+1) ;
				}
			}
		}
		
		found = 0 ;											 /* check up of the enemy - checker for other my own checkers */
		i = c ;
		i-- ;
		while (i > '@' && found == 0 && readCell(i,j) != 'E')
		{
			
			if (readCell(i,j) == 'B')
			{
				found = 1 ;
			}
			i-- ;
		}
		if (found == 1 && c+1 < 'I')
		{
			if (readCell(c+1,j) == 'E')
			{
				/* Valid pos c+1, j */
				if (mine == player)
				{
					insertMineValid(c+1,j) ;
				}
				else
				{
					insertEnemyValid(c+1,j) ;
				}
			}
		}
		
		found = 0 ;										   /* check down of the enemy - checker for other my own checkers */
		i = c ;
		i++ ;
		while (i < 'I' && found == 0 && readCell(i,j) != 'E')
		{
			if (readCell(i,j) == 'B')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && c - 1 > '@')
		{
			if (readCell(c-1,j) == 'E')
			{
				/* Valid pos c-1,j */
				if (mine == player)
				{
					insertMineValid(c-1,j) ;
				}
				else
				{
					insertEnemyValid(c-1,j) ;
				}
			}
		}
		/* check main diagon up */
		found = 0 ;
		i = c ;
		i-- ;
		coltemp++ ;
		while (i > '@' && coltemp < 9 && (readCell(i,coltemp) != 'E') && found == 0)
		{
			if (readCell(i,coltemp) == 'B')
			{
				found = 1 ;
			}
			i-- ;
			coltemp++ ;
		}
		if (found == 1 && (c+1 < 'I') && (j-1 > 0))
		{
			if (readCell(c+1,j-1) == 'E')
			{
				/* Valid pos c+1,j-1 */
				if (mine == player)
				{
					insertMineValid(c+1,j-1) ;
				}
				else
				{
					insertEnemyValid(c+1,j-1) ;
				}
			}
		}
		
		/* Check main diagon down */
		found = 0 ;
		i = c ;
		coltemp = j ;
		i++ ;
		coltemp-- ;
		while((i < 'I') && (coltemp > 0) && (found == 0) && (readCell(i,coltemp) != 'E'))
		{
			if (readCell(i,coltemp) == 'B')
			{
				found = 1 ;
			}
			i++ ;
			coltemp-- ;
		}
		if (found == 1 && (c-1 > '@') && (j+1 < 9))
		{
			if (readCell(c-1,j+1) == 'E')
			{
				/* Valid pos c-1, j+1 */
				if (player == mine)
				{
					insertMineValid(c-1,j+1) ;
				}
				else
				{
					insertEnemyValid(c-1,j+1) ;
				}
			}
		}
		
		/* Check secondary diagonal up */
		found = 0 ;
		i = c ;
		coltemp = j ;
		i-- ;
		coltemp-- ;
		while((i > '@') && (coltemp > 0) && (readCell(i,coltemp) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp) == 'B')
			{
				found = 1 ;
			}
			i-- ;
			coltemp-- ;
		}
		if (found == 1 && (c+1 < 'I') && (j+1 < 9))
		{
			if (readCell(c+1,j+1) == 'E')
			{
				/* Valid move c+1,j+1 */
				if (player == mine)
				{
					insertMineValid(c+1,j+1) ;
				}
				else
				{
					insertEnemyValid(c+1,j+1) ;
				}
			}
		}
		
		/* Check secondary diagonal down */
		found = 0 ;
		i = c ;
		coltemp = j ;
		i++ ;
		coltemp++ ;
		while ((i < 'I') && (coltemp < 9) && (readCell(i,coltemp) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp) == 'B')
			{
				found = 1 ;
			}
			i++ ;
			coltemp++ ;
		}
		if ((found == 1) && (c-1 > '@') && (j-1 > 0))
		{
			if (readCell(c-1,j-1) == 'E')
			{
				/* Valid pos c-1,j-1 */
				if (player == mine)
				{
					insertMineValid(c-1,j-1) ;
				}
				else
				{
					insertEnemyValid(c-1,j-1) ;
				}
			}
		}
		
		break;
		/* check available moves for white player */
		default:
		/* check right of the enemy - checker for other my own checkers */
		i++ ;
		while (i <= 8 && found == 0 && readCell(c,i) != 'E')
		{
			
			if (readCell(c,i) == 'W')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && j-1 > 0)
		{
			if (readCell(c,j-1) == 'E')
			{
				/* Available move c, j-1 */						/* The valid move is one left */
				if (mine == player)
				{
					insertMineValid(c,j-1) ;
				}
				else
				{
					insertEnemyValid(c,j-1) ;
				}
			}
		}
		
		found = 0 ;												/* check left of the enemy - checker for other my own checkers */
		i = j ;
		i-- ;
		while (i > 1 && found == 0 && readCell(c,i) != 'E')
		{
			
			if (readCell(c,i) == 'W')
			{
				found = 1 ;
			}
			i-- ;
			
		}
		if (found == 1 && j+1 < 9)
		{
			if (readCell(c,j+1) == 'E')
			{
				/* Available move c, j+1 */					/* The valid move is one right */
				if (mine == player)
				{
					insertMineValid(c,j+1) ;
				}
				else
				{
					insertEnemyValid(c,j+1) ;
				}
			}
		}
		
		found = 0 ;											 /* check up of the enemy - checker for other my own checkers */
		i = c ;
		i-- ;
		while (i > '@' && found == 0 && readCell(i,j) != 'E')
		{
			
			if (readCell(i,j) == 'W')
			{
				found = 1 ;
			}
			i-- ;
		}
		if (found == 1 && c+1 < 'I')
		{
			if (readCell(c+1,j) == 'E')
			{
				/* Valid pos c+1, j */
				if (mine == player)
				{
					insertMineValid(c+1,j) ;
				}
				else
				{
					insertEnemyValid(c+1,j) ;
				}
			}
		}
		
		found = 0 ;										   /* check down of the enemy - checker for other my own checkers */
		i = c ;
		i++ ;
		while (i < 'I' && found == 0 && readCell(i,j) != 'E')
		{
			if (readCell(i,j) == 'W')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && c - 1 > '@')
		{
			if (readCell(c-1,j) == 'E')
			{
				/* Valid pos c-1,j */
				if (mine == player)
				{
					insertMineValid(c-1,j) ;
				}
				else
				{
					insertEnemyValid(c-1,j) ;
				}
			}
		}
		
		/* check main diagonal up */
		found = 0 ;
		i = c ;
		i-- ;
		coltemp++ ;
		while (i > '@' && coltemp < 9 && (readCell(i,coltemp) != 'E') && found == 0)
		{
			if (readCell(i,coltemp) == 'W')
			{
				found = 1 ;
			}
			i-- ;
			coltemp++ ;
		}
		if (found == 1 && (c+1 < 'I') && (j-1 > 0))
		{
			if (readCell(c+1,j-1) == 'E')
			{
				/* Valid pos c+1,j-1 */
				if (mine == player)
				{
					insertMineValid(c+1,j-1) ;
				}
				else
				{
					insertEnemyValid(c+1,j-1) ;
				}
			}
		}
		
		/* Check main diagonal down */
		found = 0 ;
		i = c ;
		coltemp = j ;
		i++ ;
		coltemp-- ;
		while((i < 'I') && (coltemp > 0) && (found == 0) && (readCell(i,coltemp) != 'E'))
		{
			if (readCell(i,coltemp) == 'W')
			{
				found = 1 ;
			}
			i++ ;
			coltemp-- ;
		}
		if (found == 1 && (c-1 > '@') && (j+1 < 9))
		{
			if (readCell(c-1,j+1) == 'E')
			{
				/* Valid pos c-1, j+1 */
				if (player == mine)
				{
					insertMineValid(c-1,j+1) ;
				}
				else
				{
					insertEnemyValid(c-1,j+1) ;
				}
			}
		}
		
		/* Check secondary diagonal up */
		found = 0 ;
		i = c ;
		coltemp = j ;
		i-- ;
		coltemp-- ;
		while((i > '@') && (coltemp > 0) && (readCell(i,coltemp) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp) == 'W')
			{
				found = 1 ;
			}
			i-- ;
			coltemp-- ;
		}
		if (found == 1 && (c+1 < 'I') && (j+1 < 9))
		{
			if (readCell(c+1,j+1) == 'E')
			{
				/* Valid move c+1,j+1 */
				if (player == mine)
				{
					insertMineValid(c+1,j+1) ;
				}
				else
				{
					insertEnemyValid(c+1,j+1) ;
				}
			}
		}
		
		/* Check secondary diagonal down */
		found = 0 ;
		i = c ;
		coltemp = j ;
		i++ ;
		coltemp++ ;
		while ((i < 'I') && (coltemp < 9) && (readCell(i,coltemp) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp) == 'W')
			{
				found = 1 ;
			}
			i++ ;
			coltemp++ ;
		}
		if ((found == 1) && (c-1 > '@') && (j-1 > 0))
		{
			if (readCell(c-1,j-1) == 'E')
			{
				/* Valid pos c-1,j-1 */
				if (player == mine)
				{
					insertMineValid(c-1,j-1) ;
				}
				else
				{
					insertEnemyValid(c-1,j-1) ;
				}
			}
		}
		
		break;
	}
	
}


uint8_t checkEnemyMove(char c, uint8_t column)
{
	uint8_t index = c - 65 ;
	uint8_t bit = column - 1 ;
	uint8_t buffer = enemyValidtable[index] ;
	buffer = (buffer >> bit);
	buffer &= 0b00000001 ;
	if (buffer == 1)
	{
		return 1;
	}else
	{
		return 0;
	}
}

void insertMineValid(char c,uint8_t column)					/* This stores a mine valid move into the validtable */
{
	uint8_t index = c - 65 ;
	uint8_t bit = column - 1 ;
	uint8_t buffer = mineValidtable[index] ;
	buffer |= (1 << bit) ;
	mineValidtable[index] = buffer ;
}

void insertEnemyValid(char c,uint8_t column)					/* This stores a mine valid move into the validtable */
{
	uint8_t index = c - 65 ;
	uint8_t bit = column - 1 ;
	uint8_t buffer = enemyValidtable[index] ;
	buffer |= (1 << bit) ;
	enemyValidtable[index] = buffer ;
}


void firstValid(char mine)									/* This is a stupid player. It plays the first legal move */
{
	uint8_t buffer = 0 ;
	uint8_t temp = 0 ;
	uint8_t column = 0 ;
	uint8_t found = 0 ;
	uint8_t index = 0 ;
	char c = index + 65 ;
	while ((index < (SIZE / 2)) && (found == 0))
	{
		c = index + 65 ;
		buffer = mineValidtable[index] ;
		temp = buffer & 0b00000001 ;
		if (temp == 0b00000001 && found == 0)
		{
			column = 1;
			found = 1 ;
		}
		temp = buffer & 0b00000010 ;
		if (temp == 0b00000010 && found == 0)
		{
			column = 2 ;
			found = 1 ;
			
		}
		temp = buffer & 0b00000100 ;
		if (temp == 0b00000100 && found == 0)
		{
			column = 3;
			found = 1 ;
		}
		temp = buffer & 0b00001000 ;
		if (temp == 0b00001000 && found == 0)
		{
			column = 4 ;
			found = 1 ;
			
		}
		temp = buffer & 0b00010000 ;
		if (temp == 0b00010000 && found == 0)
		{
			column = 5 ;
			found = 1 ;
			
		}
		temp = buffer & 0b00100000 ;
		if (temp == 0b00100000 && found == 0)
		{
			column = 6 ;
			found = 1 ;
			
		}
		temp = buffer & 0b01000000 ;
		if (temp == 0b01000000 && found == 0)
		{
			column = 7 ;
			found = 1 ;
			
		}
		temp = buffer & 0b10000000 ;
		if (temp == 0b10000000 && found == 0)
		{
			column = 8 ;
			found = 1 ;
			
		}
		index++ ;
	}
	setChecker(c,column,mine);
	myLine = c ;
	myColumn = column ;
}

void TurnOtherCheckers(char c,uint8_t column,char enemy,char player)
{
	char row = c;
	uint8_t col = column;
	uint8_t found = 0;
	/*Check for players checkers from right*/
	col++;
	while (col < 9 && readCell(row,col) == enemy && found == 0)
	{
		if (col+1 < 9)
		{
			if(readCell(row,(col+1)) == player)
			{
				found = 1;
			}
		}
		col++;
	}
	if(found == 1 && (col < 9))
	{
		for(uint8_t i = (column + 1); i < col; i++)
		{
			setChecker(c,i,player);
		}
	}
	/*Check for players checkers from left*/
	found = 0 ;
	col = column;
	col--;
	while (col > 0 && readCell(row,col) == enemy && found == 0)
	{
		if (col-1 > 0)
		{
			if(readCell(row,(col-1)) == player)
			{
				found = 1;
			}
		}
		col--;
	}
	if(found == 1 && (column - 1 > 0))
	{
		for(uint8_t i = (column -1); i > col; i--)
		{
			setChecker(c,i,player);
		}
	}
	/*Check for players checkers from up*/
	found = 0 ;
	col = column;
	row-- ;
	while (row > '@' && readCell(row,col) == enemy && found == 0)
	{
		if (row - 1 > '@')
		{
			if(readCell((row-1),col) == player)
			{
				found = 1;
			}
		}
		row--;
	}
	if(found == 1 && (row > '@'))
	{
		for(uint8_t i = (c -1); i > row; i--)
		{
			setChecker(i,col,player);
		}
	}
	/*Check for players checkers from down*/
	found = 0 ;
	row = c;
	col = column;
	row++ ;
	while (row < 'I' && readCell(row,col) == enemy && found == 0)
	{
		if (row + 1 < 'I')
		{
			if(readCell((row+1),col) == player)
			{
				found = 1;
			}
		}
		row++;
	}
	if(found == 1 && (row < 'I'))
	{
		for(uint8_t i = c+1; i < row; i++)
		{
			setChecker(i,col,player);
		}
	}
	
	/*Check for players checkers from main diagon up*/
	found = 0 ;
	row = c;
	col = column;
	row--;
	col++;
	while ((row > '@' && col < 9) && readCell(row,col) == enemy && found != 1)
	{
		if ((row - 1 > '@') && (col + 1 < 9))
		{
			if(readCell((row-1),(col+1)) == player)
			{
				found = 1;
			}
		}
		row--;
		col++;
	}
	col = column;
	col++ ;
	if(found == 1 && (row > '@') && (col < 9))
	{
		for(uint8_t i = c - 1; i > row; i--)
		{
			setChecker(i,col,player);
			col++;
		}
	}
	/*Check for players checkers from main diagon down*/
	found = 0 ;
	row = c;
	col = column;
	row++;
	col--;
	while ((row < 'I' && col > 0) && readCell(row,col) == enemy && found != 1)
	{
		if ((row + 1 < 'I') && (col - 1 > 0))
		{
			if(readCell((row+1),(col-1)) == player)
			{
				found = 1;
			}
		}
		row++;
		col--;
	}
	col = column;
	col-- ;
	if(found == 1 && (row < 'I') && (col > 0))
	{
		for(uint8_t i = c+1; ((i < row) && (col > 0));  i++)
		{
			setChecker(i,col,player);
			col--;
		}
	}
	/*Check for players checkers from secondary diagon up*/
	found = 0 ;
	row = c;
	col = column;
	row--;
	col--;
	while ((row > '@' && col > 0) && readCell(row,col) == enemy && found != 1)
	{
		if ((row - 1 > '@') && (col - 1 > 0))
		{
			if(readCell((row-1),(col-1)) == player)
			{
				found = 1;
			}
		}
		row--;
		col--;
	}
	col = column;
	col-- ;
	if(found == 1 && (row > '@') && (col > 0))
	{
		for(uint8_t i = c-1; ((i > row) && (col > 0)); i--)
		{
			setChecker(i,col,player);
			col--;
		}
	}
	/*Check for players checkers from secondary diagon down*/
	found = 0 ;
	row = c;
	col = column;
	row++;
	col++;
	while ((row < 'I' && col < 9) && readCell(row,col) == enemy && found != 1)
	{
		if ((row+1 < 'I') && (col + 1 < 9))
		{
			if(readCell((row+1),(col+1)) == player)
			{
				found = 1;
			}
		}
		row++;
		col++;
	}
	col = column;
	col++ ;
	if(found == 1 && (row < 'I') && (col < 9))
	{
		for(uint8_t i = c+1; ((i < row) && (col < 9)); i++)
		{
			setChecker(i,col,player);
			col++;
		}
	}
}

void clearValidMoves(void)
{
	for (uint8_t i = 0; i < SIZE / 2; i++)
	{
		mineValidtable[i] = 0 ;
		enemyValidtable[i] = 0 ;
	}
}

