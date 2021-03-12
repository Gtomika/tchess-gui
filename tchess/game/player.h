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
#include <vector>
#include <stack>

#include "TChessRootDialogView.h"

#include "game.h"
#include "board/move.h"

class TChessRootDialogView;

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

		//Pointer to the GUI
		TChessRootDialogView* view;

	public:
		player(unsigned int side, bool useGui, TChessRootDialogView* v) : side(side), usesGui(useGui), view(v) {}

		virtual ~player() {}

		virtual move makeMove(game* gameController) abstract;

		virtual std::string description() const abstract;

		bool isGuiInteractive() {
			return usesGui;
		}
	};

	struct move_extra_info;
	class move;

	/*
	* A special kind of player used by the game controller to play
	* out saved games.
	*/
	class saved_game_player : public player {

		//This player gets all of his moves when created.
		std::stack<move> moveStack;

		//This player gets its name when created.
		std::string name;

	public:
		saved_game_player() = delete;

		/*
		* Create a saved game player, who already knows all his moves.
		*/
		saved_game_player(UINT side, const std::vector<move>& m, const std::string& n);

		/*
		* The make move method simply gets a move from the stack.
		*/
		move makeMove(game* gameController) {
			//should not happen
			if (moveStack.empty()) return move(0, 0, resignMove, 0);
			move m = moveStack.top();
			moveStack.pop();
			return m;
		}

		//the name method simply returns the received name.
		std::string description() const {
			return name;
		}

	};

	//Represents the result of a game.
	class saved_game {

		//Name of white player.
		std::string whitePlayer;

		//Name of black player.
		std::string blackPlayer;

		//The result of the game.
		std::string result;

		//The reason for the result (checkmate, resignation, ...).
		std::string reason;

		//Moves in this game.
		std::vector<move> moves;

		//Extra info about the moves.
		std::vector<move_extra_info> moveExtras;

	public:
		saved_game(std::string wpn, std::string bpn, std::string res, std::string rea,
			const std::vector<move>& moves, const std::vector<move_extra_info> extras);

		//Write this saved game into a file that can be parsed later.
		void writeToFile(CString path);

		const std::vector<move>& getMoves() const {
			return moves;
		}

		const std::string& getWhiteName() const {
			return whitePlayer;
		}

		const std::string& getBlackName() const {
			return blackPlayer;
		}
	};

	/*
	* Returns a saved game object created from the save file at the given path.
	*/
	saved_game readSaveFromFile(CString path);
}

#endif /* SRC_GAME_PLAYER_H_ */
