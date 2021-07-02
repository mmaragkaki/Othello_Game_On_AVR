/*
 * NewTest.c
 *
 * Created: 24/4/2019 1:15:33 Ο€ΞΌ
 * Author : Panagiotis
 */ 
#include <stdio.h>
#include <avr/io.h>
#define SIZE 16
void tableInit(void);
void setBlack(char c,uint8_t column);
void setChecker(char c,uint8_t column,char player);
char readCell(char c,uint8_t column);
uint8_t scanChessboard(char mine) ;
void checkAvailable(char c,uint8_t j,char player,char mine) ;
void insertMineValid(char c,uint8_t column) ;
void insertEnemyValid(char c,uint8_t column) ;
void firstValid(char mine) ;
void TurnOtherCheckers(char c,uint8_t column,char enemy,char player) ;
uint8_t checkEnemyMove(char c, uint8_t column) ;
void clearValidMoves(void) ;

uint8_t table[SIZE] ;
uint8_t mineValidtable[SIZE/2] ;
uint8_t enemyValidtable[SIZE/2] ;

char mine = ' ' ;
char enemy = ' ' ;

char myLine = 'A' ;
uint8_t myColumn = 1 ;

int main(void)
{
	tableInit() ;
	mine = 'B' ;
	enemy = 'W' ;
	
	scanChessboard(mine) ;
	
	
	setChecker('D',3,mine) ;
	TurnOtherCheckers('D',3,enemy,mine) ;
	clearValidMoves() ;
	scanChessboard(mine) ;
	
	if (checkEnemyMove('C',5) == 1)
	{
		setChecker('C',5,enemy) ;
		TurnOtherCheckers('C',5,mine,enemy) ;
		clearValidMoves() ;
		scanChessboard(mine) ;
	}
	
	firstValid(mine) ;
	
	
	
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
	//uint8_t buffer = 0 ;
	//uint8_t temp = 0 ;
	//uint8_t column = 0 ;
	//uint8_t found = 0 ;
	//uint8_t index = 0 ;
	//while ((index < (SIZE / 2)) && (found == 0))
	//{
		//char c = index + 65 ;
		//buffer = mineValidtable[index] ;
		//temp = buffer & 0b00000001 ;
		//if (temp == 1 && found == 0)
		//{
			//column = 1;
			//found = 1 ;
			//setChecker(c,column,'B') ;	
		//}
		//temp = buffer & 0b00000010 ;
		//if (temp == 2 && found == 0)
		//{
			//column = 2 ;
			//found = 1 ;
			//setChecker(c,column,'B') ;
		//}
		//temp = buffer & 0b00000100 ;
		//if (temp == 4 && found == 0)
		//{
			//column = 3;
			//found = 1 ;
			//setChecker(c,column,'B') ;
		//}
		//temp = buffer & 0b00010000 ;
		//if (temp == 16 && found == 0)
		//{
			//column = 4 ;
			//found = 1 ;
			//setChecker(c,column,'B') ;
		//}
		//temp = buffer & 0b00100000 ;
		//if (temp == 32 && found == 0)
		//{
			//column = 5 ;
			//found = 1 ;
			//setChecker(c,column,'B') ;
		//}
		//temp = buffer & 0b01000000 ;
		//if (temp == 64 && found == 0)
		//{
			//column = 6 ;
			//found = 1 ;
			//setChecker(c,column,'B') ;
		//}
		//temp = buffer & 0b10000000 ;
		//if (temp == 128 && found == 0)
		//{
			//column = 7 ;
			//found = 1 ;
			//setChecker(c,column,'B') ;
		//}
		//
		//index++ ;
	//}
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


