#include <iostream>
#include "../Board.h"
#include "../engine/engine.h"
#include "../engine/val.h"

int main()
{
    int failed = 0;

    // Test 1: eval simple material sum
    {
        Board b;
        Piece *q = new Piece(0, 'Q', {0, 0});
        Piece *r = new Piece(1, 'R', {0, 1});
        b.placePiece(q);
        b.placePiece(r);
        int got = eval(b);
        int expect = pieceValFromSymbol('Q') - pieceValFromSymbol('R');
        if (got != expect)
        {
            std::cerr << "test_eval_simple FAILED: got " << got << " expected " << expect << "\n";
            ++failed;
        }
        else
        {
            std::cerr << "test_eval_simple OK\n";
        }
        delete q; delete r;
    }

    // Test 2: negamax runs and increments nodesVisited
    {
        Board b;
        Piece *wK = new Piece(0, 'K', {7, 7});
        Piece *bK = new Piece(1, 'K', {0, 0});
        Piece *wQ = new Piece(0, 'Q', {6, 6});
        Piece *bQ = new Piece(1, 'Q', {1, 1});
        Piece *bB = new Piece(1, 'B', {2, 2});
        Piece *bN = new Piece(1, 'N', {3, 3});
        b.placePiece(wK);
        b.placePiece(bK);
        b.placePiece(wQ);
        b.placePiece(bQ);
        b.placePiece(bB);
        b.placePiece(bN);
        long before = get_nodes_visited();
        int res = negamax(b, 1, -100000, 100000, 0);
        long after = get_nodes_visited();
        (void)res;
        if (after <= before)
        {
            std::cerr << "test nodesVisited FAILED: before " << before << " after " << after << "\n";
            ++failed;
        }
        else
        {
            std::cerr << "test negamax nodesVisited OK\n";
        }
        delete wK; delete bK; delete wQ; delete bQ; delete bB; delete bN;
    }

    // Test 3: see executes
    {
        Board b;
        Piece *wP = new Piece(0, 'P', {1, 1});
        Piece *bP = new Piece(1, 'P', {2, 2});
        Piece *wK = new Piece(0, 'K', {7, 7});
        Piece *bK = new Piece(1, 'K', {0, 0});
        Piece *wQ = new Piece(0, 'Q', {6, 6});
        Piece *bQ = new Piece(1, 'Q', {1, 1});
        b.placePiece(wK);
        b.placePiece(bK);
        b.placePiece(wP);
        b.placePiece(bP);
        b.placePiece(wQ);
        b.placePiece(bQ);
        Position target{2,2};
        int s = see(b, target, 0);
        std::cerr << "see returned " << s << "\n";
        delete wP; delete bP; delete wK; delete bK; delete wQ; delete bQ;
    }

    if (failed)
    {
        std::cerr << failed << " tests failed\n";
        return 1;
    }

    std::cerr << "ALL TESTS PASSED\n";
    return 0;
}
