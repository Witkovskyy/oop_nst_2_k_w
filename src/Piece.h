#pragma once
#include <iostream>
using namespace std;
struct Position{
    int row;
    int col;
};

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

};