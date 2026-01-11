/**
 * @brief Perform piece val from symbol.
 *
 * @details Implements the behavior implied by the function name.
 * @param symbol Parameter.
 * @return Integer result.
 */


#pragma once
#include <limits>

/**
 * @brief Perform piece val from symbol.
 *
 * @details Implements the behavior implied by the function name.
 * @param symbol Parameter.
 * @return Integer result.
 */
static inline int pieceValFromSymbol(char symbol)
{
    symbol = toupper(symbol);
    switch (symbol)
    {
    case 'P': // Pawn
        return 100;
    case 'N': // Knight
        return 320;
    case 'B': // Bishop
        return 350;
    case 'R': // Rook
        return 500;
    case 'Q': // Queen
        return 1500;
    case 'K': // King
        return 20000;
    default:
        return 0; // Empty or unknown
    }
}

/**
 * @brief Perform `max`.
 *
 * @details Documentation for `max`.
 * @return Result of the operation.
 */
static const int INF = std::numeric_limits<int>::max();

//static const int depth = 6; // Fixed search depth, might change later

inline static const int pawnPST[8][8] = {
	{ 0,  0,  0,  0,  0,  0,  0,  0}, // Promotion Line 8
    {50, 50, 50, 50, 50, 50, 50, 50}, // Linia 7
    {10, 10, 20, 30, 30, 20, 10, 10}, 
    { 5,  5, 10, 25, 25, 10,  5,  5}, // Strong center +25
    { 0,  0,  0, 20, 20,  0,  0,  0}, // Center +20
    { 5, -5,-10, 10, 10,-10, -5,  5}, 
    { 5, 10, 10,-20,-20, 10, 10,  5}, 
    { 0,  0,  0,  0,  0,  0,  0,  0}  // Line 1
};
//Radioactive pawns
//static const int pawnPST[8][8] = {
//    {0, 0, 0,    0,    0, 0, 0, 0},
//    {0, 0, 0,    0,    0, 0, 0, 0},
//    {0, 0, 0,    0,    0, 0, 0, 0},
//    {0, 0, 0, 9999, 9999, 0, 0, 0}, 
//    {0, 0, 0, 9999, 9999, 0, 0, 0},
//    {0, 0, 0,    0,    0, 0, 0, 0},
//    {0, 0, 0,    0,    0, 0, 0, 0},
//    {0, 0, 0,    0,    0, 0, 0, 0}
//};

inline static const int knightPST[8][8] =
{
	{-50,-40,-30,-30,-30,-30,-40,-50},
	{-40,-20,  0,  0,  0,  0,-20,-40},
	{-30,  0, 10, 15, 15, 10,  0,-30},
	{-30,  5, 15, 20, 20, 15,  5,-30},
	{-30,  0, 15, 20, 20, 15,  0,-30},
	{-30, 5, 10, 15, 15, 10, 5,-30},
	{-40,-20, 0, 5, 5, 0,-20,-40},
	{-50,-40,-30,-30,-30,-30,-40,-50}
};
inline static const int bishopPST[8][8] =
{
	{-20,-10,-10,-10,-10,-10,-10,-20},
	{-10,  0,  0,  0,  0,  0,  0,-10},
	{-10,  0,  5, 10, 10,  5,  0,-10},
	{-10, 5, 5, 10, 10, 5, 5,-10},
	{-10, 0,10,10,10,10,0,-10},
	{-10,10,10,10,10,10,10,-10},
	{-10,5,0,0,0,0,5,-10},
	{-20,-10,-10,-10,-10,-10,-10,-20}
};
inline static const int rookPST[8][8] = {{
	0, 0, 0, 0, 0, 0, 0, 0 },
	{ 5,10,10,10,10,10,10,5 },
	{ -5,0,0,0,0,0,0,-5 },
	{ -5,0,0,0,0,0,0,-5 },
	{ -5,0,0,0,0,0,0,-5 },
	{ -5,0,0,0,0,0,0,-5 },
	{ -5,0,0,0,0,0,0,-5 },
	{ 0, 0, 0, 5, 5, 0, 0, 0 }};
inline static const int queenPST[8][8] = {{
-20,-10,-10, -5, -5,-10,-10,-20 },
{ -10,  0,  0,  0,  0,  0,  0,-10 },
{ -10,  0,  5,  5,  5,  5,  0,-10 },
{ -5, 0, 5, 5, 5, 5, 0, -5 },
{ 0, 0, 5, 5, 5, 5, 0, -5 },
{ -10, 5, 5, 5, 5, 5, 0,-10 },
{ -10, 0, 5, 0, 0, 0, 0,-10 },
{ -20,-10,-10, -5, -5,-10,-10,-20 }};
inline static const int kingPST[8][8] = { {
	-30,-40,-40,-50,-50,-40,-40,-30 },
{ -30,-40,-40,-50,-50,-40,-40,-30 },
{ -30,-40,-40,-50,-50,-40,-40,-30 },
{ -30,-40,-40,-50,-50,-40,-40,-30 },
{ -20,-30,-30,-40,-40,-30,-30,-20 },
{ -10,-20,-20,-20,-20,-20,-20,-10 },
{ 20, 20,  0,  0,  0,  0, 20, 20 },
{ 20, 30, 10,  0,  0, 10, 30, 20 } };