#pragma once

enum class Color
{
    WHITE,
    BLACK,
    NONE
};
enum class PieceType
{
    EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};
struct Piece {
    Color color{Color::NONE};
    PieceType type{PieceType::EMPTY};
};