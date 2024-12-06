#include "utilities.hpp"

namespace Utilities {
    std::map<double, std::vector<double>> combineStateWithTime(const std::vector<std::vector<double>> &states, const std::vector<double> &times) {
        if (states.size() != times.size()) {
            throw states.size() - times.size();
        }

        std::map<double, std::vector<double>> mapped_states;

        for (size_t i = 0; i < states.size(); i++) {
            mapped_states[times[i]] = states[i];
        }

        return mapped_states;
    }

    SaveAllStates::SaveAllStates(std::vector<std::vector<double>> &states, std::vector<double> &times): m_states(states), m_times(times) {}

    void SaveAllStates::operator()(const std::vector<double> &x, double t) {
        m_states.push_back(x);
        m_times.push_back(t);
    }
}  // namespace Utilities
