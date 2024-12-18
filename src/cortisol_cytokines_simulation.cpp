#include "cortisol_cytokines_simulation.hpp"

#include <fmt/base.h>

#ifndef NDEBUG
    #include <fmt/chrono.h>
    #include <fmt/color.h>
#endif

#include <boost/numeric/odeint/integrate/integrate.hpp>
#include <chrono>

#include "cortisol_cytokines.hpp"
#include "utilities.hpp"

CortisolCytokinesSimulation::CortisolCytokinesSimulation(std::filesystem::path input_path, int days) {
    this->input_path = input_path;
    this->days = days;
}

void CortisolCytokinesSimulation::setDays(int days) {
    this->days = days;
}

void CortisolCytokinesSimulation::setInputPath(std::filesystem::path input_path) {
    this->input_path = input_path;
}

void CortisolCytokinesSimulation::startSimulation() {
    CortisolCytokines cortisol_cytokines;
    if (!input_path.empty()) {
        cortisol_cytokines = CortisolCytokines(input_path);
    }

    std::vector<double> x = {2, 5, 10, 0, 0, 0.7, 0.17, 2.32};

    std::vector<std::vector<double>> states;
    std::vector<double> times;

    fmt::println("Starting simulation.");

#ifndef NDEBUG
    auto simulation_start = std::chrono::high_resolution_clock::now();
#endif

    boost::numeric::odeint::integrate(cortisol_cytokines, x, 0.0, double(days), 0.001, Utilities::IntegralObserver(states, times));

#ifndef NDEBUG
    auto simulation_end = std::chrono::high_resolution_clock::now();

    auto simulation_duration = std::chrono::duration_cast<std::chrono::microseconds>(simulation_end - simulation_start);
    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Simulation duration: {} ({})\n\n", simulation_duration, std::chrono::duration_cast<std::chrono::seconds>(simulation_duration));
#endif

    fmt::println("Starting plotting.");

#ifndef NDEBUG
    auto plotting_start = std::chrono::high_resolution_clock::now();
#endif

    CortisolCytokines::plotResults(states, times, 0);

#ifndef NDEBUG
    auto plotting_end = std::chrono::high_resolution_clock::now();

    auto plotting_duration = std::chrono::duration_cast<std::chrono::microseconds>(plotting_end - plotting_start);
    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Plotting duration: {} ({})\n", plotting_duration, std::chrono::duration_cast<std::chrono::seconds>(plotting_duration));
#endif
}
