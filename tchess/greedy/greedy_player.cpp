/*
 * greedy_player.cpp
 *
 *  Created on: 2021. febr. 1.
 *      Author: Gáspár Tamás
 */
#include <algorithm>
#include <limits>

#include "greedy_player.h"
#include "board/evaluation.h"

namespace tchess
{
	std::vector<move> createEnemyMoves(unsigned int enemySide, const chessboard& board, const game_information& info) {
		std::vector<move> enemyMoves;
		move_generator generator(board, info);
		generator.generatePseudoLegalMoves(enemySide, enemyMoves);
		return enemyMoves;
	}

	move greedy_player::makeMove(const game& gameController) {
		const std::vector<move>& gameMoves = gameController.getMoves();
		if(gameMoves.size() > 0) {
			move enemyMove = gameMoves.back(); //update our board with enemy move
			board.makeMove(enemyMove, 1-side);
			updateGameInformation(board, enemyMove, info); //update game information
		}
		std::vector<move> moves;
		move_generator generator(board, info);
		generator.generatePseudoLegalMoves(side, moves); //generate all pseudo legal moves
		auto legalCheck = [&](const move& m) { return !(isLegalMove(m, board, info)); }; //used later
		auto legalEnd = std::remove_if(moves.begin(), moves.end(), legalCheck);

		std::cout << "Greedy board before move:\n" << board.to_string() << std::endl;
		move bestMove; //stores current best move
		int bestEvaluation = WORST_VALUE;
		//iterate all legal moves to greedily find best move
		for(auto it = moves.begin(); it != legalEnd; ++it) {
			move _move = *it;
			int capturedPiece = board.makeMove(_move, side);
			game_information infoAfterMove = info; //copy game info to not modify the original
			updateGameInformation(board, _move, infoAfterMove);
			//the move is legal, and it is made on the board, now evaluate
			std::vector<move> enemyMoves = createEnemyMoves(1-side, board, infoAfterMove);
			bool legalEnemyMoves = false;
			for(const move& enemyMove: enemyMoves) {
				if(isLegalMove(enemyMove, board, infoAfterMove)) {
					legalEnemyMoves = true;
					break;
				}
			}
			special_board sb = isSpecialBoard(1-side, board, legalEnemyMoves, 1);
			int evaluation = 0;
			if(sb.special) { //no need for static evaluation
				 evaluation = sb.evaluation;
			} else { //static evaluation
				evaluation = evaluateBoard(side, board, infoAfterMove, moves.size());
			}
			board.unmakeMove(_move, side, capturedPiece); //unmake move after evaluation
			//std::string moveString = _move.to_string(std::abs(board[_move.getFromSquare()]));
			//std::cout << "Evaluation " << evaluation << " for move " << moveString << std::endl;
			//The eval function will return higher score for better positions.
			if(evaluation >= bestEvaluation) { //new best move
				bestEvaluation = evaluation;
				bestMove = _move;
			}
		}
		board.makeMove(bestMove, side); //keep board updated
		updateGameInformation(board, bestMove, info);
		std::cout << "Greedy board after move:\n" << board.to_string() << std::endl;
		return bestMove; //submit best greedy move
}

	std::string greedy_player::description() const {
		std::string sideName = side == white ? "White" : "Black";
		return "Greedy move maker (" + sideName + ")";
	}
}



