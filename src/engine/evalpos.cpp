#include "../Board.h"
#include "val.h"
#include "moves.h"
#include <algorithm>
#include <vector>
#include <limits>
#include "logger/logger.h"
#include <string>
#include "tables/zobrist.h"
#include "tables/TT.h"
#include "engine.h"
#include <cctype> // Necessary for toupper

// count nodes visited by negamax
static long nodesVisited = 0;
long get_nodes_visited() { return nodesVisited; }

inline int to01(int sign) { return sign > 0 ? 0 : 1; }
inline int toSign(int color01) { return color01 == 0 ? +1 : -1; }
inline bool isValidPos(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }

int getPstValue(const int table[8][8], int row, int col, int color)
{
    if (color == 0) // WHITE
        return table[7 - row][col];
    else // BLACK
        return table[row][col];
}

struct Undo
{
    Position from;
    Position to;
    Piece* pieceMoved;
    Piece* pieceCaptured;
    char promotion;
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
    // Handle promotion revert
    if (move.promotion != 0) {
        undo.pieceMoved->setSymbol('P');
    }

    int fromSq = move.from.row * 8 + move.from.col;
    int toSq = move.to.row * 8 + move.to.col;
    Piece* p = undo.pieceMoved; // Piece that moved
    int pIdx = getPieceIndex(p->getSymbol(), p->getColor());

    // Revert side
    board.zobristKey ^= sideKey;

    // Bring back the Zobrist key
    if (move.promotion != 0) {
        int promoIdx = getPieceIndex(move.promotion, p->getColor());
        board.zobristKey ^= pieceKeys[promoIdx][toSq];
    }
    else {
        board.zobristKey ^= pieceKeys[pIdx][toSq];
    }

    // Revert captured
    if (undo.pieceCaptured) {
        int capIdx = getPieceIndex(undo.pieceCaptured->getSymbol(), undo.pieceCaptured->getColor());
        board.zobristKey ^= pieceKeys[capIdx][toSq];
    }

    // Insert piece back
    board.zobristKey ^= pieceKeys[pIdx][fromSq];
}

static void applyMove(Board& board, const Move& move, Undo& undo)
{
    undo.from = move.from;
    undo.to = move.to;
    undo.pieceMoved = board.getPieceAt(move.from);
    undo.pieceCaptured = board.getPieceAt(move.to); // Might be nullptr
    undo.promotion = move.promotion;

    int fromSq = move.from.row * 8 + move.from.col;
    int toSq = move.to.row * 8 + move.to.col;

    Piece* p = board.getPieceAt(move.from);
    int pIdx = getPieceIndex(p->getSymbol(), p->getColor());

    // Zobrist remove piece
    board.zobristKey ^= pieceKeys[pIdx][fromSq];
    // Zobrist If captured, remove captured piece 
    if (undo.pieceCaptured) {
        int capIdx = getPieceIndex(undo.pieceCaptured->getSymbol(), undo.pieceCaptured->getColor());
        board.zobristKey ^= pieceKeys[capIdx][toSq];
    }

    // Zobrist insert piece
    // If promoted put new figure
    if (move.promotion != 0) {
        int promoIdx = getPieceIndex(move.promotion, p->getColor());
        board.zobristKey ^= pieceKeys[promoIdx][toSq];
    }
    else {
        board.zobristKey ^= pieceKeys[pIdx][toSq];
    }

    // Switch side
    board.zobristKey ^= sideKey;

    // Raw move application
    // Delete piece
    board.squares[move.from.row][move.from.col] = nullptr;
    // Place piece
    board.squares[move.to.row][move.to.col] = undo.pieceMoved;

    // Update piece position
    if (undo.pieceMoved) {
        undo.pieceMoved->setPosition(move.to);
    }
    // Handle promotion
    if (move.promotion != 0) {
        undo.pieceMoved->setSymbol(move.promotion);
    }
}

