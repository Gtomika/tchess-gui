#include <fstream>

#include "saved_game.h"

tchess::saved_game::saved_game(std::string wpn, std::string bpn, std::string res, std::string rea,
	const std::vector<move>& moves, const std::vector<move_extra_info> extras)
	: whitePlayer(wpn), blackPlayer(bpn), result(res), reason(rea), 
	moves(moves), moveExtras(extras)
{
	
}

void tchess::saved_game::writeToFile(CString p)
{
	CT2CA convertedPath(p);
	std::string path(convertedPath);

	std::ofstream output;
	output.open(path);

	output << whitePlayer << std::endl;
	output << blackPlayer << std::endl;
	output << result << std::endl;
	output << reason << std::endl;
	output << std::endl; //empty line before moves

	for (size_t i = 0; i < moves.size(); ++i) {
		move_extra_info& extra = moveExtras[i];
		move& m = moves[i];
		std::string moveString = m.to_string(extra.pieceThatMoved) + " " + extra.appendToMove;
		output << moveString;
		if (i < moves.size() - 1) {
			output << std::endl;
		}
	}

	output.close();
}
