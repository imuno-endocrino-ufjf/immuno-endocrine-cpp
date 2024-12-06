#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <map>
#include <vector>

namespace Utilities {
    std::map<double, std::vector<double>> combineStateWithTime(const std::vector<std::vector<double>> &states, const std::vector<double> &times);

    class SaveAllStates {
        private:
            std::vector<std::vector<double>> &m_states;
            std::vector<double> &m_times;

        public:
            SaveAllStates(std::vector<std::vector<double>> &states, std::vector<double> &times);
            void operator()(const std::vector<double> &x, double t);
    };
}  // namespace Utilities

#endif
