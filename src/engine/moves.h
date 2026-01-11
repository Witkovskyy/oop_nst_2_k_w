#pragma once
#include "../Board.h"
#include "../Piece.h"
#include <vector>
struct Move {
    Position from;
    Position to;
    Piece* pieceMoved;
    Piece* pieceCaptured;
	char promotion = 0; // 0 = no promotion, otherwise char code of promoted piece
};

/**
 * @brief Perform generate quiet moves.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Collection of results.
 */
std::vector<Move> generateQuietMoves(Board& board, int color);
/**
 * @brief Perform generate capture moves.
 *
 * @details Implements the behavior implied by the function name.
 * @param board Board state to operate on.
 * @param color Side/color parameter.
 * @return Collection of results.
 */
std::vector<Move> generateAllCaptures(Board& board, int color);
