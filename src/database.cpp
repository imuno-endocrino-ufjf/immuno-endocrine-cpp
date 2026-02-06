#include "database.hpp"
#include <stdexcept>

Database::Database(const std::string& db_path) : db(nullptr) {
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database");

    }
    initializeSchema();
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

bool Database::simulationExists(const std::string& hash,
                                std::string& results_path_out) {
    const char* sql =
        "SELECT results_path FROM simulations WHERE parameters_hash = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare SELECT");
    }

    sqlite3_bind_text(stmt, 1, hash.c_str(), -1, SQLITE_STATIC);

    bool exists = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        results_path_out =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        exists = true;
    }

    sqlite3_finalize(stmt);
    return exists;
}

void Database::insertSimulation(const std::string& hash,
                                const std::string& config_json,
                                const std::string& results_path) {
    const char* sql =
        "INSERT INTO simulations (parameters_hash, config_json, results_path) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare INSERT");
    }

    sqlite3_bind_text(stmt, 1, hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, config_json.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, results_path.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert simulation");
    }

    sqlite3_finalize(stmt);
}

void Database::initializeSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS simulations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            parameters_hash TEXT UNIQUE NOT NULL,
            config_json TEXT NOT NULL,
            results_path TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    char* error_message = nullptr;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &error_message) != SQLITE_OK) {
        std::string error = error_message ? error_message : "unknown error";
        sqlite3_free(error_message);
        throw std::runtime_error("Failed to initialize database schema: " + error);
    }
}