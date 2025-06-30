// this is a temporary fix for fmt/color.h using std::copy
// this issue has already been fix but a version with the bugfix hasn't been released yet
// clang-format off
// ↑ without this clang-format tries to put this include beneath the fmt/color.h include which breaks the patch
#include <algorithm>
// clang-format on

#include <fmt/base.h>
#include <fmt/color.h>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdio.h>


#include "cortisol_cytokines_simulation.hpp"
#include "utilities.hpp"
#include "database_operations.h"
#include "sqlite3.h"

int main(int argc, char *argv[]) {
    std::filesystem::path input_path;
    int days = 36500;
    bool plot = true;
    bool csv = true;

#ifndef NDEBUG
    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "Profiling enabled!\n\n");
#endif

    for (int i = 1; i < argc; i++) {
        try {
            if (
                auto input_path_return = Utilities::readParameter<std::filesystem::path>(
                    {"-i", "--input"},
                    argv[i],
                    argv[i + 1],
                    [](std::string input) -> std::filesystem::path {
                        std::filesystem::path input_path = input;

                        if (!std::filesystem::is_regular_file(input_path)) {
                            fmt::print(stderr, fg(fmt::color::dark_red) | fmt::emphasis::bold, "Invalid file path: {}\n", input);
                            exit(3);
                        }

                        return input_path;
                    }
                )
            ) {
                input_path = input_path_return.value();
                i++;
            } else if (
                auto days_return = Utilities::readParameter<int>(
                    {"-d", "--days"},
                    argv[i],
                    argv[i + 1],
                    [](std::string input) -> int {
                        int days = std::stoi(input);

                        if (days <= 0) {
                            fmt::print(stderr, fg(fmt::color::dark_red) | fmt::emphasis::bold, "Invalid amount of days: {}\n", days);
                            exit(3);
                        }

                        return days;
                    }
                )
            ) {
                days = days_return.value();
                i++;
            } else if (
                auto no_plot_return = Utilities::readParameter<bool>(
                    {"--no-plot"},
                    argv[i],
                    (char *) "1",
                    [](std::string input) -> bool {
                        return true;
                    }
                )
            ) {
                plot = false;
            } else if (
                auto no_csv_return = Utilities::readParameter<bool>(
                    {"--no-csv"},
                    argv[i],
                    (char *) "1",
                    [](std::string input) -> bool {
                        return true;
                    }
                )
            ) {
                csv = false;
            } else {
                fmt::print(stderr, fg(fmt::color::dark_red) | fmt::emphasis::bold, "Unknown parameter: {}\n", argv[i]);
                exit(1);
            }
        } catch (std::invalid_argument exception) {
            fmt::print(stderr, fg(fmt::color::dark_red) | fmt::emphasis::bold, "Missing value for paramter: {}\n", argv[i]);
            exit(2);
        }
    }

    CortisolCytokinesSimulation cortisol_cytokines_simulation;

    cortisol_cytokines_simulation.setDays(days);
    cortisol_cytokines_simulation.setPlot(plot);
    cortisol_cytokines_simulation.setCsv(csv);

    if (!input_path.empty()) {
        cortisol_cytokines_simulation.setInputPath(input_path);
    }

    cortisol_cytokines_simulation.startSimulation();

    return 0;
}
