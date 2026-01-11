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
int negamax(Board &board, int depth, int alpha, int beta, int color);
int eval(const Board &board, int color);
long get_nodes_visited();
int quiescence(Board& board, int alpha, int beta, int color);
bool isInCheck(Board& board, int color);