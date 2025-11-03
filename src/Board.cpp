#include "Board.h"
#include "Piece.h"
#include "Pawn.h"
#include "kNight.h"
#include "Queen.h"
#include "Bishop.h"
#include "Rook.h"
#include "King.h"
#include <iostream>
using namespace std;

Piece* Board::wyswietl() {
    return squares[1][1];
}

Board::Board() {
    for (int i = 0; i < sizeboard; i++) {
        for (int j = 0; j < sizeboard; j++) {
            squares[i][j] = nullptr;
        }
    }
}

void Board::placePiece(Piece* piece) {
    Position pos = piece->getPosition();
    squares[pos.row][pos.col] = piece;
}

bool Board::isEmpty(Position pos) {
    return squares[pos.row][pos.col] == nullptr;
}

bool Board::movePiece(Position oldpos, Position newpos, Piece* piece) {
    
    if (piece->getSymbol() == 'P') {
        Pawn* pawn = dynamic_cast<Pawn*>(piece);
        if (pawn!=nullptr && pawn->canMove(newpos,this)) {
            squares[oldpos.row][oldpos.col] = nullptr;
            squares[newpos.row][newpos.col] = piece;
            pawn->setPosition(newpos);
            return true;
        }
        else {
            //cout << "Ruch nieprawidllowy!\n"; - wypiszemy w main.cpp
            return false;
        }
        

    }

    if (piece->getSymbol() == 'N') {
        Knight* knight = dynamic_cast<Knight*>(piece);
        if (knight != nullptr && knight->canMove(newpos, this)) {
            squares[oldpos.row][oldpos.col] = nullptr;
            squares[newpos.row][newpos.col] = piece;
            knight->setPosition(newpos);
            return true;
        }
        else {
            //cout << "Ruch nieprawidllowy!\n"; - wypiszemy w main.cpp
            return false;
        }
    }

    if (piece->getSymbol() == 'Q') {
        Queen* queen = dynamic_cast<Queen*>(piece);
        if (queen != nullptr && queen->canMove(newpos, this)) {
            squares[oldpos.row][oldpos.col] = nullptr;
            squares[newpos.row][newpos.col] = piece;
            queen->setPosition(newpos);
            return true;
        }
        else {
            //cout << "Ruch nieprawidllowy!\n"; - wypiszemy w main.cpp
            return false;
        }
    }
    if (piece->getSymbol() == 'B') {
        Bishop* bishop = dynamic_cast<Bishop*>(piece);
        if (bishop != nullptr && bishop->canMove(newpos, this)) {
            squares[oldpos.row][oldpos.col] = nullptr;
            squares[newpos.row][newpos.col] = piece;
            bishop->setPosition(newpos);
            return true;
        }
        else {
            //cout << "Ruch nieprawidllowy!\n"; - wypiszemy w main.cpp
            return false;
        }
    }

    if (piece->getSymbol() == 'R') {
        Rook* rook = dynamic_cast<Rook*>(piece);
        if (rook != nullptr && rook->canMove(newpos, this)) {
            squares[oldpos.row][oldpos.col] = nullptr;
            squares[newpos.row][newpos.col] = piece;
            rook->setPosition(newpos);
            return true;
        }
        else {
            //cout << "Ruch nieprawidllowy!\n"; - wypiszemy w main.cpp
            return false;
        }
    }

    if (piece->getSymbol() == 'K') {
        King* king = dynamic_cast<King*>(piece);
        if (king != nullptr && king->canMove(newpos, this)) {
            squares[oldpos.row][oldpos.col] = nullptr;
            squares[newpos.row][newpos.col] = piece;
            king->setPosition(newpos);
            return true;
        }
        else {
            //cout << "Ruch nieprawidllowy!\n"; - wypiszemy w main.cpp
            return false;
        }
    }
    return false;
}

void Board::DisplayBoard() {
    cout << "  a b c d e f g h \n";
    for (int row = sizeboard - 1; row >= 0; row--) {
        cout << row + 1 << " ";
        for (int col = 0; col < sizeboard; col++) {
            if (squares[row][col] != nullptr) {
                cout << squares[row][col]->getSymbol() << ' ';
            }
            else {
                cout << ". ";
            }
        }
        cout << endl;
    }
}

Piece* Board::getPieceAt(Position pos) {
    if (pos.row < 0 || pos.row >= sizeboard || pos.col < 0 || pos.col >= sizeboard)
        return nullptr; // poza planszą
    return squares[pos.row][pos.col];
}

