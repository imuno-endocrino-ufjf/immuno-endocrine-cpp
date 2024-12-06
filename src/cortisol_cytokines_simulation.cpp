#include "cortisol_cytokines_simulation.hpp"

#include <boost/numeric/odeint/integrate/integrate.hpp>

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

    boost::numeric::odeint::integrate(cortisol_cytokines, x, 0.0, double(days), 0.001, Utilities::SaveAllStates(states, times));

    CortisolCytokines::plotResults(states, times, 0);
}
