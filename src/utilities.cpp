#include "utilities.hpp"

#ifndef NDEBUG
    #include <fmt/base.h>
    #include <fmt/chrono.h>
    #include <fmt/color.h>

    #include <chrono>
#endif

namespace Utilities {
    std::map<double, std::vector<double>> combineStateWithTime(const std::vector<std::vector<double>> &states, const std::vector<double> &times) {
        if (states.size() != times.size()) {
            throw states.size() - times.size();
        }

        std::map<double, std::vector<double>> mapped_states;

        for (std::size_t i = 0; i < states.size(); i++) {
            mapped_states[times[i]] = states[i];
        }

        return mapped_states;
    }

    IntegralObserver::IntegralObserver(std::vector<std::vector<double>> &states, std::vector<double> &times): m_states(states), m_times(times) {}

    void IntegralObserver::operator()(const std::vector<double> &x, double t) {
#ifndef NDEBUG
        if (m_times.size() != 0) {
            int previous_time = m_times[m_times.size() - 1];
            int previous_time_decade = previous_time / 3650;

            int current_time_decade = t / 3650;

            if (previous_time_decade != current_time_decade) {
                auto current_decade_time = std::chrono::high_resolution_clock::now();

                auto current_decade_duration = std::chrono::duration_cast<std::chrono::microseconds>(current_decade_time - this->previous_decade_time);
                fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->Finished decade {}. Decade simulation duration: {} ({})\n", current_time_decade, current_decade_duration, std::chrono::duration_cast<std::chrono::seconds>(current_decade_duration));

                this->previous_decade_time = current_decade_time;
            }
        }
#endif

        m_states.push_back(x);
        m_times.push_back(t);
    }
}  // namespace Utilities
