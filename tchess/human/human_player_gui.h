#pragma once
#include "game/player.h"
#include "TChessRootDialogView.h"

namespace tchess {

	/*
	* This is the player agent for user controlled player who uses the GUI to make moves.
	*/
	class human_player_gui : public player {

		//A pointer to the view that is used to enter the moves.
		const TChessRootDialogView* dialogRef;

	public:

		human_player_gui(unsigned int side, const TChessRootDialogView* dr) : player(side, true), dialogRef(dr) {}

		/*
		 * Waits until the user clicks 2 squares, then attempts to generate a move from those squares.
		 */
		move makeMove(game* gameController) override;

		/**
		 * Return a string describing this player as human controlled (includes played side).
		 */
		std::string description() const override;
	};
}

