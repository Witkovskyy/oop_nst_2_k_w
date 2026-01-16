/**
 * @file engine.h
 * @brief Public declarations for the chess engine (negamax / eval / move generation helpers).
 * @version 0.1
 * @date 2026-01-12
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <vector>
#include "../Board.h"
#include "moves.h"

class Engine {
public:
	/**
 * @brief Structure to hold undo information.
 *
 */
	struct Undo
	{
		Position from;
		Position to;
		Piece* pieceMoved;
		Piece* pieceCaptured;
		char promotion;
	};
	// ================================
	// Small helpers used across engine
	// ================================

	/**
	 * @brief Convert sign (+1 / -1) to 0/1 color encoding used by board/pieces.
	 * @param sign +1 for white, -1 for black
	 * @return 0 for white, 1 for black
	 */
	static inline int to01(int sign);

	/**
	 * @brief Convert 0/1 color encoding back to sign (+1 / -1).
	 * @param color01 0 for white, 1 for black
	 * @return +1 for white, -1 for black
	 */
	static inline int toSign(int color01);

	/**
	 * @brief Validate board coordinates.
	 * @param r Row index [0..7]
	 * @param c Col index [0..7]
	 * @return true if in bounds
	 */
	static inline bool isValidPos(int r, int c);

	/**
	 * @brief Get the PST value for a piece depending on color orientation.
	 * @param table 8x8 PST table
	 * @param row board row
	 * @param col board col
	 * @param color 0=white, 1=black
	 * @return pst score
	 */
	int getPstValue(const int table[8][8], int row, int col, int color);

	// ================================
	// Search stats
	// ================================

	/**
	 * @brief Return number of nodes visited by negamax.
	 * @return nodes visited
	 */
	long get_nodes_visited();

	// ================================
	// Core engine API
	// ================================

	/**
	 * @brief Static evaluation (material + PST) from the given perspective.
	 * @param board Board state
	 * @param color Perspective sign (+1 white, -1 black)
	 * @return evaluation score
	 */
	int eval(const Board& board, int color);

	/**
	 * @brief Quiescence search (captures only).
	 * @param board Board state (mutated via apply/undo internally)
	 * @param alpha alpha bound
	 * @param beta beta bound
	 * @param color Perspective sign (+1 white, -1 black)
	 * @return quiescence score
	 */
	int quiescence(Board& board, int alpha, int beta, int color);

	/**
	 * @brief Negamax search with alpha-beta pruning and transposition table.
	 * @param board Board state (mutated via apply/undo internally)
	 * @param depth depth in plies
	 * @param alpha alpha bound
	 * @param beta beta bound
	 * @param color Perspective sign (+1 white, -1 black)
	 * @return best score
	 */
	int negamax(Board& board, int depth, int alpha, int beta, int color);

	/**
	 * @brief Check if the given side is in check.
	 * @param board Board state
	 * @param color01 0=white, 1=black
	 * @return true if king of color01 is attacked
	 */
	bool isInCheck(Board& board, int color01);

	/**
	 * @brief Check if a square is attacked by the attackerColor side.
	 * @param board Board state
	 * @param pos Target square
	 * @param attackerColor 0=white, 1=black
	 * @return true if attacked
	 */
	bool isSquareAttacked(Board& board, Position pos, int attackerColor);

	/**
	 * @brief Check repetition based on board.positionHistory and zobristKey.
	 * @param board Board state
	 * @return true if current position repeated
	 */
	bool isRepetition(const Board& board);

	// ================================
	// Move helpers used by UI/engine
	// ================================

	/**
	 * @brief Generate legal moves for a color (0=white, 1=black).
	 * @param board Board state (will be temporarily mutated during legality checks)
	 * @param color01 0=white, 1=black
	 * @return vector of legal moves
	 *
	 */
	std::vector<Move> legalMoves(Board& board, int color01);

	/**
	 * @brief Order moves in-place for better alpha-beta performance.
	 * @param moves move list to sort
	 *
	 */
	void orderMoves(std::vector<Move>& moves);
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
	void undoMove(Board& board, const Move& move, const Undo& undo);
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
	void applyMove(Board& board, const Move& move, Undo& undo);
};