/**
 * @file evalpos.cpp
 * @brief File implementation for position evaluation.
 * @version 0.1
 * @date 2026-01-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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
/**
 * @brief Returns the number of nodes visited by the search.
 *
 * This counter is incremented in Engine::negamax() (and optionally in quiescence,
 * depending on your design). It is useful for profiling and debugging performance.
 *
 * @return Total number of visited nodes since last reset (or since program start
 *         if you do not reset the counter).
 */
long Engine::get_nodes_visited() { return nodesVisited; }


/**
 * @brief Converts side sign (+1/-1) to 0/1 color encoding.
 *
 * Many parts of this project encode colors as:
 * - 0 = White
 * - 1 = Black
 *
 * While the negamax layer uses a sign:
 * - +1 = White perspective
 * - -1 = Black perspective
 *
 * @param sign +1 for White, -1 for Black (any value > 0 treated as White).
 * @return 0 for White, 1 for Black.
 */
inline int Engine::to01(int sign) { return sign > 0 ? 0 : 1; }
/**
 * @brief Converts 0/1 color encoding back to a sign (+1/-1).
 *
 * @param color01 0 = White, 1 = Black.
 * @return +1 for White, -1 for Black.
 */
inline int Engine::toSign(int color01) { return color01 == 0 ? +1 : -1; }

/**
 * @brief Checks whether (row, col) is inside the 8x8 chessboard.
 *
 * @param r Row index.
 * @param c Column index.
 * @return true if both r and c are in range [0..7], otherwise false.
 */
inline bool Engine::isValidPos(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }
/**
 * @brief Returns piece-square-table (PST) value adjusted for the piece color.
 *
 * The PST tables are typically authored from White's perspective. For White pieces,
 * rows are mirrored so that "advanced" squares get the intended bonus.
 *
 * @param table 8x8 PST table (authored from White view).
 * @param row Board row [0..7] in your internal representation.
 * @param col Board col [0..7].
 * @param color 0 = White, 1 = Black.
 * @return PST bonus (or penalty) for that square.
 */
int Engine::getPstValue(const int table[8][8], int row, int col, int color)
{
    if (color == 0) return table[7 - row][col];
    return table[row][col];
}
/**
 * @brief Reverts a previously applied move (make/undo search loop).
 *
 * Restores:
 * - board.squares[] pointers (moved piece back to from-square, captured piece back to to-square),
 * - piece positions (Piece::setPosition),
 * - promotion (restores pawn symbol if promotion was applied),
 * - incremental Zobrist hash updates (reverses exactly the operations in applyMove()).
 *
 * @param board Board state to mutate.
 * @param move The move that had been applied.
 * @param undo Snapshot produced by applyMove() containing all data required to revert.
 *
 */
void Engine::undoMove(Board& board, const Move& move, const Undo& undo)
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
/**
 * @brief Applies a move on the board (make/undo search loop).
 *
 * Performs a lightweight "make move" used by the engine:
 * - Updates board.squares[] (from-square becomes empty; to-square becomes moved piece),
 * - Stores enough information into @p undo to revert later (captured piece, moved piece, promotion),
 * - Updates incremental Zobrist key:
 *   - XOR out moved piece from from-square,
 *   - XOR out captured piece from to-square (if any),
 *   - XOR in moved/promoted piece on to-square,
 *   - XOR side-to-move key.
 * - Handles promotion by changing the moved piece symbol to @p move.promotion.
 *
 * @param board Board state to mutate.
 * @param move Move to apply.
 * @param undo Output snapshot used later by undoMove().
 *
 * @note This function assumes Piece pointers live outside and are not owned here.
 */
void Engine::applyMove(Board& board, const Move& move, Undo& undo)
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
/**
 * @brief Checks whether a square is attacked by a given side (0/1 color).
 *
 * This is used mainly for:
 * - detecting check (king attacked),
 * - filtering pseudo-legal moves into legal moves (moves that leave own king in check are illegal).
 *
 * Attack detection includes:
 * - pawn attacks (diagonals),
 * - knight jumps,
 * - rook/queen orthogonal rays,
 * - bishop/queen diagonal rays,
 * - king adjacency.
 *
 * @param board Board state to inspect.
 * @param pos Target square.
 * @param attackerColor 0 = White attacks, 1 = Black attacks.
 * @return true if the target square is attacked by attackerColor, otherwise false.
 *
 */
bool Engine::isSquareAttacked(Board& board, Position pos, int attackerColor)
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
/**
 * @brief Determines whether the given side's king is currently in check.
 *
 * Locates the king of @p color on the board, then calls isSquareAttacked() for the
 * king square using the opposite color.
 *
 * @param board Board state to inspect.
 * @param color 0 = White king, 1 = Black king.
 * @return true if that king square is attacked, otherwise false.
 *
 */
