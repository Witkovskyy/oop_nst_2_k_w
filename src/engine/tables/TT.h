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

    TranspositionTable(int sizeInMB) {
		// How many entries fit in the given size
        int entrySize = sizeof(TTEntry);
        int numEntries = (sizeInMB * 1024 * 1024) / entrySize;
        table.resize(numEntries);
        size = numEntries;
    }

    void clear() {
        std::fill(table.begin(), table.end(), TTEntry{ 0, 0, 0, TT_EXACT, Move() });
    }

    // Save position
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