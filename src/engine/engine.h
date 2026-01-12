/**
 * @file engine.h
 * @brief File declaration for engine.
 * @version 0.1
 * @date 2026-01-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include "../Board.h"

// Engine API
/**
 * @brief Perform see.
 *
 * @details Implements the behavior implied by the function name.
 * @param start Parameter.
 * @param target Parameter.
 * @param sideToMove Move data/descriptor.
 * @return Integer result.
 */
int see(Board &start, Position target, int sideToMove);
/**
 * @brief Perform negamax.
 * 
 * @param board 
 * @param depth 
 * @param alpha 
 * @param beta 
 * @param color 
 * @return int 
 */
int negamax(Board &board, int depth, int alpha, int beta, int color);
/**
 * @brief Perform eval.
 * 
 * @param board 
 * @param color 
 * @return int 
 */
int eval(const Board &board, int color);
/**
 * @brief Get the nodes visited object
 * 
 * @return long 
 */
long get_nodes_visited();
/**
 * @brief Perform quiescence search.
 * 
 * @param board 
 * @param alpha 
 * @param beta 
 * @param color 
 * @return int 
 */
int quiescence(Board& board, int alpha, int beta, int color);
/**
 * @brief Perform isInCheck.
 * 
 * @param board 
 * @param color 
 * @return true 
 * @return false 
 */
bool isInCheck(Board& board, int color);