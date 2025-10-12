#pragma once
#include <optional>

namespace chess
{
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
    struct Piece
    {
    public:
        Color color;
        PieceType type;
    };
    struct Square
    {
        int file;
        int rank;
        bool isValid() const
        {
            return file >= 0 && file < 8 && rank >= 0 && rank < 8;
        }
    };
    struct Board
    {
        Board();
        Piece cells[8][8];
        Color sideToMove;
        bool castleWK, castleWQ, castleBK, castleBQ;
        std::optional<Square> enPassant;
        int halfmoveClock;
        int fullmoveNumber;
    };
    const Piece EMPTY_PIECE = {Color::NONE, PieceType::EMPTY};
    struct Cell
    {
        PieceType piece{PieceType::EMPTY};
        Color color{Color::WHITE};
    };
}
