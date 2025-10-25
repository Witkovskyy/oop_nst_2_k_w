#include <iostream>
using namespace std;
struct Position{
    int row;
    int col;
};

class Piece
{
    private:
        int color;
        char symbol;
        Position current_position;
    public:
        Piece(int,char,Position);
        int getColor();
        char getSymbol();
        Position getPosition();
        void setColor(int c);
        void setSymbol(char s);
        void setPosition(Position setPos);

};