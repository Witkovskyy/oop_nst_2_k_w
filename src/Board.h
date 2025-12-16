#pragma once
#include <iostream>
#include "Piece.h"
#include <vector>

const int sizeboard = 8;

class Board {
private:
    Piece* squares[sizeboard][sizeboard];

    // Funkcja pomocnicza (nieużywana w main, więc możesz ją usunąć lub zostawić)
    Piece* wyswietl();

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
   // std::vector<Position> getLegalMoves(Position startPos, int color);

};
