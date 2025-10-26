#pragma once
#include "../Board.h"
#include <vector>
#include <cctype>
#include <algorithm>

// pawns
static int whitePawnMoves[][2] = {{1, 0}, {2, 0}};
static int whitePawnCaptures[][2] = {{1, -1}, {1, 1}};
static int blackPawnMoves[][2] = {{-1, 0}, {-2, 0}};
static int blackPawnCaptures[][2] = {{-1, -1}, {-1, 1}};
// pawn enpassant
static int whitePawnEnpassant[][2] = {{1, -1}, {1, 1}};
static int blackPawnEnpassant[][2] = {{-1, -1}, {-1, 1}};
// knights
static int knightMoves[][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
// bishops
static int bishopDirections[][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
// rooks
static int rookDirections[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
// queens
static int queenDirections[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
// kings
static int kingMoves[][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

struct Move {
    Position from;
    Position to;
    Piece *pieceMoved;
    Piece *pieceCaptured;
};

static std::vector<Position> generateMovesForPiece(Board &board, Position pos, char symbol, int color){
    symbol = toupper(symbol);
    std::vector<Position> moves;
    switch(symbol){
        case 'P':
            if(color == 0){
                // Generate white pawn moves
                for(auto &move : whitePawnMoves){
                    Position newPos = {pos.row + move[0], pos.col + move[1]};
                    if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && board.isEmpty(newPos)){
                        moves.push_back(newPos);
                    }
                }
            } else {
                // Generate black pawn moves
                for(auto &move : blackPawnMoves){
                    Position newPos = {pos.row + move[0], pos.col + move[1]};
                    if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && board.isEmpty(newPos)){
                        moves.push_back(newPos);
                    }
                }
            }
            break;
        case 'N':
            for(auto &move : knightMoves){
                Position newPos = {pos.row + move[0], pos.col + move[1]};
                if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && board.isEmpty(newPos)){
                    moves.push_back(newPos);
                }
            }
            break;
        case 'B':
            for(auto &dir : bishopDirections){
                for(int i = 1; i < 8; ++i){
                    Position newPos = {pos.row + dir[0] * i, pos.col + dir[1] * i};
                    if(!board.validateMove(pos, newPos, board.getPieceAt(pos)))
                        break;
                    if(board.isEmpty(newPos)){
                        moves.push_back(newPos);
                        continue;
                    }
                    // blocked by any piece -> stop sliding further
                    break;
                }
            }
            break;
        case 'R':
            for(auto &dir : rookDirections){
                for(int i = 1; i < 8; ++i){
                    Position newPos = {pos.row + dir[0] * i, pos.col + dir[1] * i};
                    if(!board.validateMove(pos, newPos, board.getPieceAt(pos)))
                        break;
                    if(board.isEmpty(newPos)){
                        moves.push_back(newPos);
                        continue;
                    }
                    // blocked by any piece -> stop sliding further
                    break;
                }
            }
            break;
        case 'Q':
            for(auto &dir : queenDirections){
                for(int i = 1; i < 8; ++i){
                    Position newPos = {pos.row + dir[0] * i, pos.col + dir[1] * i};
                    if(!board.validateMove(pos, newPos, board.getPieceAt(pos)))
                        break;
                    if(board.isEmpty(newPos)){
                        moves.push_back(newPos);
                        continue;
                    }
                    // blocked by any piece -> stop sliding further
                    break;
                }
            }
            break;
        case 'K':
            for(auto &move : kingMoves){
                Position newPos = {pos.row + move[0], pos.col + move[1]};
                if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && board.isEmpty(newPos)){
                    moves.push_back(newPos);
                }
            }
            break;
    }
    return moves;
}

static std::vector<Position> generateCapturesForPiece(Board &board, Position pos, char symbol, int color){
    symbol = toupper(symbol);
    std::vector<Position> captures;
    switch(symbol){
        case 'P':
            if(color == 0){
                // Generate white pawn captures
                for(auto &cap : whitePawnCaptures){
                    Position newPos = {pos.row + cap[0], pos.col + cap[1]};
                    if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && !board.isEmpty(newPos)){
                        captures.push_back(newPos);
                    }
                }
            } else {
                // Generate black pawn captures
                for(auto &cap : blackPawnCaptures){
                    Position newPos = {pos.row + cap[0], pos.col + cap[1]};
                    if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && !board.isEmpty(newPos)){
                        captures.push_back(newPos);
                    }
                }
            }
            break;
        case 'N':
            for(auto &move : knightMoves){
                Position newPos = {pos.row + move[0], pos.col + move[1]};
                if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && !board.isEmpty(newPos)){
                    captures.push_back(newPos);
                }
            }
            break;
        case 'B':
            for(auto &dir : bishopDirections){
                for(int i = 1; i < 8; ++i){
                    Position newPos = {pos.row + dir[0] * i, pos.col + dir[1] * i};
                    if(!board.validateMove(pos, newPos, board.getPieceAt(pos)))
                        break;
                    if(board.isEmpty(newPos)){
                        continue;
                    }
                    // occupied square: if enemy -> capture, then stop sliding
                    if(board.getPieceAt(newPos)->getColor() != color)
                        captures.push_back(newPos);
                    break;
                }
            }
            break;
        case 'R':
            for(auto &dir : rookDirections){
                for(int i = 1; i < 8; ++i){
                    Position newPos = {pos.row + dir[0] * i, pos.col + dir[1] * i};
                    if(!board.validateMove(pos, newPos, board.getPieceAt(pos)))
                        break;
                    if(board.isEmpty(newPos)){
                        continue;
                    }
                    if(board.getPieceAt(newPos)->getColor() != color)
                        captures.push_back(newPos);
                    break;
                }
            }
            break;
        case 'Q':
            for(auto &dir : queenDirections){
                for(int i = 1; i < 8; ++i){
                    Position newPos = {pos.row + dir[0] * i, pos.col + dir[1] * i};
                    if(!board.validateMove(pos, newPos, board.getPieceAt(pos)))
                        break;
                    if(board.isEmpty(newPos)){
                        continue;
                    }
                    if(board.getPieceAt(newPos)->getColor() != color)
                        captures.push_back(newPos);
                    break;
                }
            }
            break;
        case 'K':
            for(auto &move : kingMoves){
                Position newPos = {pos.row + move[0], pos.col + move[1]};
                if(board.validateMove(pos, newPos, board.getPieceAt(pos)) && !board.isEmpty(newPos)){
                    captures.push_back(newPos);
                }
            }
            break;
        default:
            break;
    }
    return captures;
}

