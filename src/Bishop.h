#pragma once
#include <iostream>
#include "Bishop.h"
#include "Board.h"
using namespace std;

class Bishop : public Piece {
private:
	int direction;

public:
	Bishop(int, char, Position);
	bool canMove(Position pos, Board &board);

};

