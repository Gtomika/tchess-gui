/*
 * random_player.h
 *
 *  Created on: 2021. jan. 30.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_RANDOM_RANDOM_PLAYER_H_
#define SRC_RANDOM_RANDOM_PLAYER_H_

#include "game/player.h"

namespace tchess
{
	/*
	 * Player agent that does not do any decision making, it only randomly
	 * selects one legal move and makes that move every time.
	 *
	 * Even though the decision making of this agent is very simple, it still has to
	 * maintain a board representation to be able to list the legal moves.
	 */
	class random_player: public player {

		//Board object.
		chessboard board;

		//game info object.
		game_information info;

	public:
		random_player() = delete;

		random_player(unsigned int side, TChessRootDialogView* view) : player(side, false, view) {}

		/*
		 * Will submit one randomly selected legal move.
		 */
		move makeMove(game* gameController) override;

		std::string description() const override;
	};
}

#endif /* SRC_RANDOM_RANDOM_PLAYER_H_ */
