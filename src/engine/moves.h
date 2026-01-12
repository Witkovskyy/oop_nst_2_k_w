#pragma once
#include "../Board.h"
#include "../Piece.h"
#include <vector>
/**
 * @brief Move structure.
 *
 * @details Represents a chess move with relevant details.
 * @args from Starting position of the move.
 * @args to Ending position of the move.
 * @args pieceMoved Pointer to the piece being moved.
 * @args pieceCaptured Pointer to the piece being captured (if any).
 * @args promotion Character representing the promotion piece (if any).
 */
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
