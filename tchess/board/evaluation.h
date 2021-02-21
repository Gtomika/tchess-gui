/*
 * evaluation.h
 *
 *  Created on: 2021. febr. 1.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_BOARD_EVALUATION_H_
#define SRC_BOARD_EVALUATION_H_

#include "board.h"

namespace tchess
{
	//piece values
	extern const int pieceValues[7];

	//piece square tables (for both sides)
	extern const int pawnTable[2][64];

	extern const int knightTable[2][64];

	extern const int bishopTable[2][64];

	extern const int rookTable[2][64];

	extern const int kingTable[2][64];

	extern const int kingEndgameTable[2][64];

	extern const int queenTable[2][64];

	struct special_board {
			bool special;
			int evaluation;
	};

	/*
	 * Worst possible value that the evaluation can return. For example when checkmated.
	 */
	extern const int WORST_VALUE;

	/*
	 * Best possible value that can happen during evaluation.
	 */
	extern const int BEST_VALUE;

	/*
	 * Checks if the position is special in a way that it does not need to be evaluated. For example
	 * mates and insufficient material. Legal moves parameter is true when there are still legal moves
	 * in this position.
	 */
	special_board isSpecialBoard(unsigned int enemySide, const chessboard& board, bool legalMoves, int depth);

	/*
	 * Returns true if the board is in the endgame.
	 * Definition: it is endgame if both sides have at most 13 worth of material (king not counted).
	 */
	bool isEndgame(const chessboard& board);

	/*
	 * Will count from how many diagonals a bishop is locked in BY PAWNS. The less the better
	 */
	int bishopLockedFactor(unsigned int square, const chessboard& board);

	/*
	 * Evaluates if a rook is on an open file. It will check what kind of pawns are
	 * on said file.
	 * Returns how good the situation is for "side".
	 */
	int rookFileEvaluation(unsigned int side, unsigned int square, const chessboard& board);

	/*
	 * Checks if a pawn is passed, meaning there are no enemy pawns ahead of it in the same file
	 * or the files right next to it.
	 */
	bool isPassedPawn(unsigned int side, unsigned int square, const chessboard& board);

	/*
	 * Checks if there are doubled pawns on the file of 'square'. Returns the number of pawns
	 * from the given side.
	 */
	unsigned int doubledPawnEvaluation(unsigned int side, unsigned int square, const chessboard& board);

	/*
	 * Checks if the pawn is isolated, meaning it has no friendly pawns on the neighbouring squares.
	 */
	bool isIsolatedPawn(unsigned int side, unsigned int square, const chessboard& board);

	/*
	 * Evaluates king safety by counting friendly and enemy pieces in the kings quadrant of the board.
	 * Queen counts as 3 pieces.
	 * The higher the return value, the safer it is for the side.
	 */
	int kingSafetyEvaluation(unsigned int side, unsigned int square, const chessboard& board);

	/*
	 * Main static evaluator function. Used in negamax, so it evaluates relative to the side to move.
	 *
	 * This method must be called after the move has been made on the board,
	 * then after evaluation the move can be unmade.
	 *
	 * Parameters:
	 *  - side: the side who made the last move on the board and for whom the evaluation is happening.
	 *  - board: the board object
	 *  - info: game info object needed to generate the enemy moves (to check for mates)
	 *  - The amount of PSEUDO LEGAL moves this side has.
	 */
	int evaluateBoard(unsigned int side, const chessboard& board,
			const game_information& info, unsigned int moveAmount);
}

#endif /* SRC_BOARD_EVALUATION_H_ */
