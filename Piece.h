/**
 * @brief Piece method: piece.
 *
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */


﻿#pragma once
#include <iostream>
using namespace std;

class Board;

struct Position {
    int row;
    int col;
};

class Board;

class Piece
{
    public:
        int color;
        char symbol;
        Position current_position;
        int value;
    public:
        /**
 * @brief Piece method: piece.
 *
 * @details Applies to a single chess piece instance.
 * @param Position Board position/index.
 * @return Result of the operation.
 */
        Piece(int,char,Position);
        int getColor();
        char getSymbol();
        Position getPosition();
        void setColor(int c);
        void setSymbol(char s);
        /**
 * @brief Set position.
 *
 * @details Applies to a single chess piece instance.
 * @param setPos Board position/index.
 */
        void setPosition(Position setPos);
        virtual bool canMove(Position newPos, Board &board);
};