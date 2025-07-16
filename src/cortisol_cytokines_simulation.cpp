#include "database_operations.hpp"
#include <sqlite3.h>

#include "cortisol_cytokines_simulation.hpp"

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <boost/numeric/odeint/integrate/integrate.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <fmt/chrono.h>
#include <fmt/color.h>

#include "cortisol_cytokines_model.hpp"
#include "utilities.hpp"

#ifndef NDEBUG
    #include <fmt/chrono.h>
    #include <fmt/color.h>
#endif

CortisolCytokinesSimulation::CortisolCytokinesSimulation(std::filesystem::path input_path, int days, bool plot, bool csv) {
    this->input_path = input_path;
    this->days = days;
    this->plot = plot;
    this->csv = csv;
}

void CortisolCytokinesSimulation::setDays(int days) {
    this->days = days;
}

void CortisolCytokinesSimulation::setInputPath(std::filesystem::path input_path) {
    this->input_path = input_path;
}

void CortisolCytokinesSimulation::setPlot(bool plot) {
    this->plot = plot;
}

void CortisolCytokinesSimulation::setCsv(bool csv) {
    this->csv = csv;
}

void CortisolCytokinesSimulation::startSimulation() const {
    CortisolCytokinesModel cortisol_cytokines_model;
    std::vector<double> initial_conditions = {2, 5, 10, 0.7, 0, 0, 0.17, 2.32};
    nlohmann::json json_file;

    if (!this->input_path.empty()) {
        try {
            std::ifstream file_stream(input_path);
            auto json_file = nlohmann::json::parse(file_stream);
            auto custom_initial_conditions = json_file.at("initial_conditions");

            initial_conditions = {
                custom_initial_conditions.at("antigens"),
                custom_initial_conditions.at("active_macrophages"),
                custom_initial_conditions.at("resting_macrophages"),
                custom_initial_conditions.at("il-10"),
                custom_initial_conditions.at("il-6"),
                custom_initial_conditions.at("il-8"),
                custom_initial_conditions.at("tnf-alpha"),
                custom_initial_conditions.at("cortisol")
            };

            cortisol_cytokines_model.setParameters(json_file);
        } catch (const nlohmann::json::parse_error &exception) {
            fmt::print(stderr, fg(fmt::color::dark_red) | fmt::emphasis::bold, "Error reading from file {}.\n", input_path.string());

#ifndef NDEBUG
            fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "{}", exception.what());
#endif

            exit(512);
        } catch (const nlohmann::json::basic_json::out_of_range &exception) {
            fmt::print(stderr, fg(fmt::color::dark_red) | fmt::emphasis::bold, "Error reading attribute from file.\n");

#ifndef NDEBUG
            fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "{}", exception.what());
#endif

            exit(513);
        }
    } else {
        cortisol_cytokines_model.setDefaultParameters();
        json_file["parameters"] = cortisol_cytokines_model.values.toJson();
        json_file["initial_conditions"] = initial_conditions;
    }

    sqlite3* db;

    if(sqlite3_open("simulations.db", &db) != SQLITE_OK){
        fmt::print(stderr, "Error while opening SQLITE Database.\n");
        return;
    }

    // creates hash from parameters
    std::string param_hash = hashParameters(json_file);
    std::string results_json;

    // checks if the result already exists in the database
    if(simulationExists(db, param_hash, results_json)){
        fmt::print("Simulation already exists in the database. Reusing data:\n");

        auto parsed = nlohmann::json::parse(results_json);
        auto states = parsed.at("states").get<std::vector<std::vector<double>>>();
        auto times = parsed.at("times").get<std::vector<double>>();

        if (this->plot) {
            CortisolCytokinesModel::plotResults(states, times);
        }

        if (this->csv) {
            Utilities::writeCsv(
                {"Time", "Antigens", "Active Macrophages", "Resting Macrophages", "IL10", "IL6", "IL8", "TNF-ɑ", "Cortisol"},
                Utilities::combineStateWithTimeVector(states, times)
            );
        }

        sqlite3_close(db);
        return;
    }


    std::vector<std::vector<double>> states;
    std::vector<double> times;

    fmt::print("Starting new simulation.\n");

#ifndef NDEBUG
    auto simulation_start = std::chrono::high_resolution_clock::now();
#endif

    boost::numeric::odeint::integrate(cortisol_cytokines_model, initial_conditions, 0.0, double(days), 0.0001, Utilities::IntegralObserver(states, times));

#ifndef NDEBUG
    auto simulation_end = std::chrono::high_resolution_clock::now();

    auto simulation_duration = std::chrono::duration_cast<std::chrono::microseconds>(simulation_end - simulation_start);
    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Simulation duration: {} ({})\n", simulation_duration, std::chrono::duration_cast<std::chrono::seconds>(simulation_duration));
#endif

    fmt::print("Simulation done.\n");

    if (this->plot) {
        fmt::print("\nStarting plotting.\n");

#ifndef NDEBUG
        auto plotting_start = std::chrono::high_resolution_clock::now();
#endif

        CortisolCytokinesModel::plotResults(states, times);

#ifndef NDEBUG
        auto plotting_end = std::chrono::high_resolution_clock::now();

        auto plotting_duration = std::chrono::duration_cast<std::chrono::microseconds>(plotting_end - plotting_start);
        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Plotting duration: {} ({})\n", plotting_duration, std::chrono::duration_cast<std::chrono::seconds>(plotting_duration));
#endif

        fmt::print("Plotting done.\n");
    }

    if (this->csv) {
        fmt::print("\nStarting CSV write.\n");

#ifndef NDEBUG
        auto csv_start = std::chrono::high_resolution_clock::now();
        auto csv_combination_start = std::chrono::high_resolution_clock::now();
#endif

        auto combined_state_time = Utilities::combineStateWithTimeVector(states, times);

#ifndef NDEBUG
        auto csv_combination_end = std::chrono::high_resolution_clock::now();
        auto csv_combination_duration = std::chrono::duration_cast<std::chrono::microseconds>(csv_combination_end - csv_combination_start);
        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->Finished combining time with states. Combination duration: {} ({})\n", csv_combination_duration, std::chrono::duration_cast<std::chrono::seconds>(csv_combination_duration));

        auto csv_writing_start = std::chrono::high_resolution_clock::now();
#endif

        Utilities::writeCsv(
            {"Time", "Antigens", "Active Macrophages", "Resting Macrophages", "IL10", "IL6", "IL8", "TNF-ɑ", "Cortisol"},
            combined_state_time
        );

#ifndef NDEBUG
        auto csv_writing_end = std::chrono::high_resolution_clock::now();
        auto csv_writing_duration = std::chrono::duration_cast<std::chrono::microseconds>(csv_writing_end - csv_writing_start);
        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->Finished writing CSV. CSV writing duration: {} ({})\n", csv_writing_duration, std::chrono::duration_cast<std::chrono::seconds>(csv_writing_duration));

        auto csv_end = std::chrono::high_resolution_clock::now();
        auto csv_duration = std::chrono::duration_cast<std::chrono::microseconds>(csv_end - csv_start);
        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "CSV write duration: {} ({})\n", csv_duration, std::chrono::duration_cast<std::chrono::seconds>(csv_duration));
#endif

        fmt::print("CSV write done.\n");
    }

    //  storing in the database
    nlohmann::json results_json_obj;
    results_json_obj["states"] = states;
    results_json_obj["times"] = times;

    storeSimulationResults(db, param_hash, json_file.dump(), results_json_obj.dump());

    sqlite3_close(db);
    fmt::print("Data successfully stored.\n");
}
