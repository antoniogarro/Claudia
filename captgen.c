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


int CaptureGen(move *poss_moves)
{
	int nmoves = 0;
	if(board.white_to_move){
		for(char orig = 0; orig < 0x78; orig++){
			switch (board.squares[orig]){
				case W_PAWN:
					nmoves = WhitePawnCaptures(orig, poss_moves, nmoves);
					break;

				case W_KNIGHT:
					nmoves = NonSlidingCaptures(orig, knight_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_BISHOP:
					nmoves = SlidingCaptures(orig, bishop_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_ROOK:
					nmoves = SlidingCaptures(orig, rook_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_QUEEN:
					nmoves = SlidingCaptures(orig, king_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				case W_KING:
					nmoves = NonSlidingCaptures(orig, king_delta, WHITE_COLOR, poss_moves, nmoves);
					break;

				default: break;
			}
			/*Skip dummy board: worth?*/
			if(COLUMN(orig) == H_COLUMN && ROW(orig) != EIGHT_ROW) orig += 8;
		}
	}
	else{
		for(char orig = 0; orig < 0x78; orig++){
			switch (board.squares[orig]){
				case B_PAWN:
					nmoves = BlackPawnCaptures(orig, poss_moves, nmoves);
					break;

				case B_KNIGHT:
					nmoves = NonSlidingCaptures(orig, knight_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_BISHOP:
					nmoves = SlidingCaptures(orig, bishop_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_ROOK:
					nmoves = SlidingCaptures(orig, rook_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_QUEEN:
					nmoves = SlidingCaptures(orig, king_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				case B_KING:
					nmoves = NonSlidingCaptures(orig, king_delta, BLACK_COLOR, poss_moves, nmoves);
					break;

				default: break;
			}
			if(COLUMN(orig) == H_COLUMN && ROW(orig) != EIGHT_ROW) orig += 8;
		}
	}

	return nmoves;
}


int WhitePawnCaptures(const char orig, move *captures, int ncapts)
{
	char dest = orig + ROW_UP;

	for(int i = 0; w_pawn_capture[i]; i++){
		dest = orig + w_pawn_capture[i];
		if(IN_BOARD(dest)){
			if(board.squares[dest] == EMPTY){
				if(dest == board.en_passant){ /*Captures en Passant.*/
					captures[ncapts] = (dest << 8) | orig;
					ncapts++;
				}
			}else{
				if(GET_COLOR(board.squares[dest]) == BLACK_COLOR){
					if(ROW(dest) == EIGHT_ROW){
						captures[ncapts] = (W_QUEEN << 16) | (dest << 8) | orig;	
						ncapts++;
						captures[ncapts] = (W_KNIGHT << 16)|(dest << 8) | orig;
						ncapts++;
						captures[ncapts] = (W_ROOK << 16) | (dest << 8) | orig;
						ncapts++;
						captures[ncapts] = (W_BISHOP << 16) | (dest << 8) | orig;
						ncapts++;
					}else{
						captures[ncapts] = (dest << 8)  | orig;
						ncapts++;
					}
				}
			}
		}
	}
	return ncapts;
}

int BlackPawnCaptures(const char orig, move *captures, int ncapts)
{
	char dest = orig + ROW_DOWN;
	
	for(int i = 0; b_pawn_capture[i]; i++){
		dest = orig + b_pawn_capture[i];
		if(IN_BOARD(dest)){
			if(board.squares[dest] == EMPTY){
				if(dest == board.en_passant){
					captures[ncapts] = (dest << 8) | orig;
					ncapts++;
				}
			}else{
				if(GET_COLOR(board.squares[dest])){
					if(ROW(dest) == FIRST_ROW){
						captures[ncapts] = (B_QUEEN << 16) | (dest << 8) | orig;	
						ncapts++;
						captures[ncapts] = (B_KNIGHT << 16)|(dest << 8) | orig;
						ncapts++;
						captures[ncapts] = (B_ROOK << 16) | (dest << 8) | orig;
						ncapts++;
						captures[ncapts] = (B_BISHOP << 16) | (dest << 8) | orig;
						ncapts++;
					}else{
						captures[ncapts] = (dest << 8) | orig;
						ncapts++;
					}
				}
			}
		}
	}

	return ncapts;
}

int SlidingCaptures(const char orig, const char *delta, const char piece_color, move* captures, int ncapts)
{
	char dest;
	for(int i = 0; delta[i]; i++){
		dest = orig + delta[i];
		while(1){
			if(IN_BOARD(dest)){
				if(board.squares[dest] == EMPTY) dest += delta[i];
				else{
					/*Different color Piece, capture, stop sliding:*/
					if(GET_COLOR(board.squares[dest]) != piece_color){
						captures[ncapts] = (dest << 8) | orig;
						ncapts++;
						break;
					}else break; /*same color piece, stop sliding.*/
				}
			}else break; /*Out of Board, stop sliding.*/
		}
	}
	return ncapts;
}

int NonSlidingCaptures(const char orig, const char *delta, const char piece_color,	move *captures, int ncapts)
{
	char dest;
	for(int i = 0; delta[i]; i++){
		dest = orig + delta[i];
		if(IN_BOARD(dest)){
			if(board.squares[dest] != EMPTY &&
					GET_COLOR(board.squares[dest]) != piece_color){
				captures[ncapts] = (dest << 8) | orig;
				ncapts++;
			}
		}
	}
	return ncapts;
}

int CheckEscGen(move *pPossibleEsc, const char *checking_sqs, const int nchecking)
{
	int ncheck_esc = 0;
	//Si el jaque es simple:
		//Generamos las capturas posibles para la pieza que da jaque, almacenadas en cSavingSquares
		//int iSavers = AttackingPieces(checking_sqs[0], OwnColor, cSavingSquares);

		//Generamos las interposiciones posibles
		//Buscamos todas las casillas intemedias: conviene una tabla con las deltas, aunque se pueden calcular.
		//int iInterp = AttackingPieces(TODAS LAS INTERMEDIAS, OwnColor, cInterpSquares);
	//en cualquier caso:
		//Generamos los movimientos del rey
		//NonSlidingMoves(king_delta,...);

	//Copiamos los escapes en pPossibleEsc.
	//Esto genera pseudo-legal moves, ¡pero el rey aun puede quedar en jaque!
return ncheck_esc;
}

int ChecksGen()
{
	return 0;
}