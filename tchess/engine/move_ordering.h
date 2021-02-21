/*
 * move_ordering.h
 *
 *  Created on: 2021. febr. 10.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_ENGINE_MOVE_ORDERING_H_
#define SRC_ENGINE_MOVE_ORDERING_H_

#include "board/board.h"

namespace tchess
{
	/*
	 * Extracts the principal variation (best move sequence) . Then prints it.
	 */
	void printPrincipalVariation(chessboard& board, const game_information& info, unsigned int depth);

}

#endif /* SRC_ENGINE_MOVE_ORDERING_H_ */
