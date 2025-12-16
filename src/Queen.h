#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Queen : public Piece {
private:
	int direction;

public:
	Queen(int, char, Position);
	bool canMove(Position pos, Board &board);

};

