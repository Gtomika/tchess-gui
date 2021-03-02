/*
 * engine.cpp
 *
 *  Created on: 2021. febr. 3.
 *      Author: Gáspár Tamás
 */

#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>

#include "engine.h"
#include "board/evaluation.h"

namespace tchess
{
	unsigned int engine_depth = 6;

	move engine::alphaBetaNegamaxRoot() {
		unsigned int side = info.getSideToMove();
		//create legal moves for this board and side
		std::vector<move> moves;
		move_generator generator(board, info);
		generator.generatePseudoLegalMoves(side, moves);
		auto legalCheck = [&](const move& m) { return !(isLegalMove(m, board, info)); };
		auto legalEnd = std::remove_if(moves.begin(), moves.end(), legalCheck);

		//got moves, post message to set progress bar range
		short* bottom = new short(0);
		short* top = new short(moves.size());
		PostMessage(view->GetSafeHwnd(), MOVE_GENERATION_RANGE, reinterpret_cast<WPARAM>(bottom), reinterpret_cast<LPARAM>(top));

		//order moves
		std::sort(moves.begin(), moves.end(), std::greater<move>());
		//we cant be at maximum depth, since this is the root call
		move bestMove;
		int bestEvaluation = WORST_VALUE;
		int count = 0;
		for(auto it = moves.begin(); it != legalEnd; it++) { //iterate legal moves
			move& _move = *it;
			int p = std::abs(board[_move.getFromSquare()]);
			int capturedPiece = board.makeMove(_move, side);
			game_information infoAfterMove = info; //create a game info object
			updateGameInformation(board, _move, infoAfterMove); //update new info object with move
			int evaluation = -alphaBetaNegamax(WORST_VALUE, BEST_VALUE, depth-1, infoAfterMove); //move down in the tree
			board.unmakeMove(_move, side, capturedPiece); //unmake the move before moving on
			if(evaluation >= bestEvaluation) {
				bestEvaluation = evaluation;
				bestMove = _move;
			}
			//finished with this move, post progress message
			++count;
			int* pCount = new int(count);
			PostMessage(view->GetSafeHwnd(), MOVE_GENERATION_PROGRESS, 0, reinterpret_cast<LPARAM>(pCount));
		}
		std::cout << std::endl;
		ttable->invalidateEntries();
		return bestMove;
	}

	//Helper struct that stores if a move was checked to be legal, and what is the result of the check.
	struct legality_checked {
		bool checked;
		bool legal;
		legality_checked() : checked(false), legal(false) {}
	};

	int engine::alphaBetaNegamax(int alpha, int beta, unsigned int depthLeft, game_information& gameInfo) {
		unsigned int side = gameInfo.getSideToMove();
		int alphaOriginal = alpha;
		//look up position in transposition table
		uint64 zobristKey = createZobrishHash(board, gameInfo);
		transposition_entry& entry = ttable->find(zobristKey);
		if(entry != EMPTY_ENTRY && zobristKey == entry.hashKey && entry.depth >= depthLeft) {
			//found in transposition table
			entry.usefulEntry = true; //mark this as useful
			if(entry.entryType == exact) { //exact match
				return entry.score;
			} else if(entry.entryType == lowerBound) {
				alpha = max(alpha, entry.score);
			} else if(entry.entryType == upperBound) {
				beta = min(beta, entry.score);
			}
			if(alpha >= beta) return entry.score;
		}
		//create pseudo legal moves for this board and side
		std::vector<move> moves;
		move_generator generator(board, gameInfo);
		generator.generatePseudoLegalMoves(side, moves);
		//SORT moves indo descending order based on their move scores (move ordering)
		std::sort(moves.begin(), moves.end(), std::greater<move>());
		//this lambda is used to see which moves are legal
		std::vector<legality_checked> legalityChecks(moves.size());
		bool legalMovesExist = false; //stores if any legal move was found
		for(unsigned int i = 0; i<moves.size(); ++i) { //start checking moves for legality, but only until one legal is found
			if(isLegalMove(moves[i], board, gameInfo)) {
				legalityChecks[i].checked = true; //store that this move was already checked
				legalityChecks[i].legal = true;
				legalMovesExist = true;
				break; //dont check anymore moves
			} else {
				legalityChecks[i].checked = true; //store that this move was already checked
				legalityChecks[i].legal = false;
			}
		}
		if(depthLeft == 0) { //we are at maximum search depth, evaluate
			special_board sb = isSpecialBoard(side, board, legalMovesExist, depth - depthLeft); //detect mates and drawn games
			if(sb.special) {
				return sb.evaluation; //return special evaluation
			} else {
				return evaluateBoard(side, board, gameInfo, moves.size()); //evaulate non special board
			}
		}
		int bestEvaluation = WORST_VALUE;
		move bestMove;
		for(unsigned int i = 0; i<moves.size(); ++i) { //iterate moves, some was already checked for legality!
			move& move = moves[i];
			bool isLegal = legalityChecks[i].checked ? legalityChecks[i].legal : isLegalMove(move, board, gameInfo);
			if(isLegal) { //only evaluate this moves if it ends up being legal
				int capturedPiece = board.makeMove(move, side);
				game_information infoAfterMove = gameInfo; //create a game info object
				updateGameInformation(board, move, infoAfterMove); //update new info object with move
				int evaluation = -alphaBetaNegamax(-beta, -alpha, depthLeft - 1, infoAfterMove); //move down in the tree
				board.unmakeMove(move, side, capturedPiece); //unmake the move before moving on
				if(evaluation > bestEvaluation) {
					bestEvaluation = evaluation;
					bestMove = move;
				}
				if(bestEvaluation > alpha) {
					alpha = bestEvaluation;
				}
				if(bestEvaluation >= beta) {
					break;
				}
			}
		}
		//store move in the transposition table
		unsigned short entryType;
		if(bestEvaluation <= alphaOriginal) {
			entryType = upperBound;
		} else if(bestEvaluation >= beta) {
			entryType = lowerBound;
		} else {
			entryType = exact;
		}
		transposition_entry newEntry(zobristKey, entryType, depth, bestEvaluation, false, bestMove);
		ttable->put(zobristKey, newEntry);
		return alpha;
	}

	move engine::makeMove(game* gameController) {
		const std::vector<move>& gameMoves = gameController->getMoves();
		if(gameMoves.size() > 0) {
			move enemyMove = gameMoves.back(); //update our board with enemy move
			board.makeMove(enemyMove, 1-side);
			updateGameInformation(board, enemyMove, info); //update game information
		}
		move bestMove;
		move bookMove = NULLMOVE;
		if(opening) bookMove = openingBook.getBookMove(board, info);
		if(!(bookMove == NULLMOVE)) { //found a book opening
			std::cout << "I am playing from my opening book!" << std::endl;
			bestMove = bookMove;
		} else {
			std::cout << "I am looking for my move..." << std::endl;
			opening = false; //not in the opening anymore
			bestMove = alphaBetaNegamaxRoot();
		}
		//std::cout << "My board before my move:\n" << board.to_string();
		//update out board with the selected move
		board.makeMove(bestMove, side); //keep board updated
		updateGameInformation(board, bestMove, info);
		return bestMove;
	}

	std::string engine::description() const {
		return "Tchess engine";
	}
}



