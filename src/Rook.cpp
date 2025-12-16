#include "Rook.h"
#include "Board.h"
#include <cmath>
#include <algorithm> // To jest konieczne dla min() i max()

Rook::Rook(int c, char s, Position p) : Piece(c, 'R', p) {}

bool Rook::canMove(Position new_pos, Board &board) {
    Position cur_pos = getPosition();

    // 1. Ochrona granic
    if (new_pos.row < 0 || new_pos.row > 7 || new_pos.col < 0 || new_pos.col > 7) return false;

    // 2. Nie można stać w miejscu
    if (new_pos.row == cur_pos.row && new_pos.col == cur_pos.col) return false;

    // 3. Sprawdzenie czy ruch jest w linii prostej
    bool isVertical = (new_pos.col == cur_pos.col);
    bool isHorizontal = (new_pos.row == cur_pos.row);

    if (!isVertical && !isHorizontal) return false;

    // --- RUCH PIONOWY (Bezpieczny, bez pętli while) ---
    if (isVertical) {
        int start = std::min(cur_pos.row, new_pos.row);
        int end = std::max(cur_pos.row, new_pos.row);

        // Sprawdzamy tylko pola POMIĘDZY start a end
        for (int r = start + 1; r < end; r++) {
            if (board.getPieceAt({ r, cur_pos.col }) != nullptr) {
                return false; // Przeszkoda
            }
        }
    }
    // --- RUCH POZIOMY (Bezpieczny) ---
    else if (isHorizontal) {
        int start = std::min(cur_pos.col, new_pos.col);
        int end = std::max(cur_pos.col, new_pos.col);

        for (int c = start + 1; c < end; c++) {
            if (board.getPieceAt({ cur_pos.row, c }) != nullptr) {
                return false; // Przeszkoda
            }
        }
    }

    // --- CEL ---
    Piece* target = board.getPieceAt(new_pos);
    if (target == nullptr) return true;
    if (target->getColor() != getColor()) return true;

    return false;
}