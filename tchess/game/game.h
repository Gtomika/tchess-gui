/*
 * game.h
 *
 * Header file of the game class which is the central class connecting the two players.
 *
 *  Created on: 2021. jan. 24.
 *      Author: Gáspár Tamás
 */

#ifndef SRC_GAME_GAME_H_
#define SRC_GAME_GAME_H_

#include <iostream>
#include <vector>

#include "board/board.h"
#include "board/move.h"
#include "player.h"

namespace tchess
{
	/**
	 * Allows the player to select who will play White and Black sides.
	 */
	char selectPlayerForSide(const std::string& sideName);

	/*
	 * Allows the user to select who will play on both sides. For example, player vs engine or
	 * player vs player.
	 * After selection, the game will be fully player out. Returns true if a new game is to
	 * be started.
	 */
	bool selectPlayersAndStart();

	class player;

	/*
	 * This template connects the two player's sides. It maintaints the game board and checks if
	 * moves submitted are legal. IT also signals the players when it is their turn to move.
	 *
	 * The template parameters are the references for the two players. These objects must have a method
	 * called 'makeMove', which will tell the player/engine/whatever that its their time to make a move.
	 * They also must have a 'description' method which will return text info about the them.
	 *
	 * These player objects will then create their move and send it to the game object using 'submitMove'.
	 */
	class game {

		bool gameEnded;

		bool startNewGame;

		/**
		 * The object that stores the current state of the board.
		 */
		chessboard board;

		/**
		 * Game information object, see above.
		 */
		game_information info;

		/**
		 * Reference to the white player.
		 */
		player* whitePlayer;

		/**
		 * Reference to the black player.
		 */
		player* blackPlayer;

		/*
		 * The moves of the game are stored in this list.
		 */
		std::vector<move> moves;

		/*
		 * Stores how many illegal moves can be submitted by the players before they
		 * automatically lose the match. By default this is 5.
		 */
		unsigned int illegalMoveCounter[2];

	public:
		game() = delete;

		~game() {
			delete whitePlayer;
			delete blackPlayer;
		}

		/**
		 * Create the game, from references to both sides.
		 */
		game(player* wp, player* bp) : gameEnded(false), startNewGame(false),
				whitePlayer(wp), blackPlayer(bp), illegalMoveCounter{5,5} {}

		/**
		 * This method starts the game by requesting the first move from the white player.
		 * Then it will run until the game is not over, and after that it will return true
		 * if the user wants to start another game.
		 */
		bool playGame();

		/*
		 * Gets the move list of the game. Used by player agents to learn about the opponents
		 * moves.
		 */
		const std::vector<move>& getMoves() const;

	private:
		/**
		 * This method asks the player agent whose turn it is to move to submit a move.
		 * This is done by the players 'makeMove' method. Then, it checks if this move
		 * is legal.
		 *  - If legal, then updates the game.
		 *  - If illegal, then asks the player for a new move.
		 */
		void acceptMove();


		/**
		 * This method checks if a move is valid. It will take into consideration the board and
		 * the game_information object.
		 *
		 * THIS WILL MAKE THE MOVE ON THE BOARD!!! That is needed to check for legality. If the move turns out to
		 * be illegal, the move must be unmaked.
		 */
		move_legality_result isValidMove(const move& playerMove, std::vector<move>& pseudoLegalMoves);

		/*
		 * Called when the game has ended. Prints information about the ending.
		 * Returns true if the user chose to start a new game.
		 */
		bool endGame(bool draw, const std::string& winninSide, const std::string& message);
	};
}

#endif /* SRC_GAME_GAME_H_ */