// ROBUST implementation of isSquareAttacked (with explicit loops for safety)
bool isSquareAttacked(Board& board, Position pos, int attackerColor)
{
    // 1. Pawn attacks
    // If attacker is White (0), pawn must be 'below' (row-1) attacking up.
    // If attacker is Black (1), pawn must be 'above' (row+1) attacking down.
    int pawnRow = (attackerColor == 0) ? pos.row - 1 : pos.row + 1;

    if (pawnRow >= 0 && pawnRow < 8) {
        // Attack from left diagonal
        if (pos.col - 1 >= 0) {
            Piece* p = board.squares[pawnRow][pos.col - 1];
            if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'P') return true;
        }
        // Attack from right diagonal
        if (pos.col + 1 < 8) {
            Piece* p = board.squares[pawnRow][pos.col + 1];
            if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'P') return true;
        }
    }

    // 2. Knight attacks
    // Manual unroll for safety to avoid vector overhead or index mistakes
    static int kDr[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
    static int kDc[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    for (int i = 0; i < 8; ++i) {
        int r = pos.row + kDr[i];
        int c = pos.col + kDc[i];
        if (isValidPos(r, c)) {
            Piece* p = board.squares[r][c];
            if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'N') return true;
        }
    }

    // 3. Sliding attacks (ROOK / QUEEN) - Explicit Loops
    // UP
    for (int r = pos.row + 1; r < 8; ++r) {
        Piece* p = board.squares[r][pos.col];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'R' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }
    // DOWN
    for (int r = pos.row - 1; r >= 0; --r) {
        Piece* p = board.squares[r][pos.col];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'R' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }
    // RIGHT
    for (int c = pos.col + 1; c < 8; ++c) {
        Piece* p = board.squares[pos.row][c];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'R' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }
    // LEFT
    for (int c = pos.col - 1; c >= 0; --c) {
        Piece* p = board.squares[pos.row][c];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'R' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }

    // 4. Sliding attacks (BISHOP / QUEEN) - Explicit Loops
    // UP-RIGHT
    for (int i = 1; ; ++i) {
        int r = pos.row + i; int c = pos.col + i;
        if (!isValidPos(r, c)) break;
        Piece* p = board.squares[r][c];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'B' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }
    // UP-LEFT
    for (int i = 1; ; ++i) {
        int r = pos.row + i; int c = pos.col - i;
        if (!isValidPos(r, c)) break;
        Piece* p = board.squares[r][c];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'B' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }
    // DOWN-RIGHT
    for (int i = 1; ; ++i) {
        int r = pos.row - i; int c = pos.col + i;
        if (!isValidPos(r, c)) break;
        Piece* p = board.squares[r][c];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'B' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }
    // DOWN-LEFT
    for (int i = 1; ; ++i) {
        int r = pos.row - i; int c = pos.col - i;
        if (!isValidPos(r, c)) break;
        Piece* p = board.squares[r][c];
        if (p) {
            if (p->getColor() == attackerColor) {
                char s = toupper(p->getSymbol());
                if (s == 'B' || s == 'Q') return true;
            }
            break; // Blocked
        }
    }

    // 5. King attacks
    static int kDrK[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    static int kDcK[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int i = 0; i < 8; ++i) {
        int r = pos.row + kDrK[i];
        int c = pos.col + kDcK[i];
        if (isValidPos(r, c)) {
            Piece* p = board.squares[r][c];
            if (p && p->getColor() == attackerColor && toupper(p->getSymbol()) == 'K') return true;
        }
    }

    return false;
}

bool isInCheck(Board& board, int color)
{
    // Find king position - robust search
    Position kingPos = { -1, -1 };
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.squares[r][c];
            if (p && p->getColor() == color && toupper(p->getSymbol()) == 'K') {
                kingPos = { r, c };
                goto found_king;
            }
        }
    }
found_king:;

    // No king found, we treat as game over (or checkmate in specific context)
    if (kingPos.row == -1) return true;

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
            // PST (Piece Square Table)
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
    return finalScore * color;
}

static std::vector<Move> legalMoves(Board& board, int color) {
    // Generate pseudo-legal moves
    auto quiet = generateQuietMoves(board, color);
    auto caps = generateAllCaptures(board, color);
    quiet.insert(quiet.end(), caps.begin(), caps.end()); // Merge moves

    std::vector<Move> realLegalMoves;
    realLegalMoves.reserve(quiet.size());

    // Check if moves leave king in check
    for (auto& move : quiet) {
        Undo undo;
        applyMove(board, move, undo); // Try move

        // Is king in check after move?
        if (!isInCheck(board, color)) {
            realLegalMoves.push_back(move);
            // Legal if no check
        }

        undoMove(board, move, undo);
        // Undo
    }

    return realLegalMoves;
}

static std::vector<Move> generateCaptures(Board& board, int color)
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

    // PROMOTION BONUS
    char sym = toupper(move.pieceMoved->getSymbol());
    if (sym == 'P') {
        int r = move.to.row;
        // IF PAWN LAST RANK
        if (r == 0 || r == 7) {
            return 9000;
        }
    }

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
static bool gameOver(const Board& board)
{
    bool whiteKing = false, blackKing = false;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece* cell = board.squares[row][col];
            if (cell == nullptr) continue;
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
int quiescence(Board& board, int alpha, int beta, int color)
{
    int stand = eval(board, color);
    if (stand >= beta)
        return beta;
    if (stand > alpha)
        alpha = stand;

    auto caps = generateCaptures(board, to01(color));
    // Only consider capture moves
    orderMoves(caps);
    for (auto& move : caps)
    {
        Undo undo;
        applyMove(board, move, undo);

        int score = -quiescence(board, -beta, -alpha, -color);
        undoMove(board, move, undo);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

bool isRepetition(const Board& board) {
    // Move is a repetition if current zobristKey appeared before in positionHistory
    for (int i = board.positionHistory.size() - 1; i >= 0; --i) {
        if (board.positionHistory[i] == board.zobristKey) {
            return true;
            // Repetition found
        }
    }
    return false;
}

// Negamax algorithm with alpha-beta pruning
int negamax(Board& board, int depth, int alpha, int beta, int color)
{
    if (isRepetition(board)) {
        // 0 is equal position, slight minus for engine to avoid repetition
        return -35;
    }

    ++nodesVisited;
    int ttScore;
    Move ttMove;
    // Read hash
    unsigned long long key = board.zobristKey;
    if (TT.probe(key, depth, alpha, beta, ttScore, ttMove)) {
        return ttScore;
        // Score found in TT, immediately return
    }

    if (depth == 0 || gameOver(board))
        return quiescence(board, alpha, beta, color);

    auto moves = legalMoves(board, to01(color));

    if (moves.empty()) {
        // No legal moves, check for checkmate or stalemate
        if (isInCheck(board, to01(color))) return -100000 + depth; // CHECKMATE
        return 0; // STALEMATE
    }

    orderMoves(moves);

    int best = -INF;
    Move bestMove;
    // For TT storage
    int oldAlpha = alpha;
    for (auto& move : moves)
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
            break;
        // beta cutoff
    }

    TTFlag flag = TT_EXACT;
    if (best <= oldAlpha) flag = TT_ALPHA; // Not better than alpha
    else if (best >= beta) flag = TT_BETA;
    // Cutoff

    TT.store(key, best, depth, flag, bestMove);

    return best;
}