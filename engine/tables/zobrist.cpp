/**
 * @file zobrist.cpp
 * @brief Zobrist hashing initialization.
 *
 * This module creates the random key tables used by the engine. The exact randomness source and
 * seeding determine whether hashes are stable across runs.
 */


#include "zobrist.h"

// Define the Zobrist keys
unsigned long long pieceKeys[12][64];
unsigned long long sideKey;