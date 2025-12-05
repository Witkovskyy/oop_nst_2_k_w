#pragma once
#include "../Board.h"
#include "val.h"
#include "deepcopy.h"
#include "moves.h"
#include <algorithm>
#include <vector>
#include <limits>

// count nodes visited by negamax
static long nodesVisited = 0;
long get_nodes_visited() { return nodesVisited; }

inline int to01(int sign) { return sign > 0 ? 0 : 1; }
inline int toSign(int color01) { return color01 == 0 ? +1 : -1; }

inline bool isValidPos(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

int getPstValue(const int table[8][8], int row, int col, int color)
{
	if (color == 0) // WHITE
		//return table[row][col];
        return table[7 - row][col];
	else // BLACK
		//return table[7 - row][col];
        return table[row][col];

}
struct Undo
{
    Position from;
    Position to;
    Piece* pieceMoved;
    Piece* pieceCaptured;
};

static void undoMove(Board& board, const Move& move, const Undo& undo)
{
    board.squares[move.from.row][move.from.col] = undo.pieceMoved;
    board.squares[move.to.row][move.to.col] = undo.pieceCaptured;

    if (undo.pieceMoved) {
        undo.pieceMoved->setPosition(move.from);
    }

	// Double check for captured piece
    if (undo.pieceCaptured) {
        undo.pieceCaptured->setPosition(move.to);
    }
}
static void applyMove(Board& board, const Move& move, Undo& undo)
{
    undo.from = move.from;
    undo.to = move.to;
    undo.pieceMoved = board.getPieceAt(move.from);
    undo.pieceCaptured = board.getPieceAt(move.to); // Might be nullptr

	// Raw move application
    // Delete piece
    board.squares[move.from.row][move.from.col] = nullptr;

    // Place piece
    board.squares[move.to.row][move.to.col] = undo.pieceMoved;

	// Update piece position
    if (undo.pieceMoved) {
        undo.pieceMoved->setPosition(move.to);
    }
}

// HERE IS A BASIC IMPLEMENTATION OF isInCheck AND isSquareAttacked
// WILL BE REPLACED LATER WITH MORE OPTIMIZED VERSION
// NOW ITS JUST GEMINI GENERATED CODE NEEDED FOR TESTING PURPOSES
bool isSquareAttacked(Board& board, Position pos, int attackerColor)
{
    int pawnDir = (attackerColor == 0) ? -1 : 1;

    // Diagonals for pawn
    if (isValidPos(pos.row - pawnDir, pos.col - 1)) {
        Piece* p = board.getPieceAt({ pos.row - pawnDir, pos.col - 1 });
        if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'P') return true;
    }
    if (isValidPos(pos.row - pawnDir, pos.col + 1)) {
        Piece* p = board.getPieceAt({ pos.row - pawnDir, pos.col + 1 });
        if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'P') return true;
    }

    // Knights
    static int knightMoves[8][2] = { {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1} };
    for (auto& m : knightMoves) {
        int r = pos.row + m[0];
        int c = pos.col + m[1];
        if (isValidPos(r, c)) {
            Piece* p = board.getPieceAt({ r, c });
            if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'N') return true;
        }
    }

    // Straights
    static int rookDirs[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
    for (auto& d : rookDirs) {
        for (int i = 1; i < 8; ++i) {
            int r = pos.row + d[0] * i;
            int c = pos.col + d[1] * i;
            if (!isValidPos(r, c)) break;

            Piece* p = board.getPieceAt({ r, c });
            if (p) {
                if (p->getColor() == attackerColor) {
                    char s = toupper(p->getSymbol());
                    if (s == 'R' || s == 'Q') return true;
                }
				break; // Blocked
            }
        }
    }

    // Diagonals
    static int bishopDirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    for (auto& d : bishopDirs) {
        for (int i = 1; i < 8; ++i) {
            int r = pos.row + d[0] * i;
            int c = pos.col + d[1] * i;
            if (!isValidPos(r, c)) break;

            Piece* p = board.getPieceAt({ r, c });
            if (p) {
                if (p->getColor() == attackerColor) {
                    char s = toupper(p->getSymbol());
                    if (s == 'B' || s == 'Q') return true;
                }
                break;
            }
        }
    }

    // King
    static int kingMoves[8][2] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };
    for (auto& m : kingMoves) {
        int r = pos.row + m[0];
        int c = pos.col + m[1];
        if (isValidPos(r, c)) {
            Piece* p = board.getPieceAt({ r, c });
            if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'K') return true;
        }
    }

    return false;
}
bool isInCheck(Board& board, int color)
{
	// Find king position
    Position kingPos = { -1, -1 };

	// Unoptimized search for king
	// We need to store king position to optimize later
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPieceAt({ r, c });
            if (p && p->getColor() == color && toupper(p->getSymbol()) == 'K') {
                kingPos = { r, c };
                goto found; // gemini ahh moment
            }
        }
    }
found:;

    if (kingPos.row == -1) return true; // No king, we treat as game over

    // Check king square
    int enemyColor = (color == 0) ? 1 : 0;
    return isSquareAttacked(board, kingPos, enemyColor);
}



