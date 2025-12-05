#pragma once
#include "../Board.h"

// Engine API
int see(Board &start, Position target, int sideToMove);
int negamax(Board &board, int depth, int alpha, int beta, int color);
int eval(const Board &board, int color);
long get_nodes_visited();
