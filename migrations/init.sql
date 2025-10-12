PRAGMA foreign_keys = ON;  

CREATE TABLE IF NOT EXISTS players (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    score INTEGER DEFAULT 0
    games_played INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS games (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    white_id     BIGINT NOT NULL REFERENCES players(id),
    black_id     BIGINT NOT NULL REFERENCES players(id),
    date_played DATETIME DEFAULT CURRENT_TIMESTAMP,
    result TEXT CHECK (result IN ('1-0','0-1','1/2-1/2','*')) DEFAULT '*',
    initial_fen  TEXT NOT NULL DEFAULT 'startpos',
    FOREIGN KEY (white_id) REFERENCES players(id) ON DELETE CASCADE,
    FOREIGN KEY (black_id) REFERENCES players(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS moves (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    game_id INTEGER,
    ply INTEGER NOT NULL,
    san TEXT NOT NULL,
    uci TEXT NOT NULL,
    fen_after TEXT NOT NULL,
    FOREIGN KEY (game_id) REFERENCES games(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_player_name ON players(name);
CREATE INDEX IF NOT EXISTS idx_game_player ON games(player_id);
CREATE INDEX IF NOT EXISTS idx_move_game ON moves(game_id);