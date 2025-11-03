#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Knight : public Piece {
private:
	int direction;

public:
	Knight(int, char, Position);
	bool canMove(Position, Board*);

};

