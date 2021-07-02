/*
 * NewTest.c
 *
 * Created: 24/4/2019 1:15:33 Ο€ΞΌ
 * Author : TEAM
 */ 
#include <stdio.h>
#include <avr/io.h>
#define SIZE 16
void tableInit(void);
void setBlack(char c,uint8_t column);
void setChecker(char c,uint8_t column,char player,uint8_t chessboard[]);
char readCell(char c,uint8_t column,uint8_t chessboard[]);
void scanChessboard(char mine) ;
void checkAvailable(char c,uint8_t j,char player,char mine) ;
void insertMineValid(char c,uint8_t column) ;
void insertEnemyValid(char c,uint8_t column) ;
void firstValid(char mine) ;
void TurnOtherCheckers(char c,uint8_t column,char enemy,char player,uint8_t chessboard[]) ;
uint8_t checkEnemyMove(char c, uint8_t column) ;
void clearValidMoves(void) ;
uint8_t countMineValidmoves(void) ;
uint8_t scanCopyChessBoard(char mine,uint8_t chessBoard[]) ;
void Tactics(char mine) ;

uint8_t table[SIZE] ;

uint8_t table1[SIZE] ;											/* Parallel chessboards */
uint8_t table2[SIZE] ;
uint8_t table3[SIZE] ;
uint8_t table4[SIZE] ;
uint8_t table5[SIZE] ;
uint8_t table6[SIZE] ;
uint8_t table7[SIZE] ;
uint8_t table8[SIZE] ;
uint8_t table9[SIZE] ;


uint8_t mineValidtable[SIZE/2] ;
uint8_t enemyValidtable[SIZE/2] ;

uint8_t blackCount = 0 ;
uint8_t whiteCount = 0 ;

char mine = ' ' ;
char enemy = ' ' ;

char myLine = 'A' ;
uint8_t myColumn = 1 ;

int main(void)
{
	tableInit() ;
	mine = 'W' ;
	enemy = 'B' ;
	DDRB = 255 ; 
	PORTB = 0 ;
	
	scanChessboard(mine) ;
	
	if (checkEnemyMove('E',6) == 1)
	{
		setChecker('E',6,enemy,table) ;
		TurnOtherCheckers('E',6,mine,enemy,table) ;
	}
	clearValidMoves() ;
	scanChessboard(mine) ;
	Tactics(mine) ;
	
	if (myLine == 'F' && myColumn == 4)
	{
		PORTB |= 255 ;
	}
	else if (myLine == 'F' && myColumn == 6)
	{
		PORTB |= 255 ;
	}
	else if (myLine == 'D' && myColumn == 6)
	{
		PORTB |= 255 ;
	}
	else if (myLine == 'H' && myColumn == 5)
	{
		PORTB |= 255 ;
	}
	else
	{
		PORTB |= 0 ;
	}
	
	
	
	
	
	
	
    while (1) 
    {
    }
}



void tableInit(void)
{
	for (uint8_t i = 0; i < SIZE; i++)
	{
		table[i] = 0 ;
	}
	clearValidMoves() ;
	setChecker('D',4,'W',table) ;
	setChecker('D',5,'B',table) ;
	setChecker('E',4,'B',table) ;
	setChecker('E',5,'W',table) ;
}

void setChecker(char c,uint8_t column,char player,uint8_t chessboard[])
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
	buffer = chessboard[index] ;
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
	chessboard[index] = buffer ;
}

char readCell(char c,uint8_t column,uint8_t chessboard[])
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
	buffer = chessboard[index] ;											/* read data from memory cell and store into buffer */
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

