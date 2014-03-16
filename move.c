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
#include <stdio.h>
#include "board.h"
/*
'MakeMove()' modifies curr_move to update information depending on the position (captured, catle, EP...).
'MakeMove()' does NOT verify legality!
*/
void MakeMove(move *curr_move)
{
	unsigned char orig = ORIGMASK(*curr_move);
	unsigned char dest = DESTMASK(*curr_move);
	unsigned char promoted = PROMMASK(*curr_move);
	/*Piecepromoted stores only piece type, NOT correct color, because it comes
	from CharToPiece, and algebraic notation does not ensure Q != q.*/

	*curr_move |= (board.en_passant << 24);		/*stores PAWN_EP in order to undo.*/
	
	/*don't update 'castle rights' or 'en passant' board.zobrist_key, too expensive?.*/
	/*if(IN_BOARD(board.en_passant)) board.zobrist_key ^= zobkeys.zob_enpass[board.en_passant];*/

	board.ply++;
	/*captured piece: bits 20-23; 0 if EMPTY, o en passant capture:*/
	if(board.squares[dest]){
		*curr_move |= (board.squares[dest] << 20);
		board.rev_plies[board.ply] = 0;
	}
	else{
		board.rev_plies[board.ply] = board.rev_plies[board.ply-1]+1;	
	}
	switch (board.squares[orig]){
		case W_PAWN:
			if(ROW(dest) == EIGHT_ROW){			/*Promotion*/
				DropPiece(dest, TURN_WHITE(promoted));
				board.en_passant = 0xF;		/*Invalid square: no pawn en passant.*/
			}else{
				DropPiece(dest, W_PAWN);
				if(ROW(dest) == FOURTH_ROW && ROW(orig) == SECOND_ROW){
					board.en_passant = dest + ROW_DOWN;
				}else{
					/*Remove en passant captured pawn (we won't later).*/
					if(dest == board.en_passant) RemovePiece(dest + ROW_DOWN);
					board.en_passant = 0xF;
				}
			}
			board.rev_plies[board.ply] = 0;
			break;

		case B_PAWN:
			if(ROW(dest) == FIRST_ROW){
				DropPiece(dest, TURN_BLACK(promoted));
				board.en_passant = 0xF;
			}
			else{
				DropPiece(dest, B_PAWN);
				if(ROW(dest) == FIFTH_ROW && ROW(orig) == SEVENTH_ROW){
					board.en_passant = dest + ROW_UP;
				}else{
					if(dest == board.en_passant) RemovePiece(dest + ROW_UP);
					board.en_passant = 0xF;
				}
			}
			board.rev_plies[board.ply] = 0;
			break;

		case W_KING:
			DropPiece(dest, W_KING);
			board.wking_pos = dest;
			if(orig == e1){
				if(dest == g1){		/*Short castle*/
					RemovePiece(h1);
					DropPiece(f1, W_ROOK);
					board.w_castled = 1;
				}
				if(dest == c1){		/*Long castle*/
					RemovePiece(a1);
					DropPiece(d1, W_ROOK);
					board.w_castled = 1;
				}

				if(board.wq_castle) *curr_move |= (Q_CASTLE_RIGHT << 16);
				if(board.wk_castle) *curr_move |= (K_CASTLE_RIGHT << 16);
				/*Stores lost castle rights: 'bits promoted' set to 8 if long castle is lost, 5 if short, 13 if both.*/
			}
			board.wk_castle = 0, board.wq_castle = 0;
			board.en_passant = 0xF;
			break;

		case B_KING:
			DropPiece(dest, B_KING);
			board.bking_pos = dest;
			if(orig == e8){
				if(dest == g8){
					RemovePiece(h8);
					DropPiece(f8, B_ROOK);
					board.b_castled = 1;
				}
				if(dest == c8){
					RemovePiece(a8);
					DropPiece(d8, B_ROOK);
					board.b_castled = 1;
				}

				if(board.bq_castle) *curr_move |= (Q_CASTLE_RIGHT << 16);
				if(board.bk_castle) *curr_move |= (K_CASTLE_RIGHT << 16);
			}
			board.bk_castle = 0, board.bq_castle = 0;
			board.en_passant = 0xF;
			break;

		case W_ROOK:
			if(board.wq_castle && orig == a1) {
				*curr_move |= (Q_CASTLE_RIGHT << 16);
				board.wq_castle = 0;
			}
			if(board.wk_castle && orig == h1) {
				*curr_move |= (K_CASTLE_RIGHT << 16);	
				board.wk_castle = 0;
			}
			DropPiece(dest, W_ROOK);
			board.en_passant = 0xF;
			break;

		case B_ROOK:
			if(board.bq_castle && orig == a8) {
				*curr_move |= (Q_CASTLE_RIGHT << 16);
				board.bq_castle = 0;
			}
			if(board.bk_castle && orig == h8) {
				*curr_move |= (K_CASTLE_RIGHT << 16);
				board.bk_castle = 0;
			}
			DropPiece(dest, B_ROOK);
			board.en_passant = 0xF;
			break;

		default:
			DropPiece(dest, board.squares[orig]);
			board.en_passant = 0xF;
			break;
	}
	
	RemovePiece(orig);
	/*if(IN_BOARD(board.en_passant)) board.zobrist_key ^= zobkeys.zob_enpass[board.en_passant];*/
	board.white_to_move = !(board.white_to_move);
	board.zobrist_key ^= zobkeys.zob_side;
	board.zobrist_history[board.ply] = board.zobrist_key;

}

