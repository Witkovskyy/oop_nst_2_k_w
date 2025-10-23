#include <iostream>
#include <math.h>
using namespace std;

enum class Color { WHITE, BLACK };
struct Position {
    int row;
    int col;
};

class Board; 

class Piece {
protected:
    int color;       // 0 = WHITE, 1 = BLACK
    Position pos;
    char symbol;

public:
    Piece(int c, Position p, char s) {
        color = c;
        pos = p;
        symbol = s;
    }
    // destr
    ~Piece() {}
    // ----
    int getColor() {
        return color;
    }
    Position getPosition() {
        return pos;
    }
    char getSymbol() {
        return symbol;
    }
    void setColor(int c) {
        color = c;
    }
    void setPosition(Position p) {
        pos = p;
    }
    void setSymbol(char s) {
        symbol = s;
    }
    virtual bool canMove(Position newPos, const Board& board) = 0;
};

class Pawn : public Piece {
public:
    Pawn(int c, Position p) : Piece(c, p, 'P') {}

    bool canMove(Position newPos, const Board& board) override {
        int direction = (color == 0) ? 1 : -1; 
        if (newPos.col == pos.col && newPos.row == pos.row + direction) {
            return true;
        }
        return false;
    }
};





int main() {
  
  //just a comment

}