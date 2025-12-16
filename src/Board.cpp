#include "Board.h"
#include "Piece.h"
#include "Pawn.h"
#include "kNight.h"
#include "Queen.h"
#include "Bishop.h"
#include "Rook.h"
#include "King.h"
#include "engine/tables/zobrist.h"   
#include <iostream>
#include <cmath>
#include <vector>

Piece* Board::Display() {
    return squares[1][1];
}
using namespace std;
const int B_SIZE = 8;

Board::Board() {
    for (int i = 0; i < B_SIZE; i++)
        for (int j = 0; j < B_SIZE; j++)
            squares[i][j] = nullptr;
}

Piece* Board::Display() { return squares[0][0]; }
bool Board::isEmpty(Position pos) { return getPieceAt(pos) == nullptr; }

Piece* Board::getPieceAt(Position pos) {
    if (pos.row < 0 || pos.row >= B_SIZE || pos.col < 0 || pos.col >= B_SIZE) return nullptr;
    return squares[pos.row][pos.col];
}

void Board::placePiece(Piece* piece) {
    if (!piece) return;
    Position pos = piece->getPosition();
    if (pos.row >= 0 && pos.row < B_SIZE && pos.col >= 0 && pos.col < B_SIZE)
        squares[pos.row][pos.col] = piece;
}

bool Board::validateMove(Position old_pos, Position new_pos, Piece* piece) {
    if (new_pos.row < 0 || new_pos.row >= 8 || new_pos.col < 0 || new_pos.col >= 8)
        return false;
	return true;
}

bool Board::movePiece(Position oldpos, Position newpos, Piece* piece) {
    if (!piece) return false;
    // Tutaj zakładamy, że ruch jest już zweryfikowany przez isMoveSafe w main.cpp
    // Ale dla pewności można zostawić sprawdzenie canMove, jeśli wywołujesz to spoza maina
    if (piece->canMove(newpos, this)) {
        squares[oldpos.row][oldpos.col] = nullptr;
        squares[newpos.row][newpos.col] = piece;
        piece->setPosition(newpos);
        return true;
    }
    return false;
}

Position Board::findKing(int color) {
    // Optymalizacja: Można by to cache'ować w zmiennej, ale przy 64 polach to nie jest główne gardło.
    for (int r = 0; r < B_SIZE; r++)
        for (int c = 0; c < B_SIZE; c++) {
            Piece* p = squares[r][c];
            if (p && p->getSymbol() == 'K' && p->getColor() == color) return { r, c };
        }
    return { -1, -1 };
}

