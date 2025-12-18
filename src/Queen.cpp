#include "Queen.h"
#include "Board.h"
#include <cmath>
#include <algorithm> // Necessary

Queen::Queen(int c, char s, Position p) : Piece(c, 'Q', p) {}

bool Queen::canMove(Position new_pos, Board &board) {
    Position cur_pos = getPosition();

    if (new_pos.row < 0 || new_pos.row > 7 || new_pos.col < 0 || new_pos.col > 7) return false;
    if (new_pos.row == cur_pos.row && new_pos.col == cur_pos.col) return false;

    int row_diff = new_pos.row - cur_pos.row;
    int col_diff = new_pos.col - cur_pos.col;

    bool isDiagonal = (abs(row_diff) == abs(col_diff));
    bool isStraight = (row_diff == 0 || col_diff == 0);

    if (!isDiagonal && !isStraight) return false;

    if (isStraight) {
		if (col_diff == 0) { // Straight vertical
            int start = std::min(cur_pos.row, new_pos.row);
            int end = std::max(cur_pos.row, new_pos.row);
            for (int r = start + 1; r < end; r++) {
                if (board.getPieceAt({ r, cur_pos.col }) != nullptr) return false;
            }
        }
		else { // Straight horizontal
            int start = std::min(cur_pos.col, new_pos.col);
            int end = std::max(cur_pos.col, new_pos.col);
            for (int c = start + 1; c < end; c++) {
                if (board.getPieceAt({ cur_pos.row, c }) != nullptr) return false;
            }
        }
    }
    else if (isDiagonal) {
        int r_step = (row_diff > 0) ? 1 : -1;
        int c_step = (col_diff > 0) ? 1 : -1;

        int r = cur_pos.row + r_step;
        int c = cur_pos.col + c_step;

		// Loop with safety counter
		// Safety feature to prevent infinite loops
        int safety = 0;
        while (r != new_pos.row && safety < 10) {
            if (board.getPieceAt({ r, c }) != nullptr) return false;
            r += r_step;
            c += c_step;
            safety++;
        }
    }

    Piece* target = board.getPieceAt(new_pos);
    if (target == nullptr || target->getColor() != getColor()) return true;

    return false;
}