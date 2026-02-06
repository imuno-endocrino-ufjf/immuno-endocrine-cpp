#pragma once
#include <string>
#include <optional>

struct SimulationQueryResult {
    bool found;
    std::string csv_path;
};

class DatabaseOperations {
public:
    DatabaseOperations(const std::string& db_path);

    SimulationQueryResult findSimulation(
        const std::string& parameters_hash,
        int model_version,
        int days
    );

    void storeSimulation(
        const std::string& parameters_json,
        const std::string& parameters_hash,
        int model_version,
        const std::string& csv_path,
        int days,
        const std::string& status
    );
};
