#pragma once
#include <iostream>
#include <vector>
#include "Piece.h"
#include <vector>

const int sizeboard = 8;

class Board {
public:
    Piece* squares[sizeboard][sizeboard];
    Piece* Display();
public:
    Board();
    //Position findKingPos(int color);
   // bool isKingInCheck(int color);
    //bool isCheckMate(int color);

    // --- NOWE METODY DO SZACHA I MATA ---
    Position findKing(int color);                   // Znajdź pozycję króla
    bool isSquareAttacked(Position pos, int enemyColor); // Czy pole jest atakowane?
    bool isKingInCheck(int color);                  // Czy król jest szachowany?
    bool isMoveSafe(Position start, Position end);  // Czy ruch nie powoduje szacha? (Obsługa ZWIĄZANIA)
    bool isCheckMate(int color);                    // Czy to MAT?

    void placePiece(Piece* piece);
    bool isEmpty(Position pos);
    bool movePiece(Position oldpos, Position newpos, Piece* piece);
    void DisplayBoard();
    Piece* getPieceAt(Position pos);
	bool validateMove(Position from, Position to, Piece* piece);
	void promotePawn(Board &board, Position pos, char newSymbol, int color);
	unsigned long long zobristKey = 0;
	void computeZobristHash();
	std::vector<unsigned long long> positionHistory;
};
