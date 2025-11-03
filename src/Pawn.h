#pragma once
#include <iostream>
#include "Piece.h"
#include "Board.h"
using namespace std;

class Pawn : public Piece {
private:
	int direction;

public:
	Pawn(int,char, Position);
	bool canMove(Position, Board*);

};



