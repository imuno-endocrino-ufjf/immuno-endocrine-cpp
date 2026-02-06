CREATE TABLE IF NOT EXISTS input (
    input_id INTEGER PRIMARY KEY AUTOINCREMENT,
    model_parameters TEXT NOT NULL,
    parameters_hash TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS model (
    model_id INTEGER PRIMARY KEY AUTOINCREMENT,
    model_description TEXT,
    model_version INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS output (
    output_id INTEGER PRIMARY KEY AUTOINCREMENT,
    model_id INTEGER NOT NULL,
    input_id INTEGER NOT NULL,
    results_path TEXT NOT NULL,
    FOREIGN KEY (model_id) REFERENCES model(model_id),
    FOREIGN KEY (input_id) REFERENCES input(input_id)
);

CREATE TABLE IF NOT EXISTS simulation (
    simulation_id INTEGER PRIMARY KEY AUTOINCREMENT,
    output_id INTEGER NOT NULL,
    time_days INTEGER NOT NULL,
    simulation_status TEXT NOT NULL,
    FOREIGN KEY (output_id) REFERENCES output(output_id)
);