void Takeback(const move prev_move)
{
	unsigned char orig = ORIGMASK(prev_move);
	unsigned char dest = DESTMASK(prev_move);
	/*Piecepromoted stores the piece type, not correct color.*/
	unsigned char promoted = PROMMASK(prev_move);
	unsigned char captured = CAPTMASK(prev_move);
	
	/*if(IN_BOARD(board.en_passant)) board.zobrist_key ^= zobkeys.zob_enpass[board.en_passant];*/
	board.en_passant = EPMASK(prev_move);
	/*if(IN_BOARD(board.en_passant)) board.zobrist_key ^= zobkeys.zob_enpass[board.en_passant];*/
	board.white_to_move = !(board.white_to_move);
	board.zobrist_key ^= zobkeys.zob_side;

	switch(board.squares[dest]){
			case W_PAWN:
				if(dest == board.en_passant)  DropPiece(dest + ROW_DOWN, B_PAWN);
				DropPiece(orig, W_PAWN);
				break;
			case B_PAWN:
				if(dest == board.en_passant) DropPiece(dest + ROW_UP, W_PAWN);
				DropPiece(orig, B_PAWN);
				break;
			case W_KING:
				DropPiece(orig, W_KING);
				board.wking_pos = orig;
				if(orig == e1){
					if(dest == g1){				/*short castle.*/
						RemovePiece(f1);
						DropPiece(h1, W_ROOK);
						board.wk_castle = 1;
						board.w_castled = 0;
					}else{
						if(dest == c1){			/*Long castle.*/
							RemovePiece(d1);
							DropPiece(a1, W_ROOK);
							board.wq_castle = 1;
							board.w_castled = 0;
						}
					}
					if(promoted == Q_CASTLE_RIGHT || promoted == BOTH_CASTLES){
						board.wq_castle = 1;
					}
					if(promoted == K_CASTLE_RIGHT || promoted == BOTH_CASTLES){
						board.wk_castle = 1;	
					}
				}
				break;

			case B_KING:
				DropPiece(orig, B_KING);
				board.bking_pos = orig;
				if(orig == e8){
					if(dest == g8){
						RemovePiece(f8);
						DropPiece(h8, B_ROOK);
						board.bk_castle = 1;
						board.b_castled = 0;
					}else{
						if(dest == c8){
							RemovePiece(d8);
							DropPiece(a8, B_ROOK);
							board.bq_castle = 1;
							board.b_castled = 0;
						}
					}
					if(promoted == Q_CASTLE_RIGHT || promoted == BOTH_CASTLES){
						board.bq_castle = 1;
					}
					if(promoted == K_CASTLE_RIGHT || promoted == BOTH_CASTLES){
						board.bk_castle = 1;
					}
				}
				break;

			case W_ROOK:
				if(promoted){
					if(promoted == Q_CASTLE_RIGHT){
						board.wq_castle = 1;
						DropPiece(orig, W_ROOK);
					}else{
						if(promoted == K_CASTLE_RIGHT){
							board.wk_castle = 1;
							DropPiece(orig, W_ROOK);
						}else{
							DropPiece(orig, W_PAWN);
						}
					}
				}else{
					DropPiece(orig, W_ROOK);
				}
				break;

			case B_ROOK:
				if(promoted){
					if(promoted == Q_CASTLE_RIGHT){
						board.bq_castle = 1;
						DropPiece(orig, B_ROOK);
					}else{
						if(promoted == K_CASTLE_RIGHT){
							board.bk_castle = 1;
							DropPiece(orig, B_ROOK);
						}else{				
							DropPiece(orig, B_PAWN);
						}
					}
				}else{
					DropPiece(orig, B_ROOK);
				}
				break;
			/*case EMPTY:
				break; */
			default:
				if(promoted){
					/*drop a PAWN, same color as the piece's on dest.*/
					DropPiece(orig, (B_PAWN | GET_COLOR(board.squares[dest])));
				}else{
					DropPiece(orig, board.squares[dest]);
				}
				break;
	}
	DropPiece(dest,captured);
	board.ply--;
}

void RemovePiece(unsigned char sq)
{
	board.zobrist_key ^= zobkeys.zob_pieces[board.squares[sq]][sq];
	board.squares[sq] = EMPTY;
}

void DropPiece(unsigned char sq, unsigned char piece)
{
	board.zobrist_key ^= zobkeys.zob_pieces[board.squares[sq]][sq];
	board.zobrist_key ^= zobkeys.zob_pieces[piece][sq];
	board.squares[sq] = piece;
}