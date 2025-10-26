#pragma once
#include "../Piece.h"
#include "../Board.h"
#include <vector>

//   OwnedBoard ob(start);
//   Board &copy = ob.board;
struct OwnedBoard
{
    Board board;                 // the deep-copied board
    std::vector<Piece *> owned;  // list of allocated Piece pointers

    // create deep copy
    explicit OwnedBoard(const Board &src)
    {
        for (int row = 0; row < 8; ++row)
        {
            for (int col = 0; col < 8; ++col)
            {
                Piece *p = const_cast<Board &>(src).getPieceAt({row, col});
                if (!p)
                    continue;
                Piece *clone = new Piece(p->getColor(), p->getSymbol(), p->getPosition());
                clone->value = p->value;
                board.placePiece(clone);
                owned.push_back(clone);
            }
        }
    }

    // prevent accidental copying
    OwnedBoard(const OwnedBoard &) = delete;
    OwnedBoard &operator=(const OwnedBoard &) = delete;

    // destructor
    ~OwnedBoard()
    {
        for (Piece *p : owned)
            delete p;
        owned.clear();
    }
};
