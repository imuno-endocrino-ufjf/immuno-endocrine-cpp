#include "cortisol_cytokines_simulation.hpp"

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <boost/numeric/odeint/integrate/integrate.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "cortisol_cytokines.hpp"
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

void CortisolCytokinesSimulation::startSimulation() {
    CortisolCytokines cortisol_cytokines;
    std::vector<double> x = {2, 5, 10, 0.7, 0, 0, 0.17, 2.32};

    if (!input_path.empty()) {
        try {
            std::ifstream file_stream(input_path);
            auto json = nlohmann::json::parse(file_stream);
            auto initial_conditions = json.at("initial_conditions");

            x = {
                initial_conditions.at("antigens"),
                initial_conditions.at("active_macrophages"),
                initial_conditions.at("resting_macrophages"),
                initial_conditions.at("il-10"),
                initial_conditions.at("il-6"),
                initial_conditions.at("il-8"),
                initial_conditions.at("tnf-alpha"),
                initial_conditions.at("cortisol")
            };

            cortisol_cytokines.setParameters(json);
        } catch (const nlohmann::json::parse_error &exception) {
            fmt::print(stderr, "Error reading from file {}.\n", input_path.string());

#ifndef NDEBUG
            fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "{}", exception.what());
#endif

            exit(512);
        } catch (const nlohmann::json::basic_json::out_of_range &exception) {
            fmt::print(stderr, "Error reading attribute from file.\n");

#ifndef NDEBUG
            fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "{}", exception.what());
#endif

            exit(513);
        }
    }

    std::vector<std::vector<double>> states;
    std::vector<double> times;

    fmt::print("Starting simulation.\n");

#ifndef NDEBUG
    auto simulation_start = std::chrono::high_resolution_clock::now();
#endif

    boost::numeric::odeint::integrate(cortisol_cytokines, x, 0.0, double(days), 0.001, Utilities::IntegralObserver(states, times));

#ifndef NDEBUG
    auto simulation_end = std::chrono::high_resolution_clock::now();

    auto simulation_duration = std::chrono::duration_cast<std::chrono::microseconds>(simulation_end - simulation_start);
    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Simulation duration: {} ({})\n", simulation_duration, std::chrono::duration_cast<std::chrono::seconds>(simulation_duration));
#endif

    fmt::print("Simulation done.\n");

    if (plot) {
        fmt::print("\nStarting plotting.\n");

#ifndef NDEBUG
        auto plotting_start = std::chrono::high_resolution_clock::now();
#endif

        CortisolCytokines::plotResults(states, times);

#ifndef NDEBUG
        auto plotting_end = std::chrono::high_resolution_clock::now();

        auto plotting_duration = std::chrono::duration_cast<std::chrono::microseconds>(plotting_end - plotting_start);
        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Plotting duration: {} ({})\n", plotting_duration, std::chrono::duration_cast<std::chrono::seconds>(plotting_duration));
#endif

        fmt::print("Plotting done.\n");
    }

    if (csv) {
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
}
