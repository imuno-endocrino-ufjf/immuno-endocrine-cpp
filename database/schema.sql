CREATE TABLE IF NOT EXISTS simulations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    parameters_hash TEXT NOT NULL UNIQUE,

    config_json TEXT NOT NULL,

    results_path TEXT NOT NULL,

    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);