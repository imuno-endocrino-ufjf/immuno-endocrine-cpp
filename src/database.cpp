#include "database.hpp"
#include <stdexcept>

Database::Database(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database");
    }
}

Database::~Database() {
    sqlite3_close(db);
}

sqlite3* Database::get() {
    return db;
}
