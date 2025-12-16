#include "kNight.h" // Lub "Knight.h" - sprawdź nazwę pliku
#include "Board.h"
#include <cmath>

Knight::Knight(int c, char s, Position p) : Piece(c, 'N', p) {}

bool Knight::canMove(Position new_pos, Board* board) {
    Position cur_pos = getPosition();

    // BARDZO WAŻNE: Sprawdzenie granic przed wszystkim innym
    if (new_pos.row < 0 || new_pos.row > 7 || new_pos.col < 0 || new_pos.col > 7) return false;

    int row_diff = abs(new_pos.row - cur_pos.row);
    int col_diff = abs(new_pos.col - cur_pos.col);

    // Ruch "L"
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        Piece* target = board->getPieceAt(new_pos);
        if (target == nullptr || target->getColor() != getColor()) return true;
        return false;
    }
    return false;
}