void scanChessboard(char mine)								/* This function scans the whole chessboard for white checkers */
{
	char c = 0 ;											/* For every one white checker checks for my available moves */
	uint8_t j = 0 ;											/* and stores them in a parallel memory board the validtable */
	uint8_t buffer = 0 ;
	uint8_t temp = 0 ;
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
			if (readCell(c,j,table) == 'W')
			{
				whiteCount++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				blackCount++ ;
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
			if (readCell(c,j,table) == 'W')
			{
				whiteCount++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				blackCount++ ;
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
			if (readCell(c,j,table) == 'W')
			{
				whiteCount++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				blackCount++ ;
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
			if (readCell(c,j,table) == 'W')
			{
				whiteCount++ ;
				checkAvailable(c,j,'B',mine) ;
			}
			/* scan for white player available moves */
			else
			{
				blackCount++ ;
				checkAvailable(c,j,'W',mine) ;
			}
		}
		
	}
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
		while (i <= 8 && found == 0 && readCell(c,i,table) != 'E')
		{
			
			if (readCell(c,i,table) == 'B')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && j-1 > 0)
		{
			if (readCell(c,j-1,table) == 'E')
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
		while (i > 1 && found == 0 && readCell(c,i,table) != 'E')
		{
			
			if (readCell(c,i,table) == 'B')
			{
				found = 1 ;
			}
			i-- ;
			
		}
		if (found == 1 && j+1 < 9)
		{
			if (readCell(c,j+1,table) == 'E')
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
		while (i > '@' && found == 0 && readCell(i,j,table) != 'E')
		{
			
			if (readCell(i,j,table) == 'B')
			{
				found = 1 ;
			}
			i-- ;
		}
		if (found == 1 && c+1 < 'I')
		{
			if (readCell(c+1,j,table) == 'E')
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
		while (i < 'I' && found == 0 && readCell(i,j,table) != 'E')
		{
			if (readCell(i,j,table) == 'B')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && c - 1 > '@')
		{
			if (readCell(c-1,j,table) == 'E')
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
		while (i > '@' && coltemp < 9 && (readCell(i,coltemp,table) != 'E') && found == 0)
		{
			if (readCell(i,coltemp,table) == 'B')
			{
				found = 1 ;
			}
			i-- ;
			coltemp++ ;
		}
		if (found == 1 && (c+1 < 'I') && (j-1 > 0))
		{
			if (readCell(c+1,j-1,table) == 'E')
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
		while((i < 'I') && (coltemp > 0) && (found == 0) && (readCell(i,coltemp,table) != 'E'))
		{
			if (readCell(i,coltemp,table) == 'B')
			{
				found = 1 ;
			}
			i++ ;
			coltemp-- ;
		}
		if (found == 1 && (c-1 > '@') && (j+1 < 9))
		{
			if (readCell(c-1,j+1,table) == 'E')
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
		while((i > '@') && (coltemp > 0) && (readCell(i,coltemp,table) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp,table) == 'B')
			{
				found = 1 ;
			}
			i-- ;
			coltemp-- ;
		}
		if (found == 1 && (c+1 < 'I') && (j+1 < 9))
		{
			if (readCell(c+1,j+1,table) == 'E')
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
		while ((i < 'I') && (coltemp < 9) && (readCell(i,coltemp,table) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp,table) == 'B')
			{
				found = 1 ;
			}
			i++ ;
			coltemp++ ;
		}
		if ((found == 1) && (c-1 > '@') && (j-1 > 0))
		{
			if (readCell(c-1,j-1,table) == 'E')
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
		while (i <= 8 && found == 0 && readCell(c,i,table) != 'E')
		{
			
			if (readCell(c,i,table) == 'W')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && j-1 > 0)
		{
			if (readCell(c,j-1,table) == 'E')
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
		while (i > 1 && found == 0 && readCell(c,i,table) != 'E')
		{
			
			if (readCell(c,i,table) == 'W')
			{
				found = 1 ;
			}
			i-- ;
			
		}
		if (found == 1 && j+1 < 9)
		{
			if (readCell(c,j+1,table) == 'E')
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
		while (i > '@' && found == 0 && readCell(i,j,table) != 'E')
		{
			
			if (readCell(i,j,table) == 'W')
			{
				found = 1 ;
			}
			i-- ;
		}
		if (found == 1 && c+1 < 'I')
		{
			if (readCell(c+1,j,table) == 'E')
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
		while (i < 'I' && found == 0 && readCell(i,j,table) != 'E')
		{
			if (readCell(i,j,table) == 'W')
			{
				found = 1 ;
			}
			i++ ;
		}
		if (found == 1 && c - 1 > '@')
		{
			if (readCell(c-1,j,table) == 'E')
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
		while (i > '@' && coltemp < 9 && (readCell(i,coltemp,table) != 'E') && found == 0)
		{
			if (readCell(i,coltemp,table) == 'W')
			{
				found = 1 ;
			}
			i-- ;
			coltemp++ ;
		}
		if (found == 1 && (c+1 < 'I') && (j-1 > 0))
		{
			if (readCell(c+1,j-1,table) == 'E')
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
		while((i < 'I') && (coltemp > 0) && (found == 0) && (readCell(i,coltemp,table) != 'E'))
		{
			if (readCell(i,coltemp,table) == 'W')
			{
				found = 1 ;
			}
			i++ ;
			coltemp-- ;
		}
		if (found == 1 && (c-1 > '@') && (j+1 < 9))
		{
			if (readCell(c-1,j+1,table) == 'E')
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
		while((i > '@') && (coltemp > 0) && (readCell(i,coltemp,table) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp,table) == 'W')
			{
				found = 1 ;
			}
			i-- ;
			coltemp-- ;
		}
		if (found == 1 && (c+1 < 'I') && (j+1 < 9))
		{
			if (readCell(c+1,j+1,table) == 'E')
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
		while ((i < 'I') && (coltemp < 9) && (readCell(i,coltemp,table) != 'E') && (found == 0))
		{
			if (readCell(i,coltemp,table) == 'W')
			{
				found = 1 ;
			}
			i++ ;
			coltemp++ ;
		}
		if ((found == 1) && (c-1 > '@') && (j-1 > 0))
		{
			if (readCell(c-1,j-1,table) == 'E')
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
	setChecker(c,column,mine,table);
	myLine = c ;
	myColumn = column ;
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



void TurnOtherCheckers(char c,uint8_t column,char enemy,char player,uint8_t chessboard[])
{
	char row = c;
	uint8_t col = column;
	uint8_t found = 0;
	/*Check for players checkers from right*/
	col++;
	while (col < 9 && readCell(row,col,chessboard) == enemy && found == 0)
	{
		if (col+1 < 9)
		{
			if(readCell(row,(col+1),chessboard) == player)
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
			setChecker(c,i,player,chessboard);
		}
	}
	/*Check for players checkers from left*/
	found = 0 ;
	col = column;
	col--;
	while (col > 0 && readCell(row,col,chessboard) == enemy && found == 0)
	{
		if (col-1 > 0)
		{
			if(readCell(row,(col-1),chessboard) == player)
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
			setChecker(c,i,player,chessboard);
		}
	}
	/*Check for players checkers from up*/
	found = 0 ;
	col = column;
	row-- ;
	while (row > '@' && readCell(row,col,chessboard) == enemy && found == 0)
	{
		if (row - 1 > '@')
		{
			if(readCell((row-1),col,chessboard) == player)
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
			setChecker(i,col,player,chessboard);
		}
	}
	/*Check for players checkers from down*/
	found = 0 ;
	row = c;
	col = column;
	row++ ;
	while (row < 'I' && readCell(row,col,chessboard) == enemy && found == 0)
	{
		if (row + 1 < 'I')
		{
			if(readCell((row+1),col,chessboard) == player)
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
			setChecker(i,col,player,chessboard);
		}
	}
	
	/*Check for players checkers from main diagon up*/
	found = 0 ;
	row = c;
	col = column;
	row--;
	col++;
	while ((row > '@' && col < 9) && readCell(row,col,chessboard) == enemy && found != 1)
	{
		if ((row - 1 > '@') && (col + 1 < 9))
		{
			if(readCell((row-1),(col+1),chessboard) == player)
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
			setChecker(i,col,player,chessboard);
			col++;
		}
	}
	/*Check for players checkers from main diagon down*/
	found = 0 ;
	row = c;
	col = column;
	row++;
	col--;
	while ((row < 'I' && col > 0) && readCell(row,col,chessboard) == enemy && found != 1)
	{
		if ((row + 1 < 'I') && (col - 1 > 0))
		{
			if(readCell((row+1),(col-1),chessboard) == player)
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
			setChecker(i,col,player,chessboard);
			col--;
		}
	}
	/*Check for players checkers from secondary diagon up*/
	found = 0 ;
	row = c;
	col = column;
	row--;
	col--;
	while ((row > '@' && col > 0) && readCell(row,col,chessboard) == enemy && found != 1)
	{
		if ((row - 1 > '@') && (col - 1 > 0))
		{
			if(readCell((row-1),(col-1),chessboard) == player)
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
			setChecker(i,col,player,chessboard);
			col--;
		}
	}
	/*Check for players checkers from secondary diagon down*/
	found = 0 ;
	row = c;
	col = column;
	row++;
	col++;
	while ((row < 'I' && col < 9) && readCell(row,col,chessboard) == enemy && found != 1)
	{
		if ((row+1 < 'I') && (col + 1 < 9))
		{
			if(readCell((row+1),(col+1),chessboard) == player)
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
			setChecker(i,col,player,chessboard);
			col++;
		}
	}
}

void clearValidMoves(void)
{
	blackCount = 0 ;
	whiteCount = 0 ;
	for (uint8_t i = 0; i < SIZE / 2; i++)
	{
		mineValidtable[i] = 0 ;
		enemyValidtable[i] = 0 ;
	}
}

uint8_t countMineValidmoves(void)
{
	uint8_t count = 0 ;
	uint8_t buffer = 0 ;
	uint8_t temp = 0 ;
	uint8_t i = 0 ;
	while (i < SIZE/2 && count == 0)
	{
		buffer = mineValidtable[i] ;
		temp = buffer & 0b00000001 ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b00000010 ;
		temp = (temp >> 1) ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b00000100 ;
		temp = (temp >> 2) ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b00001000 ;
		temp = (temp >> 3) ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b00010000 ;
		temp = (temp >> 4) ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b00100000 ;
		temp = (temp >> 5) ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b01000000 ;
		temp = (temp >> 6) ;
		if (temp == 1)
		{
			count++ ;
		}
		temp = buffer & 0b10000000 ;
		temp = (temp >> 7) ;
		if (temp == 1)
		{
			count++ ;
		}
		
		i++ ;
	}
	return count ;
}

void Tactics(char mine)
{
	uint8_t buffer = 0 ;
	uint8_t temp = 0;
	uint8_t count = 0 ;
	uint8_t max = 0 ;
	uint8_t checkers = 0 ;
	char line = (char) 0 ;
	uint8_t column = 0 ;
	for (uint8_t index = 0; index < SIZE/2; index++)
	{
		/* First check line */
		line = 65 + index ;
		buffer = mineValidtable[index] ;
		/* Check column */
		temp = buffer & 0b00000001 ;
		if (temp == 1)
		{
			/* First Valid in this line */
			column = 1 ;
			count++ ;
			switch(count)
			{
				case 1:
					setChecker(line,column,mine,table1) ;
					TurnOtherCheckers(line,column,enemy,mine,table1) ;
					checkers = scanCopyChessBoard(mine,table1) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 2:
					setChecker(line,column,mine,table2) ;
					TurnOtherCheckers(line,column,enemy,mine,table2) ;
					checkers = scanCopyChessBoard(mine,table2) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 3:
					setChecker(line,column,mine,table3) ;
					TurnOtherCheckers(line,column,enemy,mine,table3) ;
					checkers = scanCopyChessBoard(mine,table3) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 4:
					setChecker(line,column,mine,table4) ;
					TurnOtherCheckers(line,column,enemy,mine,table4) ;
					checkers = scanCopyChessBoard(mine,table4) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 5:
					setChecker(line,column,mine,table5) ;
					TurnOtherCheckers(line,column,enemy,mine,table5) ;
					checkers = scanCopyChessBoard(mine,table5) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 6:
					setChecker(line,column,mine,table6) ;
					TurnOtherCheckers(line,column,enemy,mine,table6) ;
					checkers = scanCopyChessBoard(mine,table6) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 7:
					setChecker(line,column,mine,table7) ;
					TurnOtherCheckers(line,column,enemy,mine,table7) ;
					checkers = scanCopyChessBoard(mine,table7) ;
					if (max < checkers)
					{
						max = checkers ;
						myLine = line ;
						myColumn = column ;
					}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
			/*	case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break; 
				*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
		
		/* Second valid in this line */
		temp = buffer & 0b00000010 ;
		temp = (temp >> 1) ;
		if (temp == 1)
		{
			column = 2 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
			/*	case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
		/* Third valid move in this line */
		temp = buffer & 0b00000100 ;
		temp = (temp >> 2) ;
		if (temp == 1)
		{
			
			column = 3 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
		/*		case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				*/
				
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
		
		/* Fourth valid move in this line */
		temp = buffer & 0b00001000 ;
		temp = (temp >> 3) ;
		if (temp == 1)
		{
			column = 4 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
		/*		case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
		
		/* Fifth valid move in this line */
		temp = buffer & 0b00010000 ;
		temp = (temp >> 4) ;
		if (temp == 1)
		{
			column = 5 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
		/*		case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
		
		/* Sixth valid move in this line */
		temp = buffer & 0b00100000 ;
		temp = (temp >> 5) ;
		if (temp == 1)
		{
			column = 6 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
			/*	case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
			*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
		
		/* Seventh valid move in this line */
		temp = buffer & 0b01000000 ;
		temp = (temp >> 6) ;
		if (temp == 1)
		{
			column = 7 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
		/*		case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
				
		}
		
		/* Eighth valid move in this line */
		temp = buffer & 0b10000000 ;
		temp = (temp >> 7) ;
		if (temp == 1)
		{
			column = 8 ;
			count++ ;
			switch(count)
			{
				case 1:
				setChecker(line,column,mine,table1) ;
				TurnOtherCheckers(line,column,enemy,mine,table1) ;
				checkers = scanCopyChessBoard(mine,table1) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 2:
				setChecker(line,column,mine,table2) ;
				TurnOtherCheckers(line,column,enemy,mine,table2) ;
				checkers = scanCopyChessBoard(mine,table2) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 3:
				setChecker(line,column,mine,table3) ;
				TurnOtherCheckers(line,column,enemy,mine,table3) ;
				checkers = scanCopyChessBoard(mine,table3) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 4:
				setChecker(line,column,mine,table4) ;
				TurnOtherCheckers(line,column,enemy,mine,table4) ;
				checkers = scanCopyChessBoard(mine,table4) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 5:
				setChecker(line,column,mine,table5) ;
				TurnOtherCheckers(line,column,enemy,mine,table5) ;
				checkers = scanCopyChessBoard(mine,table5) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 6:
				setChecker(line,column,mine,table6) ;
				TurnOtherCheckers(line,column,enemy,mine,table6) ;
				checkers = scanCopyChessBoard(mine,table6) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 7:
				setChecker(line,column,mine,table7) ;
				TurnOtherCheckers(line,column,enemy,mine,table7) ;
				checkers = scanCopyChessBoard(mine,table7) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 8:
				setChecker(line,column,mine,table8) ;
				TurnOtherCheckers(line,column,enemy,mine,table8) ;
				checkers = scanCopyChessBoard(mine,table8) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
			/*	case 9:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 10:
				setChecker(line,column,mine,table10) ;
				TurnOtherCheckers(line,column,enemy,mine,table10) ;
				checkers = scanCopyChessBoard(mine,table10) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 11:
				setChecker(line,column,mine,table11) ;
				TurnOtherCheckers(line,column,enemy,mine,table11) ;
				checkers = scanCopyChessBoard(mine,table11) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 12:
				setChecker(line,column,mine,table12) ;
				TurnOtherCheckers(line,column,enemy,mine,table12) ;
				checkers = scanCopyChessBoard(mine,table12) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 13:
				setChecker(line,column,mine,table13) ;
				TurnOtherCheckers(line,column,enemy,mine,table13) ;
				checkers = scanCopyChessBoard(mine,table13) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				case 14:
				setChecker(line,column,mine,table14) ;
				TurnOtherCheckers(line,column,enemy,mine,table14) ;
				checkers = scanCopyChessBoard(mine,table14) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
				
				*/
				default:
				setChecker(line,column,mine,table9) ;
				TurnOtherCheckers(line,column,enemy,mine,table9) ;
				checkers = scanCopyChessBoard(mine,table9) ;
				if (max < checkers)
				{
					max = checkers ;
					myLine = line ;
					myColumn = column ;
				}
				break;
			}
			
		}
	}
}

uint8_t scanCopyChessBoard(char mine,uint8_t chessBoard[])
{
	uint8_t buffer = 0 ;
	uint8_t temp1 = 0 ;
	uint8_t temp2 = 0 ;
	uint8_t mineCount = 0 ;
	
	for (uint8_t index = 0 ; index < SIZE ; index++)
	{
		buffer = chessBoard[index] ;
		/* First column in half byte */
		temp1 = buffer & 0b00010000 ;								/* Checks for enabled position in chessboard */
		temp1 = (temp1 >> 4) ;
		if (temp1 == 1)
		{
			temp2 = buffer & 0b00000001 ;							/* Checks the colour of this checker */
			if (temp2 == 1)
			{
				/* Black checker */
				if (mine == 'B')
				{
					mineCount++ ;
				}
			}
			else
			{
				/* White checker */
				if (mine == 'W')
				{
					mineCount++ ;
				}
			}
		}
		
		/* Second column in half byte */
		temp1 = buffer & 0b00100000 ;
		temp1 = (temp1 >> 5) ;
		if (temp1 == 1)
		{
			/* Enabled position. There is a checker */
			temp2 = buffer & 0b00000010 ;
			temp2 = (temp2 >> 1) ;
			if (temp2 == 1)
			{
				/* Black checker */
				if (mine == 'B')
				{
					mineCount++ ;
				}
			}
			else
			{
				/* White checker */
				if (mine == 'W')
				{
					mineCount++ ;
				}
			}
		}
		
		/* Third column in half byte */
		temp1 = buffer & 0b01000000 ;
		temp1 = (temp1 >> 6) ;
		if (temp1 == 1)
		{
			/* There is a checker */
			temp2 = buffer & 0b00000100 ;
			temp2 = (temp2 >> 2) ;
			if (temp2 == 1)
			{
				/* Black checker */
				if (mine == 'B')
				{
					mineCount++ ;
				}
			}
			else
			{
				/* White checker */
				if (mine == 'W')
				{
					mineCount++ ;
				}
			}
		}
		
		/* Fourth column in half byte */
		temp1 = buffer & 0b10000000 ;
		temp1 = (temp1 >> 7) ;
		if (temp1)
		{
			/* There is a checker */
			temp2 = buffer & 0b00001000 ;
			temp2 = (temp2 >> 3) ;
			if (temp2 == 1)
			{
				/* Black checker */
				if (mine == 'B')
				{
					mineCount++ ;
				}
			}
			else
			{
				/* White checker */
				if (mine == 'W')
				{
					mineCount++ ;
				}
			}
		}
		
	}
	
	return mineCount ;
	
}