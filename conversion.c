/***************************************************************************************
* Copyright (c) 2014, Antonio Garro.                                                   *
* All rights reserved                                                                  *
*                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are *
* permitted provided that the following conditions are met:                            *
*                                                                                      *
* 1. Redistributions of source code must retain the above copyright notice, this list  *
* of conditions and the following disclaimer.                                          *
*                                                                                      *
* 2. Redistributions in binary form must reproduce the above copyright notice, this    *
* list of conditions and the following disclaimer in the documentation and/or other    *
* materials provided with the distribution.                                            *
*                                                                                      *
* 3. Neither the name of the copyright holder nor the names of its contributors may be *
* used to endorse or promote products derived from this software without specific      *
* prior written permission.                                                            *
*                                                                                      * 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"          *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE            *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE           *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE            *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL    *
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR           *
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER           *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF        *
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
***************************************************************************************/

#include "claudia.h"
#include "board.h"

char CharToPiece (const char fen)
{
	char piece = 0;
	switch (fen) {
		case 'p': piece = B_PAWN;
			break;
		case 'P': piece = W_PAWN;
			break;
		case 'n': piece = B_KNIGHT;
			break;
		case 'N': piece = W_KNIGHT;
			break;
		case 'b': piece = B_BISHOP;
			break;
		case 'B': piece = W_BISHOP;
			break;
		case 'r': piece = B_ROOK;
			break;
		case 'R': piece = W_ROOK;
			break;
		case 'q': piece = B_QUEEN;
			break;
		case 'Q': piece = W_QUEEN;
			break;
		case 'k': piece = B_KING;
			break;
		case 'K': piece = W_KING;
			break;
		default: piece = EMPTY;
			break;
	}
return piece;
}

char PieceToChar (const char piece)
{
	char char_piece = 0;
	switch (piece){
			case W_PAWN: char_piece = 'P';
				break;
			case B_PAWN: char_piece = 'p';
				break;
			case W_KNIGHT: char_piece = 'N';
				break;
			case B_KNIGHT: char_piece = 'n';
				break;
			case W_BISHOP: char_piece = 'B';
				break;
			case B_BISHOP: char_piece = 'b';
				break;
			case W_ROOK: char_piece = 'R';
				break;
			case B_ROOK: char_piece = 'r';
				break;
			case W_QUEEN: char_piece = 'Q';
				break;
			case B_QUEEN: char_piece = 'q';
				break;
			case W_KING: char_piece = 'K';
				break;
			case B_KING: char_piece = 'k';
				break;
			case EMPTY: char_piece = ' ';
				break;
			default: char_piece = 'C';
				break;
	}
	return char_piece;
}

char CharToCoordinate (const char fen)
{
	char coord = 0x77;
	switch (fen){
		case '1': coord = 0x00;
			break;
		case '2': coord = 0x10;
			break;
		case '3': coord = 0x20;
			break;
		case '4': coord = 0x30;
			break;
		case '5': coord = 0x40;
			break;
		case '6': coord = 0x50;
			break;
		case '7': coord = 0x60;
			break;
		case '8': coord = 0x70;
			break;
		case 'a': coord = 0x00;
			break;
		case 'b': coord = 0x01;
			break;
		case 'c': coord = 0x02;
			break;
		case 'd': coord = 0x03;
			break;
		case 'e': coord = 0x04;
			break;
		case 'f': coord = 0x05;
			break;
		case 'g': coord = 0x06;
			break;
		case 'h': coord = 0x07;
			break;
		default: coord =  0x0F;
			break;
	}
return coord;
}

char RowCoordinateToChar (const char coord)
{
	char row;
	switch (coord){
		case 0x00: row = '1';
			break;
		case 0x01: row = '2';
			break;
		case 0x02: row = '3';
			break;
		case 0x03: row = '4';
			break;
		case 0x04: row = '5';
			break;
		case 0x05: row = '6';
			break;
		case 0x06: row = '7';
			break;
		case 0x07: row = '8';
			break;
		default:   row = 0;
			break;
	}
return row;
}

char ColumnCoordinateToChar (const char coord)
{
	char column;
	switch (coord){
		case 0x00: column = 'a';
			break;
		case 0x01: column = 'b';
			break;
		case 0x02: column = 'c';
			break;
		case 0x03: column = 'd';
			break;
		case 0x04: column = 'e';
			break;
		case 0x05: column = 'f';
			break;
		case 0x06: column = 'g';
			break;
		case 0x07: column = 'h';
			break;
		default:   column = 0;
			break;
	}
return column;
}

move AlgebToMove(const char *str_mov)
{
	move curr_move = 0;
	char square = 0;
	
	/*Pieced promoted to:*/				
	square = CharToPiece(str_mov[4]);
	curr_move += square;
	curr_move = curr_move << 8;

	/*DEST:*/
	square = CharToCoordinate(str_mov[3]) + CharToCoordinate(str_mov[2]);
	curr_move += square;
	curr_move = curr_move << 8;

	/*ORIG:*/
	square = CharToCoordinate(str_mov[1]) + CharToCoordinate(str_mov[0]);
	curr_move += square;

return curr_move;
}

void MoveToAlgeb(const move curr_move, char *str_mov)
{
	/*TODO?: check str_mov to have at least 6; else, return 0.*/
	move aux_move = (curr_move & 0xFFFFF) >> 16;
	str_mov [4] = PieceToChar(aux_move);
	if(str_mov[4] == 'P' || str_mov[4] == 'p' || str_mov[4] == 'C'){
		str_mov[4] = ' ';
		str_mov[5] = 0;
	}
	else{
		str_mov[5] = ' ';
		str_mov[6] = 0;
	}

	/*DEST ROW:*/
	aux_move = (curr_move & 0xFFFF) >> 12;
	str_mov [3] = RowCoordinateToChar(aux_move);
	
	/*DEST COLUMN:*/
	aux_move = (curr_move & 0xFFF) >> 8;
	str_mov [2] = ColumnCoordinateToChar(aux_move);
	
	/*ORIG ROW:*/
	aux_move = (curr_move & 0xFF) >> 4;
	str_mov [1] = RowCoordinateToChar(aux_move);
	
	/*ORIG COLUMN:*/
	aux_move = (curr_move & 0xF);
	str_mov [0] = ColumnCoordinateToChar(aux_move);
}