static std::vector<Move> generateAllMoves(Board &board, int color){
    std::vector<Move> allMoves;
    for(int row = 0; row < 8; ++row){
        for(int col = 0; col < 8; ++col){
            Piece *piece = board.getPieceAt({row, col});
            if(piece != nullptr && piece->getColor() == color){
                std::vector<Position> pieceMoves = generateMovesForPiece(board, {row, col}, piece->getSymbol(), color);
                for(auto &pos : pieceMoves){
                    Move mv;
                    mv.from = {row, col};
                    mv.to = pos;
                    mv.pieceMoved = piece;
                    mv.pieceCaptured = board.getPieceAt(pos);
                    allMoves.push_back(mv);
                }
            }
        }
    }
    return allMoves;
}
    
static std::vector<Move> generateAllCaptures(Board &board){
    std::vector<Move> allCaptures;
    for(int row = 0; row < 8; ++row){
        for(int col = 0; col < 8; ++col){
            Piece *piece = board.getPieceAt({row, col});
            if(piece != nullptr){
                std::vector<Position> pieceCaptures = generateCapturesForPiece(board, {row, col}, piece->getSymbol(), piece->getColor());
                for(auto &pos : pieceCaptures){
                    Move mv;
                    mv.from = {row, col};
                    mv.to = pos;
                    mv.pieceMoved = piece;
                    mv.pieceCaptured = board.getPieceAt(pos);
                    allCaptures.push_back(mv);
                }
                
            }
        }
    }
    return allCaptures;
}