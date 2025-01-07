#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <map>
#include <stdexcept>
#include <vector>

#ifndef NDEBUG
    #include <chrono>
#endif

namespace Utilities {
    std::map<double, std::vector<double>> combineStateWithTimeMap(const std::vector<std::vector<double>> &states, const std::vector<double> &times);
    std::vector<std::vector<double>> combineStateWithTimeVector(const std::vector<std::vector<double>> &states, const std::vector<double> &times);

    template<class Type>
    inline std::optional<Type> readParameter(
        const std::vector<std::string> &parameter,
        char *parameter_input,
        char *value_input,
        Type (*interpret_value)(std::string) = [](std::string input) -> Type {
            return Type(input);
        }
    ) {
        bool match_found = false;
        for (auto parameter_alias : parameter) {
            if (std::string(parameter_input).compare(parameter_alias) == 0) {
                match_found = true;
                break;
            }
        }

        if (match_found) {
            if (value_input != nullptr) {
                return std::optional(interpret_value(value_input));
            } else {
                throw std::invalid_argument("Missing value");
            }
        }

        return std::nullopt;
    };

    class IntegralObserver {
        private:
            std::vector<std::vector<double>> &m_states;
            std::vector<double> &m_times;

#ifndef NDEBUG
            std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<long int, std::ratio<1LL, 1000000000LL>>> previous_decade_time = std::chrono::high_resolution_clock::now();
#endif

        public:
            IntegralObserver(std::vector<std::vector<double>> &states, std::vector<double> &times);
            void operator()(const std::vector<double> &x, double t);
    };
}  // namespace Utilities

#endif
