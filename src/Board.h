#pragma once
#include "Piece.h"
#include <iostream>

const int sizeboard = 8;

class Board
{
public:
    Piece *squares[sizeboard][sizeboard];

    Piece *getPieceAt(Position pos)
    {
        return squares[pos.row][pos.col];
    }

public:
    Board()
    {
        for (int i = 0; i < sizeboard; i++)
        {
            for (int j = 0; j < sizeboard; j++)
            {
                squares[i][j] = nullptr;
            }
        }
    }
    void placePiece(Piece *piece)
    {
        Position pos = piece->getPosition();
        squares[pos.row][pos.col] = piece;
    }
    bool isEmpty(Position pos)
    {
        return squares[pos.row][pos.col] == nullptr;
    }
    bool validateMove(Position oldpos, Position newpos, Piece *piece)
    {
        // Old position validation
        if (oldpos.row < 0 || oldpos.row >= sizeboard || oldpos.col < 0 || oldpos.col >= sizeboard)
            return false;
        // New position validation
        if (newpos.row < 0 || newpos.row >= sizeboard || newpos.col < 0 || newpos.col >= sizeboard)
            return false;

        // Additional rules can be added here
        return true;
    }

    void movePiece(Position oldpos, Position newpos, Piece *piece)
    {
        squares[oldpos.row][oldpos.col] = nullptr;
        squares[newpos.row][newpos.col] = piece;
    }

    void DisplayBoard()
    {
        std::cout << "  a b c d e f g h \n";
        for (int row = sizeboard - 1; row >= 0; row--)
        {
            std::cout << row + 1 << " ";
            for (int col = 0; col < sizeboard; col++)
            {
                if (squares[row][col] != nullptr)
                {
                    std::cout << squares[row][col]->getSymbol() << ' ';
                }
                else
                {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }
    }
};
