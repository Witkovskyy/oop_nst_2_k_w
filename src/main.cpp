#include <iostream>
#include "Piece.h"
#include "Board.h"
#include "engine/engine.h"
using namespace std;
int main()
{
    Board board;
    Piece *p1 = new Piece(0, 'K', {2, 1});
    board.placePiece(p1);
    Piece *p2 = new Piece(1, 'K', {7, 3});
    board.placePiece(p2);
    Piece *p3 = new Piece(0, 'Q', {3, 4});
    board.placePiece(p3);
    Piece *p5 = new Piece(0, 'R', {5, 5});
    board.placePiece(p5);


    cout << "Initial board:\n";
    board.DisplayBoard();

    cout << "eval = " << eval(board) << "\n";
    Position target{3 ,4};
    cout << "see - white = " << see(board, target, 0) << "\n";
    cout << "negamax = " << negamax(board, 5, -100000, 100000, 0) << "\n";

    delete p1;
    delete p2;
    delete p3;
    delete p5;
    return 0;
}