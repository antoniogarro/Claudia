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

int SEE(move* main_capture)
{
	char dest = (*main_capture & 0xFFFF) >> 8;
	char captured, sq;
	char attacking_sqs[10];
	char *less_attack_sq;
	int attackers, depth = 0, val = 0;
	move move_hist[20];
	
	MakeMove(main_capture);
	/*Note that color is 0x8(1000) or 0000; ugly.*/
	attackers = AttackingPieces(dest, (board.white_to_move << 3), attacking_sqs);

	while(attackers){
		less_attack_sq = attacking_sqs;
		for(int i = 0; i < attackers; i++){
			sq = attacking_sqs[i];
			if(board.squares[*less_attack_sq] > board.squares[sq]) {
				less_attack_sq = &attacking_sqs[i];
			}
		}

		move_hist[depth] = (dest << 8) | *less_attack_sq;
		if((board.squares[*less_attack_sq] == B_PAWN && ROW(dest) == FIRST_ROW)
					|| (board.squares[*less_attack_sq] == W_PAWN && ROW(dest) == EIGHT_ROW)){
			move_hist[depth] |= (B_QUEEN << 16);
		}
		MakeMove(&move_hist[depth]);
		depth++;
		attackers = AttackingPieces(dest, (board.white_to_move << 3), attacking_sqs);
	}

	while(depth--){
		captured = (move_hist[depth] & 0xFFFFFF) >> 20;
		if(val + Value(captured) > 0) val = -(val + Value(captured));
		else val = 0;
		Takeback(move_hist[depth]);
	}
	captured = (*main_capture & 0xFFFFFF) >> 20;
	val = val + Value(captured);
	Takeback(*main_capture);

return val;
}

void SortMoves(move *moves, const int nmoves)
{
	int eval[200];
	move temp;
	int curr_eval;
	move hash_move = GetHashMove(board.zobrist_key);
	/*Evaluate every move, store evaluations:*/
	for(int i = 0; i < nmoves; i++) eval[i] = EvaluateMove(&moves[i], hash_move);
	
	/*Order according to that evaluation:*/
	for(int i = 0; i < nmoves; i++){
		curr_eval = eval[i];
		for(int j = i; j < nmoves; j++){
			if(eval[j] > curr_eval){
				temp = moves[i];
				moves[i] = moves[j];
				moves[j] = temp;
				curr_eval = eval[j]; /*Cagüento!*/
				eval[j] = eval[i];

			}
		}
	}
	/*TODO: qsort();*/
}

int EvaluateMove(move *curr_move, const move hash_move)
{
	/*Compare with hash_move, using only orig, des; curr_move may not have captured or ep info.*/
	if((*curr_move & 0xFFFF) == (hash_move & 0xFFFF)){
		return HASHMOVE_VALUE;		/*search HashMove first.*/
	}
	/*Evaluate captures with SEE:*/
	char dest = (*curr_move & 0xFFFF) >> 8;
	if(board.squares[dest] != EMPTY) return SEE(curr_move);
	else return 0;		/*TODO: evaluate non-captures.*/
}

int FilterWinning(move *captures, int ncapts)
{
	int good_capts = ncapts;
	int eval[200];
	move temp;
	int curr_eval;
	
	/*evaluate every move, store evaluations:*/
	for(int i = 0; i < ncapts; i++){
		eval[i] = SEE(&captures[i]);
	}
	/*Store the number of 'good' captures:*/
	for(int i = 0; i < ncapts; i++){
		if(eval[i] <= 0){
			/*captures[i] = 0; Shouldn't be here.*/
			good_capts--;
		}
	}
	for(int i = 0; i < ncapts; i++){
		curr_eval = eval[i];
		for(int j = i; j < ncapts; j++){
			if(eval[j] > curr_eval){
				temp = captures[i];
				captures[i] = captures[j];
				captures[j] = temp;
				curr_eval = eval[j];
				eval[j] = eval[i];
			}
		}
	}
	return good_capts;
}

int Value(const char piece)
{

		switch(piece){

			case W_PAWN: return PAWN_VALUE;
			case B_PAWN: return PAWN_VALUE;

			case W_KNIGHT: return KNIGHT_VALUE;
			case W_BISHOP: return BISHOP_VALUE;
			case W_ROOK: return ROOK_VALUE;

			case B_KNIGHT: return KNIGHT_VALUE;
			case B_BISHOP: return BISHOP_VALUE;
			case B_ROOK: return ROOK_VALUE;
			
			case W_QUEEN: return QUEEN_VALUE;
			case B_QUEEN: return QUEEN_VALUE;
			case W_KING: return KING_VALUE;
			case B_KING: return KING_VALUE;

			default: return 0;
		}
}
