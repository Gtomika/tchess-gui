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

#define ALLOWED_ILLEGAL_MOVES 5

#include <iostream>
#include <vector>

#include "TChessRootDialogView.h"

#include "board/board.h"
#include "board/move.h"
#include "player.h"

class TChessRootDialogView;

namespace tchess
{
	class player;

	//Used in the game constructor
	extern const char humanPlayerCode;
	extern const char randomPlayerCode;
	extern const char greedyPlayerCode;
	extern const char engineCode;

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

		//Stores if the game is ongoing or not.
		bool gameEnded;

		/*
		* Stores if the controller should wait for the user to click 'next move' before making the next move.
		*/
		bool waitWithMoves;

		/*
		* True if the game controller expects the 'submitMove' method to be called if a player finished move calculation.
		*/
		bool awaitingCalculation;

		/*
		* True if the game controller expects the 'submitMove' method to be called if a player selected a move from the GUI.
		*/
		bool awaitingGui;

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

		//Used by the controller and players to update the GUI.
		TChessRootDialogView* view;

	public:
		game() = delete;

		/**
		 * Create the game controller object.
		 */
		game(char whiteCode, char blackCode, TChessRootDialogView* view, bool wait);

		~game();

		/*
		 * Gets the move list of the game. Used by player agents to learn about the opponents
		 * moves.
		 */
		const std::vector<move>& getMoves() const;

		//Checks if game has ended.
		bool gameOngoing() const;

		//Starts the game.
		void startGame();

		/*
		* This is the method of the game controller that advances the game.
		*/
		void nextMove();

		/*
		* Used by gui interactive players to submit their move to the controller.
		*/
		void submitMove(const move& m);

		const TChessRootDialogView* getView() const {
			return view;
		}

		bool isAwaitingMove() const {
			return awaitingCalculation;
		}

		bool isAwaitingGui() const {
			return awaitingGui;
		}

		const chessboard& getBoard() const {
			return board;
		}

		const game_information& getInfo() const {
			return info;
		}

	private:
		/*
		* Takes a move and checks if it is legal for the side to move. If legal, updates the game. If not, warnings are displayed. 
		* Checks if the game ended as a result of this move.
		*/
		void acceptMove(move m);

		/*
		 * This method checks if a move is valid. It will take into consideration the board and
		 * the game_information object.
		 *
		 * THIS WILL MAKE THE MOVE ON THE BOARD!!! That is needed to check for legality. If the move turns out to
		 * be illegal, the move must be unmaked.
		 */
		move_legality_result isValidMove(const move& playerMove, std::vector<move>& pseudoLegalMoves);

		/*
		 * Called when the game has ended. Updates the UI with results.
		 */
		void endGame(bool draw, unsigned int winningSide, const std::string& message);

		/*
		* Adds tge latest move to the GUI's move list.
		*/
		void displayMoveOnGui(unsigned int pieceThatMoved, const std::string& appendThis);
	};
}

#endif /* SRC_GAME_GAME_H_ */