int eval(const Board& board, int color)
{
    int whiteScore = 0;
    int blackScore = 0;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece* cell = board.squares[row][col];
            if (!cell || cell->symbol == 'E') continue;

            int material = pieceValFromSymbol(cell->symbol);

            // PST
            int positional = 0;
            char sym = toupper(cell->symbol);
            switch (sym) {
            case 'P': positional = getPstValue(pawnPST, row, col, cell->color); break;
            case 'N': positional = getPstValue(knightPST, row, col, cell->color); break;
            case 'B': positional = getPstValue(bishopPST, row, col, cell->color); break;
            case 'R': positional = getPstValue(rookPST, row, col, cell->color); break;
            case 'Q': positional = getPstValue(queenPST, row, col, cell->color); break;
            case 'K': positional = getPstValue(kingPST, row, col, cell->color); break;
            }
           
            int total = material + positional;

            if (cell->color == 0) // White
                whiteScore += total;
            else // Black
                blackScore += total;
        }
    }

    int finalScore = whiteScore - blackScore;

    // DEBUG
    static int debugCounter = 0;
    if (debugCounter < 5 && abs(finalScore) > 20) {
        std::cout << "[EVAL CHECK] White: " << whiteScore << " | Black: " << blackScore
            << " | Diff: " << finalScore << std::endl;
        debugCounter++;
    }

    return finalScore * color;
}

static std::vector<Move> legalMoves(Board& board, int color) {
    // Pseudo moves
    auto quiet = generateQuietMoves(board, color);
    auto caps = generateAllCaptures(board, color);
    quiet.insert(quiet.end(), caps.begin(), caps.end()); // quiet has now all moves

    std::vector<Move> realLegalMoves;
    realLegalMoves.reserve(quiet.size());

	// Check if moves leave king in check
    for (auto& move : quiet) {
        Undo undo;
        applyMove(board, move, undo); // Try move

		// Is king in check after move?
        if (!isInCheck(board, color)) {
            realLegalMoves.push_back(move); // Legal if no check
        }

        undoMove(board, move, undo); // Undo
    }

    return realLegalMoves;
}

static std::vector<Move> generateCaptures(Board &board, int color)
{
    return generateAllCaptures(board, color);
}

static int scoreMove(const Move& move) {
    // CAPTURES (MVV-LVA)
    if (move.pieceCaptured != nullptr) {
        int victimVal = pieceValFromSymbol(move.pieceCaptured->getSymbol());
        int attackerVal = pieceValFromSymbol(move.pieceMoved->getSymbol());

        // Equation: Victim * 10 - Attacker.
        return 10000 + (victimVal * 10) - attackerVal;
    }

    // PROMOTION
	// HERE TO IMPLENENT PROMOTION BONUS
    char sym = toupper(move.pieceMoved->getSymbol());
    if (sym == 'P') {
        int r = move.to.row;
		// IF PAWN LAST RANK - PROMOTION
        if (r == 0 || r == 7) {
			return 9000; // Very high score for promotion
        }
    }

    // Killer Moves / History Heuristic - for future
	// Temporarily return 0 for non-captures, non-promotions, as we have no other heuristics yet

    return 0;
}

static void orderMoves(std::vector<Move>& moves)
{
	// Sort descending by score
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return scoreMove(a) > scoreMove(b);
        });
}

// game over if one king is missing
static bool gameOver(const Board &board)
{
    bool whiteKing = false, blackKing = false;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece *cell = board.squares[row][col];
            if (cell == nullptr)
                continue;
            if (cell->symbol == 'K')
            {
                if (cell->color == 0) // WHITE
                    whiteKing = true;
                else
                    blackKing = true;
            }
        }
    }
    return !(whiteKing && blackKing);
}

// Quiescence search to avoid horizon effect
int quiescence(Board &board, int alpha, int beta, int color)
{
    int stand = eval(board, color);
    if (stand >= beta)
        return beta;
    if (stand > alpha)
        alpha = stand;

    auto caps = generateCaptures(board, to01(color)); // Only consider capture moves
	orderMoves(caps);
    for (auto &move : caps)
    {
        Undo undo;
        applyMove(board, move, undo);

		// Check for game over after the capture
   //     if (gameOver(board)) {
   //         undoMove(board, move, undo);
			//return 100000; // King eaten, very high score
   //     }

        int score = -quiescence(board, -beta, -alpha, -color);
        undoMove(board, move, undo);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

// Negamax algorithm with alpha-beta pruning
// Cutting off branches that won't influence the final decision
int negamax(Board &board, int depth, int alpha, int beta, int color)
{
    ++nodesVisited;
	// Check for game over
	// King eaten, return very high negative score
    //if (gameOver(board)) return -100000 - depth;

    if (depth == 0 || gameOver(board))
        return quiescence(board, alpha, beta, color);

    auto moves = legalMoves(board,to01(color));

    if (moves.empty()) {
		// No legal moves, check for checkmate or stalemate
        // if (board.isCheck(to01(color))) return -100000 + depth; // CHECKMATE
		return 0; // Stalemate
    }

    orderMoves(moves);

    int best = -INF;

    for (auto &move : moves)
    {
        Undo undo;
        applyMove(board, move, undo);
        int score = -negamax(board, depth - 1, -beta, -alpha, -color);
        undoMove(board, move, undo);
        if (score > best)
            best = score;
        if (score > alpha)
            alpha = score;
        if (alpha >= beta)
            break; // beta cutoff
    }
    return best;
}

// API freaking out, set default to 0 - white
//int negamax(Board &board, int depth, int alpha, int beta)
//{
//    return negamax(board, depth, alpha, beta);
//}
