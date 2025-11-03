#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class King : public Piece {
private:
	int direction;

public:
	King(int, char, Position);
	bool canMove(Position, Board*);

};

