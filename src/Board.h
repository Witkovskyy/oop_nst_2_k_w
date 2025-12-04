#pragma once
#include <iostream>
#include "Piece.h"

const int sizeboard = 8;
class Board {
public:
    Piece* squares[sizeboard][sizeboard];
    Piece* Display();
public:
    Board();
    void placePiece(Piece* piece);
    bool isEmpty(Position pos);
    bool movePiece(Position oldpos, Position newpos, Piece* piece);
    void DisplayBoard();
    Piece* getPieceAt(Position pos);
	bool validateMove(Position from, Position to, Piece* piece);
};