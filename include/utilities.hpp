#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <fmt/base.h>

#include <cmath>
#include <filesystem>
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

                if (this->last_key <= key) {
                    auto previous_loop_position = this->last_position;
                    auto current_loop_position = std::next(previous_loop_position);

                    while (true) {
                        if (
                            (current_loop_position == this->map.end()) ||
                            (std::fabs(previous_loop_position->first - key) < std::fabs(current_loop_position->first - key))
                        ) {
                            this->last_key = key;
                            this->last_position = previous_loop_position;

                            return previous_loop_position->second;
                        } else {
                            previous_loop_position = current_loop_position;
                            current_loop_position = std::next(current_loop_position);
                        }
                    }
                } else {
                    auto previous_loop_position = this->last_position;
                    auto current_loop_position = std::prev(previous_loop_position);

                    if (
                        (std::fabs(this->map.begin()->first - key) < std::fabs(previous_loop_position->first - key)) &&
                        (std::fabs(this->map.begin()->first - key) < std::fabs(current_loop_position->first - key))
                    ) {
                        this->last_key = key;
                        this->last_position = this->map.begin();

                        return this->map.begin()->second;
                    }

                    while (true) {
                        if (std::fabs(previous_loop_position->first - key) < std::fabs(current_loop_position->first - key)) {
                            this->last_key = key;
                            this->last_position = previous_loop_position;

                            return previous_loop_position->second;
                        } else if (current_loop_position == this->map.begin()) {
                            this->last_key = key;
                            this->last_position = current_loop_position;

                            return current_loop_position->second;
                        } else {
                            previous_loop_position = current_loop_position;
                            current_loop_position = std::prev(current_loop_position);
                        }
                    }
                }
            }
    };

    std::map<double, double> vectorToMap(std::vector<std::vector<double>> vector);

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
            // auto keyword can't be used for class members
            std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<long int, std::ratio<1LL, 1000000000LL>>> previous_decade_time = std::chrono::high_resolution_clock::now();
#endif

        public:
            IntegralObserver(std::vector<std::vector<double>> &states, std::vector<double> &times);
            void operator()(const std::vector<double> &x, double t);
    };

    void writeCsv(const std::vector<std::string> &header, const std::vector<std::vector<double>> &values, const std::filesystem::path &output_path = "output/values.csv");
}  // namespace Utilities

#endif
