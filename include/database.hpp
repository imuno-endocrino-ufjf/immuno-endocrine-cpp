#pragma once

#include <string>
#include <optional>
#include <sqlite3.h>

class Database {
public:
    Database(const std::string& db_path);
    ~Database();

    bool simulationExists(const std::string& hash,
                          std::string& results_path_out);

    void insertSimulation(const std::string& hash,
                          const std::string& config_json,
                          const std::string& results_path);

private:
    sqlite3* db;

    void initializeSchema();
};
