/**
 * @brief Perform transposition table.
 *
 * @details Implements the behavior implied by the function name.
 * @param sizeInMB Parameter.
 * @return Result of the operation.
 */


#pragma once
#include <vector>
#include "../moves.h"

enum TTFlag {
    TT_EXACT,   // Exact
    TT_ALPHA,   // Upper Bound 
    TT_BETA     // Lower Bound 
};

struct TTEntry {
    unsigned long long key; // Collision
    int score;
    int depth;
    TTFlag flag;
    Move bestMove;         
};

class TranspositionTable {
public:
    std::vector<TTEntry> table;
    int size;

    /**
 * @brief Perform transposition table.
 *
 * @details Implements the behavior implied by the function name.
 * @param sizeInMB Parameter.
 * @return Result of the operation.
 */
    TranspositionTable(int sizeInMB) {
		// How many entries fit in the given size
        int entrySize = sizeof(TTEntry);
        int numEntries = (sizeInMB * 1024 * 1024) / entrySize;
        table.resize(numEntries);
        size = numEntries;
    }

    /**
 * @brief Perform clear.
 *
 * @details Implements the behavior implied by the function name.
 */
    void clear() {
        std::fill(table.begin(), table.end(), TTEntry{ 0, 0, 0, TT_EXACT, Move() });
    }

    // Save position
    /**
 * @brief Perform store.
 *
 * @details Implements the behavior implied by the function name.
 * @param key Parameter.
 * @param score Parameter.
 * @param depth Search depth in plies.
 * @param flag Parameter.
 * @param bestMove Move data/descriptor.
 */
    void store(unsigned long long key, int score, int depth, TTFlag flag, Move bestMove) {
        int index = key % size;

		// Always preserve the entry with the greatest depth
		// Always overwrite for simplicity
        TTEntry& entry = table[index];

        if (entry.key == key) {
            if (depth >= entry.depth) {
                entry.score = score;
                entry.depth = depth;
                entry.flag = flag;
                entry.bestMove = bestMove;
            }
        }
        else {
            // Always overwrite
            entry.key = key;
            entry.score = score;
            entry.depth = depth;
            entry.flag = flag;
            entry.bestMove = bestMove;
        }
    }

    // Read position
    /**
 * @brief Perform probe.
 *
 * @details Implements the behavior implied by the function name.
 * @param key Parameter.
 * @param depth Search depth in plies.
 * @param alpha Alpha-beta bound.
 * @param beta Alpha-beta bound.
 * @param outScore Parameter.
 * @param outMove Move data/descriptor.
 * @return True if the condition holds; otherwise false.
 */
    bool probe(unsigned long long key, int depth, int alpha, int beta, int& outScore, Move& outMove) {
        if (key == 0) return false;
        int index = key % size;
        TTEntry& entry = table[index];
        if (entry.key == 0) return false;

        if (entry.key == key) { // Hit
			outMove = entry.bestMove; // Always return best move

            if (entry.depth >= depth) { // Only if reached depth
                if (entry.flag == TT_EXACT) {
                    outScore = entry.score;
                    return true;
                }
                if (entry.flag == TT_ALPHA && entry.score <= alpha) {
                    outScore = alpha;
                    return true;
                }
                if (entry.flag == TT_BETA && entry.score >= beta) {
                    outScore = beta;
                    return true;
                }
            }
        }
        return false;
    }
};

// 64 MB Transposition Table instance
extern TranspositionTable TT;