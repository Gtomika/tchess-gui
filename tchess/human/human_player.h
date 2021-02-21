/*
 * human_player.h
 *
 * Methods and classes related to the player controlled manually
 * by the user are here.
 *
 *  Created on: 2021. jan. 24.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_HUMAN_HUMAN_PLAYER_H_
#define SRC_HUMAN_HUMAN_PLAYER_H_

#include <iostream>
#include <string>

#include "game/game.h"

namespace tchess
{
	//This message is printed when the user asks for help.
	extern const std::string helpMessage;

	/*
	 * This class represents the manually controlled player agent. It has a 'makeMove' method, and so
	 * it is eligible to be passed to a 'game' object.
	 * This class uses the console to interact with the user.
	 */
	class human_player_console: public player {

		//Side of the player.
		unsigned int side;

	public:
		human_player_console(unsigned int side) : side(side) {}

		human_player_console(const human_player_console& other) : side(other.side) {}

		/*
		 * Will prompt the user on the console to enter a move.
		 */
		virtual move makeMove(const game& gameController);

		/**
		 * Return a string describing this player as human controlled (includes played side).
		 */
		virtual std::string description() const;
	};
}

#endif /* SRC_HUMAN_HUMAN_PLAYER_H_ */
