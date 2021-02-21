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

namespace tchess
{
	class game;

	/*
	 * Base class for all player agents that interact with
	 * the game controller (game class). Subclasses must override
	 * all methods.
	 */
	class player {

	public:
		virtual ~player() {}

		virtual move makeMove(const game& gameController) {
			throw std::runtime_error("Do not directly use player base class!");
		}

		virtual std::string description() const {
			throw std::runtime_error("Do not directly use player base class!");
		}
	};
}

#endif /* SRC_GAME_PLAYER_H_ */
