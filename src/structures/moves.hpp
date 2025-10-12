#pragma once
#include "board.hpp"

namespace chess
{
    enum class MoveFlag
    {
        Quiet,
        Capture,
        CastleKing,
        CastleQueen,
        EnPassant,
        Promotion
    };
    enum class Promo
    {
        None,
        Knight,
        Bishop,
        Rook,
        Queen
    };
    struct Move
    {
        Square from, to;
        Piece piece_moved;
        Piece captured = EMPTY_PIECE;
        Promo promotion{Promo::None};
        MoveFlag flag{MoveFlag::Quiet};
    };
    static const int KING_MOVE_DIRECTIONS[8][2] = {
        {1, 0},   {1, 1},   {0, 1},   {-1, 1},
        {-1, 0},  {-1, -1}, {0, -1},  {1, -1}};
    static const int KNIGHT_MOVE_DIRECTIONS[8][2] = {
        {2, 1},   {1, 2},   {-1, 2},  {-2, 1},
        {-2, -1}, {-1, -2}, {1, -2},  {2, -1}};
    static const int BISHOP_MOVE_DIRECTIONS[4][2] = {
        {1, 1},   {-1, 1},  {-1, -1}, {1, -1}};
    static const int ROOK_MOVE_DIRECTIONS[4][2] = {
        {1, 0},   {0, 1},   {-1, 0},  {0, -1}};
    static const int QUEEN_MOVE_DIRECTIONS[8][2] = {
        {1, 0},   {1, 1},   {0, 1},   {-1, 1},
        {-1, 0},  {-1, -1}, {0, -1},  {1, -1}};
    static const int BLACK_PAWN_MOVE_DIRECTIONS[2][2] = {
        {0, -1},  {0, -2}};
    static const int WHITE_PAWN_MOVE_DIRECTIONS[2][2] = {
        {0, 1},   {0, 2}};
    static const int BLACK_PAWN_CAPTURE_DIRECTIONS[2][2] = {
        {1, -1},  {-1, -1}};
    static const int WHITE_PAWN_CAPTURE_DIRECTIONS[2][2] = {
        {1, 1},   {-1, 1}};
}