#include "human_player_gui.h"

namespace tchess
{
    move human_player_gui::makeMove(const game& gameController)
    {
        return move();
    }

    std::string human_player_gui::description() const
    {
        std::string sideName = side == white ? "White" : "Black";
        return "Human controlled player (" + sideName + ")";
    }
}
