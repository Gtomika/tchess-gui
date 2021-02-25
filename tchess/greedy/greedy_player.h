/*
 * greedy_player.h
 *
 *	This player agent only cares about his next move. Same as
 *	depth 1 search.
 *
 *  Created on: 2021. febr. 1.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_GREEDY_GREEDY_PLAYER_H_
#define SRC_GREEDY_GREEDY_PLAYER_H_

#include "game/player.h"

namespace tchess
{
	/*
	 * This player agent only cares about his next move. Same as
	 *	depth 1 search.
	 */
	class greedy_player: public player {

		//Board object.
		chessboard board;

		//game info object.
		game_information info;

	public:
		greedy_player() = delete;

		greedy_player(unsigned int side) : player(side, false) {}

		/*
		 * Will submit the best move it can find after evaluating the
		 * result of all his possible moves.
		 */
		move makeMove(game* gameController) override;

		std::string description() const override;
	};

}

#endif /* SRC_GREEDY_GREEDY_PLAYER_H_ */
