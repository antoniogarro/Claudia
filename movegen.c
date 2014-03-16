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

/*TODO: pass two arrays, one for captures, one for non-captures; merge methods.*/
int MoveGen(move *poss_moves)
{
	int nmoves = 0;
	if(board.white_to_move){
		for(unsigned char orig = 0; orig < 0x78; orig++){
			switch (board.squares[orig]){
				case W_PAWN:
					nmoves = WhitePawnMoves(orig, poss_moves, nmoves);
					break;

				case W_KNIGHT:
					nmoves = NonSlidingMoves(orig, knight_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_BISHOP:
					nmoves = SlidingMoves(orig, bishop_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_ROOK:
					nmoves = SlidingMoves(orig, rook_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_QUEEN:
					nmoves = SlidingMoves(orig, king_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_KING:
					nmoves = NonSlidingMoves(orig, king_delta, WHITE_COLOR, poss_moves, nmoves);
					if(orig == 0x04){
						nmoves = GenerateWhiteCastle(poss_moves, nmoves);
					}
					break;

				default: break;
			}
			if(COLUMN(orig) == H_COLUMN && ROW(orig) != EIGHT_ROW) orig += 8;
		}
	}else{
		for(unsigned char orig = 0; orig < 0x78; orig++){
			switch (board.squares[orig]){
				case B_PAWN:
					nmoves = BlackPawnMoves(orig, poss_moves, nmoves);
					break;

				case B_KNIGHT:
					nmoves = NonSlidingMoves(orig, knight_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_BISHOP:
					nmoves = SlidingMoves(orig, bishop_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_ROOK:
					nmoves = SlidingMoves(orig, rook_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_QUEEN:
					nmoves = SlidingMoves(orig, king_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_KING:
					nmoves = NonSlidingMoves(orig, king_delta, BLACK_COLOR, poss_moves, nmoves);
					if(orig == 0x74){
						nmoves = GenerateBlackCastle(poss_moves, nmoves);
					}
					break;

				default: break;
			}

			if(COLUMN(orig) == H_COLUMN && ROW(orig) != EIGHT_ROW) orig += 8;
		}
	}
	return nmoves;
}

/*TODO: merge White and Black pawns generators?*/
int WhitePawnMoves(unsigned char orig, move *poss_moves, int nmoves)
{
	unsigned char dest = orig + ROW_UP;
	if(IN_BOARD(dest) && board.squares[dest] == EMPTY){
		if(ROW(dest) == EIGHT_ROW){			/*Promotions.*/
			if(poss_moves) poss_moves[nmoves] = (W_QUEEN << 16) | (dest << 8) | orig;
			nmoves++;
			if(poss_moves) poss_moves[nmoves] = (W_KNIGHT << 16)|(dest << 8) | orig;
			nmoves++;
			if(poss_moves) poss_moves[nmoves] = (W_ROOK << 16) | (dest << 8) | orig;
			nmoves++;
			if(poss_moves) poss_moves[nmoves] = (W_BISHOP << 16) | (dest << 8) | orig;
			nmoves++;
		}else{
			if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
			nmoves++;
			if(ROW(dest) == THIRD_ROW){
				dest = dest + ROW_UP;
				if(IN_BOARD(dest) && board.squares[dest] == EMPTY){
					if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
					nmoves++;
				}
			}
		}
	}
	for(int i = 0; w_pawn_capture[i]; i++){
		dest = orig + w_pawn_capture[i];
		if(IN_BOARD(dest)){
			if(board.squares[dest] == EMPTY){
				if(dest == board.en_passant){		/*Captures en Passant.*/
					if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
					nmoves++;
				}
			}else{
				if(GET_COLOR(board.squares[dest]) == BLACK_COLOR){
					if(ROW(dest) == EIGHT_ROW){
						if(poss_moves) poss_moves[nmoves] = (W_QUEEN << 16) | (dest << 8) | orig;
						nmoves++;
						if(poss_moves) poss_moves[nmoves] = (W_KNIGHT << 16)|(dest << 8) | orig;
						nmoves++;
						if(poss_moves) poss_moves[nmoves] = (W_ROOK << 16) | (dest << 8) | orig;
						nmoves++;
						if(poss_moves) poss_moves[nmoves] = (W_BISHOP << 16) | (dest << 8) | orig;
						nmoves++;
					}else{
						if(poss_moves) poss_moves[nmoves] = (dest << 8)  | orig;
						nmoves++;
					}
				}
			}
		}
	}
	return nmoves;
}

int BlackPawnMoves(unsigned char orig, move *poss_moves, int nmoves)
{
	unsigned char dest = orig + ROW_DOWN;
	if(IN_BOARD(dest) && board.squares[dest] == EMPTY){
		if(ROW(dest) == FIRST_ROW){
			if(poss_moves) poss_moves[nmoves] = (B_QUEEN << 16) | (dest << 8) | orig;	
			nmoves++;
			if(poss_moves) poss_moves[nmoves] = (B_KNIGHT << 16)|(dest << 8) | orig;
			nmoves++;
			if(poss_moves) poss_moves[nmoves] = (B_ROOK << 16) | (dest << 8) | orig;
			nmoves++;
			if(poss_moves) poss_moves[nmoves] = (B_BISHOP << 16) | (dest << 8) | orig;
			nmoves++;
		}else{
			if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
			nmoves++;
			if(ROW(dest) == SIXTH_ROW){
				dest = dest + ROW_DOWN;
				if(IN_BOARD(dest)){
					if(board.squares[dest] == EMPTY){
						if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
						nmoves++;
					}
				}
			}
		}
	}
	for(int i = 0; b_pawn_capture[i]; i++){
		dest = orig + b_pawn_capture[i];
		if(IN_BOARD(dest)){
			if(board.squares[dest] == EMPTY){
				if(dest == board.en_passant){
					if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
					nmoves++;
				}
			}else{
				if(GET_COLOR(board.squares[dest])){
					if(ROW(dest) == FIRST_ROW){
						if(poss_moves) poss_moves[nmoves] = (B_QUEEN << 16) | (dest << 8) | orig;	
						nmoves++;
						if(poss_moves) poss_moves[nmoves] = (B_KNIGHT << 16)|(dest << 8) | orig;
						nmoves++;
						if(poss_moves) poss_moves[nmoves] = (B_ROOK << 16) | (dest << 8) | orig;
						nmoves++;
						if(poss_moves) poss_moves[nmoves] = (B_BISHOP << 16) | (dest << 8) | orig;
						nmoves++;
					}else{
						if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
						nmoves++;
					}
				}
			}
		}
	}
	return nmoves;
}

int SlidingMoves(unsigned char orig, const char *delta, const char piece_color, move* poss_moves, int nmoves)
{
	unsigned char dest;
	for(int i = 0; delta[i]; i++){
		dest = orig + delta[i];
		while(1){
			if(IN_BOARD(dest)){
				if(board.squares[dest] == EMPTY){
					if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
					nmoves++;
					dest += delta[i];
				}else{
					/*Different color Piece, capture, stop sliding:*/
					if(GET_COLOR(board.squares[dest]) != piece_color){
						if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
						nmoves++;
						break;
					}else break;		/*same color piece, stop sliding.*/
				}
			}else break;	/*Out of Board, stop sliding.*/
		}
	}
	return nmoves;
}

int NonSlidingMoves(unsigned char orig, const char *delta, const char piece_color, move *poss_moves, int nmoves)
{
	unsigned char dest;
	for(int i = 0; delta[i]; i++){
		dest = orig + delta[i];
		if(IN_BOARD(dest)){
			if(board.squares[dest] == EMPTY){
				if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
				nmoves++;
			}else{
				if(GET_COLOR(board.squares[dest]) != piece_color){
					if(poss_moves) poss_moves[nmoves] = (dest << 8) | orig;
					nmoves++;
				}
			}
		}
	}
	return nmoves;
}

int GenerateWhiteCastle(move* poss_moves, int nmoves)
{
	unsigned char dest;
	if(board.wk_castle && board.squares[0x07] == W_ROOK && board.squares[0x06] == EMPTY
			&& board.squares[0x05] == EMPTY && !WhiteInCheck()
			&& !IsAttacked(0x05, BLACK_COLOR)){
		dest = 0x06;
		if(poss_moves) poss_moves[nmoves] = (dest << 8) | 0x04;
		nmoves++;
	}
	if(board.wq_castle && board.squares[0x00] == W_ROOK &&board.squares[0x01] == EMPTY
			&& board.squares[0x02] == EMPTY && board.squares[0x03] == EMPTY
			&& !WhiteInCheck() && !IsAttacked(0x03, BLACK_COLOR)){
		dest = 0x02;
		if(poss_moves) poss_moves[nmoves] = (dest << 8) | 0x04;
		nmoves++;
	}
	return nmoves;
}

int GenerateBlackCastle(move* poss_moves, int nmoves)
{
	unsigned char dest;
	if(board.bk_castle && board.squares[0x77] == B_ROOK &&board.squares[0x76] == EMPTY
			&& board.squares[0x75] == EMPTY && !BlackInCheck() &&
			!IsAttacked(0x75, WHITE_COLOR)){
		dest = 0x76;
		if(poss_moves) poss_moves[nmoves] = (dest << 8) | 0x74;
		nmoves++;
	}
	if(board.bq_castle && board.squares[0x70] == B_ROOK &&board.squares[0x71] == EMPTY
			&& board.squares[0x72] == EMPTY && board.squares[0x73] == EMPTY
			&& !BlackInCheck() && !IsAttacked(0x73, WHITE_COLOR)){
		dest = 0x72;
		if(poss_moves) poss_moves[nmoves] = (dest << 8) | 0x74;
		nmoves++;
	}
	return nmoves;
}
