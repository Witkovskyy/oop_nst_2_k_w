#include "Piece.h"
#include "King.h"
#include "Board.h"
#include <cmath> // dla abs()

using namespace std;

// Konstruktor
King::King(int c, char s, Position p) : Piece(c, 'K', p) {}

bool King::canMove(Position new_pos, Board &board) {
    // 1. Sprawdzenie granic planszy
    if (new_pos.row < 0 || new_pos.row >= 8 || new_pos.col < 0 || new_pos.col >= 8)
        return false;

    Position cur_pos = getPosition();
    int row_diff = abs(new_pos.row - cur_pos.row);
    int col_diff = abs(new_pos.col - cur_pos.col);

    // 2. Geometria ruchu: Król rusza się max o 1 pole
    if (row_diff > 1 || col_diff > 1)
        return false;

    // Nie może zostać w tym samym miejscu
    if (row_diff == 0 && col_diff == 0)
        return false;

    // 3. Sprawdzenie kolizji z własnymi figurami
    Piece* target = board.getPieceAt(new_pos);
    if (target != nullptr && target->getColor() == getColor())
        return false;

    // WAŻNE: Zwracamy true, sprawdzając TYLKO geometrię ruchu.
    // Nie sprawdzamy tutaj "czy wchodzę pod szach" (isKingInCheck), 
    // bo to powoduje nieskończoną pętlę i zawieszenie programu.
    // Odsiewaniem ruchów samobójczych zajmuje się Board::getLegalMoves.
    return true;
}