/**
 * @file zobrist.cpp
 * @brief Zobrist hashing initialization.
 *
 * This module creates the random key tables used by the engine. The exact randomness source and
 * seeding determine whether hashes are stable across runs.
 */


#include "zobrist.h"

// Define the Zobrist keys

/*
 * @brief Zobrist hash keys for pieces.
 * 
 * pieceKeys: A 2D array where the first dimension represents the piece type (12 types for both colors)
 *             and the second dimension represents the board square (64 squares).
 * 
 */
unsigned long long pieceKeys[12][64];
/*
 * @brief Zobrist hash key for the side to move.
 * 
 * sideKey: A single key representing the side to move (white or black).
 * 
 */
unsigned long long sideKey;