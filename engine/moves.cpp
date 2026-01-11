/**
 * @file moves.cpp
 * @brief Pseudo-legal move generation (quiet moves and captures).
 *
 * This file generates pseudo-legal moves for each piece type. "Pseudo-legal" means:
 * the moves follow piece movement rules, but may still leave the moving side in check.
 *
 * @details
 * The legality filtering is performed in evalpos.cpp (legalMoves()), which applies a move and
 * rejects it if the king is left in check.
 *
 * Current limitations:
 * - No castling rights / castling moves are generated.
 * - No en-passant capture is generated.
 * - Promotions are generated as a single default choice (typically queen) unless expanded elsewhere.
 */


#include "../Board.h"
#include "moves.h"
#include <vector>
#include <cctype>
#include <algorithm>
#include "../Piece.h"
#include "../Pawn.h"
#include "../kNight.h"
#include "../Bishop.h"
#include "../Rook.h"
#include "../Queen.h"
#include "../King.h"

// Only directions for moves
static int knightMoves[][2] = { {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1} };
static int bishopDirections[][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
static int rookDirections[][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
static int queenDirections[][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
static int kingMoves[][2] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };


// Helper border check, inline makes it faster
/**
 * @brief Check whether valid pos.
 *
 * @details Returns a boolean condition derived from current state/arguments.
 * @param r Parameter.
 * @param c Parameter.
 * @return True if the condition holds; otherwise false.
 */
inline bool isValidPos(int r, int c) {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}


static std::vector<Position> generateMovesForPiece(Board &board, Position pos, Piece* piece, int color){
    std::vector<Position> moves;
    if (!piece)
        return moves;

    /**
 * @brief Perform switch.
 *
 * @details Implements the behavior implied by the function name.
 * @param symbol Parameter.
 * @return Result of the operation.
 */
    char symbol = toupper((unsigned char)piece->getSymbol());
	int r = pos.row;
	int c = pos.col;

    switch(symbol){
        case 'P': // PAWN
            if(color == 0){
                // Generate white pawn moves
                if (isValidPos(r + 1, c) && board.isEmpty({ r + 1, c })){
                    /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param knightMoves Move data/descriptor.
 * @return Result of the operation.
 */
                    moves.push_back({ r + 1, c });
                    //if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && board.isEmpty(newPos))
                    if (r == 1 && isValidPos(r + 2, c) && board.isEmpty({ r + 2, c })) {
                        moves.push_back({ r + 2, c });
                    }
                }
            }
            else { // Black
                // Ruch o 1
                if (isValidPos(r - 1, c) && board.isEmpty({ r - 1, c })) {
                    /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param knightMoves Move data/descriptor.
 * @return Result of the operation.
 */
                    moves.push_back({ r - 1, c });
					// Ruch o 2 z pozycji startowej
                    if (r == 6 && isValidPos(r - 2, c) && board.isEmpty({ r - 2, c })) {
                        moves.push_back({ r - 2, c });
                    }
                }
            }
            break;
        case 'N': // KNIGHT
            for (auto& move : knightMoves) {
                int nr = r + move[0];
                int nc = c + move[1];
                if (isValidPos(nr, nc) && board.isEmpty({ nr, nc })) {
                    /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param d Parameter.
 * @return Result of the operation.
 */
                    moves.push_back({ nr, nc });
                }
            }
            break;
        case 'B': // BISHOP
        case 'R': // ROOK
        case 'Q': // QUEEN
        {
			// Choose directions based on piece type
            /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param d Parameter.
 * @return Result of the operation.
 */
            int (*dirs)[2] = (symbol == 'B') ? bishopDirections :
                         (symbol == 'R') ? rookDirections : queenDirections;
            int numDirs = (symbol == 'Q') ? 8 : 4;

            for(int d = 0; d < numDirs; ++d){
                for(int i = 1; i < 8; ++i){
                    int nr = r + dirs[d][0] * i;
                    int nc = c + dirs[d][1] * i;
                    
					if (!isValidPos(nr, nc)) break; // Move out from board

                    if(board.isEmpty({nr, nc})){
                        /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param kingMoves Move data/descriptor.
 * @return Result of the operation.
 */
                        moves.push_back({nr, nc});
                    } else {
						// Piece in a way, stop searching this direction
                        break; 
                    }
                }
            }
            break;
        }
        case 'K': // KING
            for(auto &move : kingMoves){
                int nr = r + move[0];
                int nc = c + move[1];
                if (isValidPos(nr, nc) && board.isEmpty({nr, nc})) {
					// We have to check for checks here in a complete implementation
                    /**
 * @brief Perform generate captures for piece.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param pos Board position/index.
 * @param symbol Parameter.
 * @param color Side/color parameter.
 * @return Collection of results.
 */
                    moves.push_back({nr, nc});
                }
            }
            // TODO: Castling here to implement
            break;
    }
    return moves;
}

/**
 * @brief Perform generate captures for piece.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param pos Board position/index.
 * @param symbol Parameter.
 * @param color Side/color parameter.
 * @return Collection of results.
 */
static std::vector<Position> generateCapturesForPiece(Board &board, Position pos, char symbol, int color){
    symbol = toupper(symbol);
    std::vector<Position> captures;
    int r = pos.row;
    int c = pos.col;

    switch(symbol){
        case 'P': // PAWN CAPTURES
            if (color == 0) { // White
                int capDir[2][2] = { {1, -1}, {1, 1} };
                for (auto& d : capDir) {
                    int nr = r + d[0];
                    int nc = c + d[1];
                    if (isValidPos(nr, nc) && !board.isEmpty({ nr, nc })) {
                        if (board.getPieceAt({ nr, nc })->getColor() != color)
                            /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param capDir Parameter.
 * @return Result of the operation.
 */
                            captures.push_back({ nr, nc });
                    }
					// TODO: En Passant need to be handled here, but requires more game state info
                }
            }
        else { // Black
            int capDir[2][2] = { {-1, -1}, {-1, 1} };
            for (auto& d : capDir) {
                int nr = r + d[0];
                int nc = c + d[1];
                if (isValidPos(nr, nc) && !board.isEmpty({ nr, nc })) {
                    if (board.getPieceAt({ nr, nc })->getColor() != color)
                        /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param knightMoves Move data/descriptor.
 * @return Result of the operation.
 */
                        captures.push_back({ nr, nc });
                }
            }
        }
        break;
            break;
        case 'N': // KNIGHT
            for (auto& move : knightMoves) {
                int nr = r + move[0];
                int nc = c + move[1];
                if (isValidPos(nr, nc) && !board.isEmpty({ nr, nc })) {
                    if (board.getPieceAt({ nr, nc })->getColor() != color)
                        /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param d Parameter.
 * @return Result of the operation.
 */
                        captures.push_back({ nr, nc });
                }
            }
            break;
        case 'B':
        case 'R':
        case 'Q':
        {
            /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param d Parameter.
 * @return Result of the operation.
 */
            int (*dirs)[2] = (symbol == 'B') ? bishopDirections :
                (symbol == 'R') ? rookDirections : queenDirections;
            int numDirs = (symbol == 'Q') ? 8 : 4;

            for (int d = 0; d < numDirs; ++d) {
                for (int i = 1; i < 8; ++i) {
                    int nr = r + dirs[d][0] * i;
                    int nc = c + dirs[d][1] * i;

                    if (!isValidPos(nr, nc)) break;

                    if (board.isEmpty({ nr, nc })) {
						continue; // Empty square, go further
                    }
                    else {
                        // Piece here
                        if (board.getPieceAt({ nr, nc })->getColor() != color) {
                            /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param kingMoves Move data/descriptor.
 * @return Result of the operation.
 */
                            captures.push_back({ nr, nc }); //Enemy - capture
                        }
                        break; // Blocked, stop
                    }
                }
            }
            break;
        }
        case 'K':
            for (auto& move : kingMoves) {
                int nr = r + move[0];
                int nc = c + move[1];
                if (isValidPos(nr, nc) && !board.isEmpty({ nr, nc })) {
                    if (board.getPieceAt({ nr, nc })->getColor() != color)
                        /**
 * @brief Perform generate quiet moves.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Collection of results.
 */
                        captures.push_back({ nr, nc });
                }
            }
            break;
    }
    return captures;
}


/**
 * @brief Perform generate quiet moves.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Collection of results.
 */

std::vector<Move> generateQuietMoves(Board& board, int color) {
    std::vector<Move> allMoves;
	// Preallocate memory to save time
    allMoves.reserve(60);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            auto* piece = board.getPieceAt({ row, col });
            if (piece != nullptr && piece->getColor() == color) {
				// Only quiet moves
                /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param pieceMoves Move data/descriptor.
 * @return Result of the operation.
 */
                auto pieceMoves = generateMovesForPiece(board, { row, col }, piece, color);

                for (auto& pos : pieceMoves) {
                    Move mv;
                    mv.from = { row, col };
                    mv.to = pos;
                    mv.pieceMoved = piece;
                    mv.pieceCaptured = nullptr; // No capture
					mv.promotion = 0; // No promotion
                    if (toupper(piece->getSymbol()) == 'P' && (pos.row == 0 || pos.row == 7)) {
                        // Options for later
                        //char promos[] = { 'Q', 'N', 'R', 'B' };
                        //for (char p : promos) {
                            mv.promotion = 'Q';
                            /**
 * @brief Perform generate all captures.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Collection of results.
 */
                            allMoves.push_back(mv);
                        //}
                        continue;
                    }
                    allMoves.push_back(mv);
                }
            }
        }
    }
    //std::cout << "Generated " << allMoves.size() << " moves for color " << color << std::endl;
    //for (int i = 0; i < allMoves.size(); i++) {
    //    std::cout << "Move " << i << ": from (" << allMoves[i].from.row << "," << allMoves[i].from.col << ") to (" << allMoves[i].to.row << "," << allMoves[i].to.col << ")\n";
    //}
    return allMoves;
}

