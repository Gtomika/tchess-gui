#pragma once

#include <string>
#include <vector>

#include "game/game.h"

namespace tchess
{
	struct move_extra_info;
	class move;

	//Represents the result of a game.
	class saved_game {

		std::string whitePlayer;

		std::string blackPlayer;

		std::string result;

		std::string reason;

		std::vector<move> moves;

		std::vector<move_extra_info> moveExtras;

	public:
		saved_game(std::string wpn, std::string bpn, std::string res, std::string rea, 
			const std::vector<move>& moves, const std::vector<move_extra_info> extras);

		//Write this saved game into a file that can be parsed later.
		void writeToFile(CString path);
	};
}