bool Engine::isInCheck(Board& board, int color)
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

/**
 * @brief Static evaluation function (material + piece-square tables).
 *
 * Computes:
 * - material score based on piece values,
 * - positional score using PST tables,
 * and returns the result from the perspective of @p color (sign +1/-1).
 *
 * Convention:
 * - Positive means good for the side represented by @p color.
 * - Negative means good for the opponent.
 *
 * @param board Board position to evaluate.
 * @param color Perspective sign (+1 = White perspective, -1 = Black perspective).
 * @return Evaluation score from the given perspective.
 *
 */
int Engine::eval(const Board& board, int color)
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

/**
 * @brief Generates all legal moves for the given side (0/1 color).
 *
 * Steps:
 * 1) Generate pseudo-legal moves (quiet moves + captures).
 * 2) For each move:
 *    - applyMove(),
 *    - test whether own king is still safe (not in check),
 *    - undoMove().
 *
 * @param board Board state (temporarily mutated by apply/undo during legality checking).
 * @param color 0 = White, 1 = Black.
 * @return Vector of legal moves.
 *
 */
std::vector<Move> Engine::legalMoves(Board& board, int color) {
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
/**
 * @brief Perform generate all captures.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Result of the operation.
 */
static std::vector<Move> generateCaptures(Board& board, int color)
{
    return generateAllCaptures(board, color);
}
/**
 * @brief Score a move for move ordering.
 * 
 * @param move Move to score.
 * @return int Score value.
 */
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
/**
 * @brief Orders moves to improve alpha-beta pruning efficiency.
 *
 * Current heuristic:
 * - Captures scored using MVV-LVA (Most Valuable Victim - Least Valuable Attacker),
 * - Promotions receive a bonus.
 *
 * @param moves Move list to reorder in-place.
 *
 */
void Engine::orderMoves(std::vector<Move>& moves)
{
    // Sort descending by score
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return scoreMove(a) > scoreMove(b);
        });
}

// game over if one king is missing
/**
 * @brief Perform game over.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */
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

/**
 * @brief Capture-only extension search to reduce the horizon effect.
 *
 * Quiescence search continues exploring forcing capture sequences at depth 0
 * instead of returning a static evaluation immediately.
 *
 * Algorithm:
 * - Compute a "stand pat" evaluation.
 * - If stand pat >= beta: fail-high cutoff.
 * - Else try all capture moves:
 *     score = -quiescence(child, -beta, -alpha, -color)
 * - Return best score within [alpha, beta].
 *
 * @param board Board state (mutated via apply/undo).
 * @param alpha Alpha bound.
 * @param beta Beta bound.
 * @param color Perspective sign (+1 = White perspective, -1 = Black perspective).
 * @return Best tactical score in the quiescence search window.
 */
int Engine::quiescence(Board& board, int alpha, int beta, int color)
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

/**
 * @brief Checks whether the current position repeats (threefold-style loop prevention).
 *
 * Uses Zobrist keys stored in board.positionHistory and compares them against
 * the current board.zobristKey.
 *
 * @param board Board state (must contain a valid zobristKey and positionHistory).
 * @return true if the current zobristKey appeared before in the history, otherwise false.
 *
 * @note This is often used to discourage draw loops or to return a draw-ish score.
 */
bool Engine::isRepetition(const Board& board) {
    // Move is a repetition if current zobristKey appeared before in positionHistory
    for (int i = board.positionHistory.size() - 1; i >= 0; --i) {
        if (board.positionHistory[i] == board.zobristKey) {
            return true;
            // Repetition found
        }
    }
    return false;
}

/**
 * @brief Negamax search with alpha-beta pruning and transposition table (TT).
 *
 * High-level flow:
 * 1) Optional repetition detection (to avoid loops).
 * 2) Transposition Table probe to reuse cached scores/bounds.
 * 3) Terminal / depth cutoff:
 *    - depth == 0 -> quiescence()
 *    - game over / no legal moves -> mate/stalemate scoring
 * 4) Generate legal moves, order them, recurse with sign flip:
 *      score = -negamax(child, depth-1, -beta, -alpha, -color)
 * 5) Update alpha and prune when alpha >= beta.
 * 6) Store result as TT_EXACT / TT_ALPHA / TT_BETA along with best move.
 *
 * @param board Board state (mutated via apply/undo during recursion).
 * @param depth Remaining depth in plies.
 * @param alpha Alpha bound (lower bound).
 * @param beta Beta bound (upper bound).
 * @param color Perspective sign (+1 = White perspective, -1 = Black perspective).
 * @return Best score from the perspective of @p color.
 *
 */
int Engine::negamax(Board& board, int depth, int alpha, int beta, int color)
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