/**
 * @brief Perform generate all captures.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Collection of results.
 */

std::vector<Move> generateAllCaptures(Board& board, int color) {
    std::vector<Move> allCaptures;
    allCaptures.reserve(20);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getPieceAt({ row, col });
            if (piece != nullptr && piece->getColor() == color) {
                /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param pieceCaptures Parameter.
 * @return Result of the operation.
 */
                auto pieceCaptures = generateCapturesForPiece(board, { row, col }, piece->getSymbol(), color);
                for (auto& pos : pieceCaptures) {
                    Move mv;
                    mv.from = { row, col };
                    mv.to = pos;
                    mv.pieceMoved = piece;
                    mv.pieceCaptured = board.getPieceAt(pos);
                    mv.promotion = 0; // No promotion
                    if (toupper(piece->getSymbol()) == 'P' && (pos.row == 0 || pos.row == 7)) {
                        // Options for later
                        //char promos[] = { 'Q', 'N', 'R', 'B' };
                        //for (char p : promos) {
                        mv.promotion = 'Q';
                        /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param i Parameter.
 * @return Result of the operation.
 */
                        allCaptures.push_back(mv);
                        //}
                        continue;
                    }
                    allCaptures.push_back(mv);
                }
            }
        }
    }
  /**
 * @brief Perform for.
 *
 * @details Implements the behavior implied by the function name.
 * @param i Parameter.
 * @return Result of the operation.
 */
  /*  std::cout << "Generated " << allCaptures.size() << " captures for color " << color << std::endl;
    for (int i = 0; i < allCaptures.size(); i++) {
        std::cout << "Capture " << i << ": from (" << allCaptures[i].from.row << "," << allCaptures[i].from.col << ") to (" << allCaptures[i].to.row << "," << allCaptures[i].to.col << ")\n";
    }*/
    return allCaptures;
}
