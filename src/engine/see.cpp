#include "../Piece.h"
#include "../Board.h"
#include "val.h" 
#include <algorithm>
#include <vector>
#include <cmath>

// Helper border check, inline makes it faster

inline bool isValid(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

// Stable directions for pieces
static int knightDeltas[8][2] = { {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1} };
static int rookDeltas[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
static int bishopDeltas[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

/// Find cheapest attacker of given color on target square.
/// Returns position of attacker and sets outValue to its value.
/// If no attacker found, returns {-1,-1} and outValue is undefined.
static Position getCheapestAttacker(Board& board, Position target, int color, int& outValue)
{
    Position bestPos = { -1, -1 };
    int bestVal = 100000; // INF

    int r = target.row;
    int c = target.col;

    /// PAWN
	/// We check where a pawn of given color could attack from.
	/// so pawn has to be on r-1 (white) or r+1 (black)
    /// moves.cpp: White=+1, Black=-1.
	/// Attacker is r-1 for White, r+1 for Black

	int pawnDir = (color == 0) ? -1 : 1; // Where did the pawn come from

    // Check for both diagonals
    for (int dc : {-1, 1}) {
        int pr = r + pawnDir;
        int pc = c + dc;
        if (isValid(pr, pc)) {
            Piece* p = board.getPieceAt({ pr, pc });
            if (p && p->getColor() == color && toupper(p->getSymbol()) == 'P') {
				// Pawn is always the cheapest attacker
                outValue = pieceValFromSymbol(p->symbol);
				return { pr, pc }; // Pawn found, return immediately cause can't be cheaper
            }
        }
    }

	/// KNIGHT
    for (auto& d : knightDeltas) {
        int nr = r + d[0];
        int nc = c + d[1];
        if (isValid(nr, nc)) {
            Piece* p = board.getPieceAt({ nr, nc });
            if (p && p->getColor() == color && toupper(p->getSymbol()) == 'N') {
				int val = pieceValFromSymbol(p->symbol); // Knight value
                if (val < bestVal) {
                    bestVal = val;
                    bestPos = { nr, nc };
                }
            }
        }
    }

	// Theoretically we could have multiple attackers of same type,
	// so we continue searching for cheaper ones.
	// Even if we found a knight, we still look for bishops, rooks, queens, kings.

	// SLIDERS: BISHOP, ROOK, QUEEN
	// Helper for scanning in directions
    auto scan = [&](int dirs[][2], int numDirs, char type1, char type2) {
        for (int i = 0; i < numDirs; ++i) {
            for (int k = 1; k < 8; ++k) {
                int nr = r + dirs[i][0] * k;
                int nc = c + dirs[i][1] * k;

                if (!isValid(nr, nc)) break;

                Piece* p = board.getPieceAt({ nr, nc });
                if (p) { // Found
                    if (p->getColor() == color) {
                        char s = toupper(p->getSymbol());
                        if (s == type1 || s == type2 || s == 'Q') {
                            int val = pieceValFromSymbol(s);
                            if (val < bestVal) {
                                bestVal = val;
                                bestPos = { nr, nc };
                            }
                        }
                    }
                    break; // Blocked, stop
                }
            }
        }
        };

    // Diagonals
	scan(bishopDeltas, 4, 'B', 'B'); // Double Bishop
    // Straights
    scan(rookDeltas, 4, 'R', 'R');

    //KING
    for (auto& d : bishopDeltas) { // Moves like a Queen but only one square
		// Simplified checking for area around king
		// This is not very efficient but king is rare so it's ok
        int nr = r + d[0]; int nc = c + d[1];
        if (isValid(nr, nc)) {
            Piece* p = board.getPieceAt({ nr, nc });
            if (p && p->getColor() == color && toupper(p->getSymbol()) == 'K') {
				// We take king only if there is no other choice
                if (20000 < bestVal) { bestVal = 20000; bestPos = { nr, nc }; }
            }
        }
    }

    outValue = bestVal;
    return bestPos;
}


int see(Board& board, Position target, int sideToMove)
{
	// Starting value of the victim
    Piece* victim = board.getPieceAt(target);
    int value = victim ? pieceValFromSymbol(victim->getSymbol()) : 0;

    // Swap list
    std::vector<int> gain;
    gain.reserve(16);
    gain.push_back(value);

	// For optimal performance, we could try to delete one figure 
	// in cache but its tricky and error-prone.
	// We stay with simple copy for safety. (Maybe we can optimize later?)

    Board copy = board; // Only one copy

    int stm = sideToMove;

    while (true)
    {
        int attackerValue = 0;
        Position from = getCheapestAttacker(copy, target, stm, attackerValue);

		// If no attacker found, stop
        if (from.row == -1) break;

		// New value after capture, new victim is the attacker
        gain.push_back(attackerValue);

		// Pseudo move the attacker to the target square
		// We then open up the x-ray attacks for the next iteration
        Piece* attackerPiece = copy.getPieceAt(from);
        copy.movePiece(from, target, attackerPiece); // target now has the attacker

        // Switch sides
        stm = (stm == 0) ? 1 : 0;

		// Optional - if king is the attacker, stop immediately, since we don't want to consider further captures
        if (toupper(attackerPiece->getSymbol()) == 'K') break;
    }

    // Back-propagation
    // gain[0] is the value of first capture
	// gain[1] is value of the attacker, that captures first victim
    // EQ: score = val_captured - see(next)

    int sz = gain.size();
	int score = 0; // If we stop at the last move ergo no more attackers, score is 0

    // Iterate to the end of the list
    for (int i = sz - 1; i >= 1; --i) {
        score = std::max(0, gain[i] - score);
    }

    // Return sidetomove score
    return gain[0] - score;
}