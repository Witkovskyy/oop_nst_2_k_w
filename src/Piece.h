#pragma once
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
        Piece(int,char,Position);
        int getColor();
        char getSymbol();
        Position getPosition();
        void setColor(int c);
        void setSymbol(char s);
        void setPosition(Position setPos);
        virtual bool canMove(Position newPos, Board& board);
};