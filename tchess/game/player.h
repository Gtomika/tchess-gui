/*
 * player.h
 *
 *  Created on: 2021. jan. 30.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_GAME_PLAYER_H_
#define SRC_GAME_PLAYER_H_

#include <stdexcept>
#include <string>

#include "game.h"
#include "board/move.h"

namespace tchess
{
	class game;

	/*
	 * Base class for all player agents that interact with
	 * the game controller (game class). Subclasses must override
	 * all methods.
	 */
	class player {

	protected:

		//Side of the player
		unsigned int side;

		//If the agent uses GUI to make its move
		bool usesGui;

	public:
		player(unsigned int side, bool useGui) : side(side), usesGui(useGui) {}

		virtual ~player() {}

		virtual move makeMove(game* gameController) abstract;

		virtual std::string description() const abstract;

		bool isGuiInteractive() {
			return usesGui;
		}
	};
}

#endif /* SRC_GAME_PLAYER_H_ */
