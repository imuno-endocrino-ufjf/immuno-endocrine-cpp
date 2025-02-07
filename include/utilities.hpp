#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <fmt/base.h>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>

#ifndef NDEBUG
    #include <chrono>
#endif

namespace Utilities {
    std::map<double, std::vector<double>> combineStateWithTimeMap(const std::vector<std::vector<double>> &states, const std::vector<double> &times);
    std::vector<std::vector<double>> combineStateWithTimeVector(const std::vector<std::vector<double>> &states, const std::vector<double> &times);

    template<class Type>
    class GetClosestValue {
        private:
            std::map<double, Type> map;
            std::map<double, Type>::iterator last_position;
            double last_key;

        public:
            inline void setValues(std::map<double, Type> map) {
                this->map = map;
                this->last_position = this->map.begin();
                this->last_key = 0;
            }

            inline Type find(double key) {
                typedef typename std::map<double, Type>::iterator MapType;

                MapType begin = this->map.begin();

                if (last_key <= key) {
                    auto next_to_last_position = std::next(this->last_position);


                    if (next_to_last_position == this->map.end()) {
                        this->last_key = key;
                        return last_position->second;
                    }

                    if (std::fabs(last_position->first - key) < std::fabs(next_to_last_position->first - key)) {
                        this->last_key = key;
                        return last_position->second;
                    } else {
                        this->last_key = key;
                        this->last_position = next_to_last_position;
                        return next_to_last_position->second;
                    }
                } else {
                    if (last_position == begin) {
                        return begin->second;
                    }

                    MapType previous_to_last_position = std::prev(this->last_position);

                    if (
                        (std::fabs(begin->first - key) < std::fabs(previous_to_last_position->first - key)) &&
                        (std::fabs(begin->first - key) < std::fabs(last_position->first - key))
                    ) {
                        this->last_key = key;
                        this->last_position = begin;
                        return begin->second;
                    } else if (
                        std::fabs(last_position->first - key) < std::fabs(previous_to_last_position->first - key)
                    ) {
                        this->last_key = key;
                        return last_position->second;
                    } else {
                        this->last_key = key;
                        this->last_position = previous_to_last_position;
                        return previous_to_last_position->second;
                    }
                }

                // typename std::map<double, Type>::iterator lower_bound = map.lower_bound(key);

                // if (lower_bound == map.begin()) {
                //     return lower_bound->second;
                // }

                // typename std::map<double, Type>::iterator previous_lower_bound = std::prev(lower_bound);

                // if (lower_bound == map.end()) {
                //     return previous_lower_bound->second;
                // } else {
                //     double lower_bound_difference = lower_bound->first - key;
                //     double previous_lower_bound_difference = key - previous_lower_bound->first;

                //     // removing this if check reduces the simulation time per decade by around 5 seconds
                //     // which isn't a substantial enough difference to also slightly reduce precision
                //     if (lower_bound_difference < previous_lower_bound_difference) {
                //         return lower_bound->second;
                //     } else {
                //         return previous_lower_bound->second;
                //     }
                // }
            }
    };

    std::map<double, double>
        vectorToMap(std::vector<std::vector<double>> vector);

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

    void writeCsv(const std::vector<std::string> &header, const std::vector<std::vector<double>> &values, const std::filesystem::path &output_path = "output/values.csv");
}  // namespace Utilities

#endif
