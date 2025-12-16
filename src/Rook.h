#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Rook : public Piece {
private:
	int direction;

public:
	Rook(int, char, Position);
	bool canMove(Position pos, Board &board);

};