bool Board::isSquareAttacked(Position pos, int enemyColor) {
    if (pos.row < 0 || pos.row >= B_SIZE) return false;

    // 1. LINIE PROSTE (Wieża/Hetman)
    int straight[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
    for (auto d : straight) {
        for (int i = 1; i < B_SIZE; i++) {
            int r = pos.row + d[0] * i, c = pos.col + d[1] * i;
            if (r < 0 || r >= B_SIZE || c < 0 || c >= B_SIZE) break;
            Piece* p = squares[r][c];
            if (p) {
                if (p->getColor() == enemyColor && (p->getSymbol() == 'R' || p->getSymbol() == 'Q')) return true;
                break;
            }
        }
    }

    // 2. SKOSY (Goniec/Hetman)
    int diag[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
    for (auto d : diag) {
        for (int i = 1; i < B_SIZE; i++) {
            int r = pos.row + d[0] * i, c = pos.col + d[1] * i;
            if (r < 0 || r >= B_SIZE || c < 0 || c >= B_SIZE) break;
            Piece* p = squares[r][c];
            if (p) {
                if (p->getColor() == enemyColor && (p->getSymbol() == 'B' || p->getSymbol() == 'Q')) return true;
                break;
            }
        }
    }

    // 3. SKOCZEK
    int km[8][2] = { {2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2} };
    for (auto m : km) {
        int r = pos.row + m[0], c = pos.col + m[1];
        if (r >= 0 && r < B_SIZE && c >= 0 && c < B_SIZE) {
            Piece* p = squares[r][c];
            if (p && p->getColor() == enemyColor && p->getSymbol() == 'N') return true;
        }
    }

    // 4. PIONEK (Uwaga na kierunek ataku przeciwnika)
    // Jeśli atakują nas BIAŁE (enemy=0), to one są na dole i biją w górę (row+1)
    // Jeśli atakują nas CZARNE (enemy=1), to one są na górze i biją w dół (row-1)
    // My sprawdzamy "skąd przyleciało", więc odwracamy logikę:
    int attackDir = (enemyColor == 0) ? -1 : 1;

    int pr = pos.row + attackDir;
    if (pr >= 0 && pr < B_SIZE) {
        if (pos.col - 1 >= 0) {
            Piece* p = squares[pr][pos.col - 1];
            if (p && p->getColor() == enemyColor && p->getSymbol() == 'P') return true;
        }
        if (pos.col + 1 < B_SIZE) {
            Piece* p = squares[pr][pos.col + 1];
            if (p && p->getColor() == enemyColor && p->getSymbol() == 'P') return true;
        }
    }

    // 5. KRÓL
    for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) {
        if (i == 0 && j == 0) continue;
        int r = pos.row + i, c = pos.col + j;
        if (r >= 0 && r < B_SIZE && c >= 0 && c < B_SIZE) {
            Piece* p = squares[r][c];
            if (p && p->getColor() == enemyColor && p->getSymbol() == 'K') return true;
        }
    }

    return false;
}

bool Board::isKingInCheck(int color) {
    Position k = findKing(color);
    if (k.row == -1) return false;
    return isSquareAttacked(k, (color == 0) ? 1 : 0);
}

bool Board::isMoveSafe(Position start, Position end) {
    Piece* p = getPieceAt(start);
    if (!p) return false;

    // Najpierw szybka weryfikacja czy pole docelowe nie jest zajęte przez swojego
    Piece* target = getPieceAt(end);
    if (target && target->getColor() == p->getColor()) return false;

    // Sprawdzenie geometrii ruchu (To jest kosztowne w pętli, dlatego w isCheckMate to zoptymalizujemy)
    if (!p->canMove(end, this)) return false;

    Piece* cap = squares[end.row][end.col];
    Position old = p->getPosition();

    // Symulacja
    squares[end.row][end.col] = p;
    squares[start.row][start.col] = nullptr;
    p->setPosition(end);

    bool safe = !isKingInCheck(p->getColor());

    // Cofnięcie
    squares[start.row][start.col] = p;
    squares[end.row][end.col] = cap;
    p->setPosition(old);

    return safe;
}

// --- OPTYMALIZACJA GŁÓWNA ---
// Zamiast sprawdzać wszystkie pola (64x64), sprawdzamy tylko sensowne ruchy
bool Board::isCheckMate(int color) {
    if (!isKingInCheck(color)) return false;

    for (int r = 0; r < B_SIZE; r++) {
        for (int c = 0; c < B_SIZE; c++) {
            Piece* p = squares[r][c];
            if (!p || p->getColor() != color) continue;

            // Generujemy listę potencjalnych celów w zależności od typu figury
            // Zamiast pętli 0..7 po całej planszy
            vector<Position> candidates;
            char sym = p->getSymbol();
            Position start = { r, c };

            if (sym == 'P') { // PIONEK
                int dir = (color == 0) ? 1 : -1;
                // Ruch o 1
                candidates.push_back({ r + dir, c });
                // Ruch o 2 (tylko z pozycji startowej)
                if ((color == 0 && r == 1) || (color == 1 && r == 6))
                    candidates.push_back({ r + 2 * dir, c });
                // Bicia
                candidates.push_back({ r + dir, c - 1 });
                candidates.push_back({ r + dir, c + 1 });
            }
            else if (sym == 'N') { // SKOCZEK
                int moves[8][2] = { {2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2} };
                for (auto m : moves) candidates.push_back({ r + m[0], c + m[1] });
            }
            else if (sym == 'K') { // KRÓL
                for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++)
                    if (i != 0 || j != 0) candidates.push_back({ r + i, c + j });
            }
            else { // FIGURY LINIOWE (R, B, Q)
                // Dla nich musimy "iść" w danym kierunku aż do przeszkody
                vector<pair<int, int>> dirs;
                if (sym == 'R' || sym == 'Q') { // Prosto
                    dirs.push_back({ 1,0 }); dirs.push_back({ -1,0 }); dirs.push_back({ 0,1 }); dirs.push_back({ 0,-1 });
                }
                if (sym == 'B' || sym == 'Q') { // Skosy
                    dirs.push_back({ 1,1 }); dirs.push_back({ 1,-1 }); dirs.push_back({ -1,1 }); dirs.push_back({ -1,-1 });
                }

                for (auto d : dirs) {
                    for (int i = 1; i < B_SIZE; i++) {
                        int nr = r + d.first * i;
                        int nc = c + d.second * i;
                        if (nr < 0 || nr >= B_SIZE || nc < 0 || nc >= B_SIZE) break;

                        candidates.push_back({ nr, nc });

                        // Jeśli napotkamy jakąkolwiek figurę, dalej nie możemy iść (blokada)
                        // isMoveSafe sprawdzi, czy to wróg (bicie) czy swój (stop), ale pętla musi się tu urwać
                        if (squares[nr][nc] != nullptr) break;
                    }
                }
            }

            // Teraz sprawdzamy tylko wygenerowane, sensowne ruchy
            for (auto target : candidates) {
                // Filtrowanie wyjścia poza planszę
                if (target.row < 0 || target.row >= B_SIZE || target.col < 0 || target.col >= B_SIZE) continue;

                // Sprawdzamy czy ruch ratuje przed matem
                if (isMoveSafe(start, target)) {
                    return false; // Znaleziono ratunek! Gra toczy się dalej.
                }
            }
        }
    }
    return true; // Przeanalizowano wszystkie sensowne ruchy i żaden nie ratuje króla -> MAT
}

void Board::DisplayBoard() {
    for (int r = B_SIZE - 1; r >= 0; r--) {
        for (int c = 0; c < B_SIZE; c++) {
            if (squares[r][c]) cout << squares[r][c]->getSymbol() << " ";
            else cout << ". ";
        }
        cout << endl;
    }
}

Piece* Board::getPieceAt(Position pos) {
    if (pos.row < 0 || pos.row >= sizeboard || pos.col < 0 || pos.col >= sizeboard)
        return nullptr; // poza planszą
    return squares[pos.row][pos.col];
}

void Board::promotePawn(Board &board, Position pos, char newSymbol, int color) {
    Piece* promoted = board.getPieceAt(pos);
	if (promoted->getSymbol() != 'P') return; // Only pawns can be promoted
	delete promoted;

	Piece* newPiece = new Queen(color, newSymbol, pos); // Default to Queen

	board.squares[pos.row][pos.col] = newPiece;

}
void Board::computeZobristHash() {
   zobristKey = 0;
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++) {
			Piece* p = squares[r][c];
            if (p) {
				int idx = getPieceIndex(p->getSymbol(), p->getColor());
				int square = r * 8 + c;
				zobristKey ^= pieceKeys[idx][square];
            }
        }
    }
}