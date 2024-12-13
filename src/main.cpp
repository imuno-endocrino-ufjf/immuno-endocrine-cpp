#if defined(ENABLE_PROFILING) && ENABLE_PROFILING == 1
    // this is a temporary fix for fmt/color.h using std::copy
    // this issue has already been fix but a version with the bugfix hasn't been released yet
    // clang-format off
    // ↑ without this clang-format tries to put this include beneath the fmt/color.h include which breaks the patch
    #include <algorithm>
    // clang-format on

    #include <fmt/base.h>
    #include <fmt/color.h>
    #include <fmt/core.h>
#endif

#include <iostream>
#include <string>

#include "cortisol_cytokines_simulation.hpp"

int main(int argc, char *argv[]) {
    std::filesystem::path input_path;
    int days = 0;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]).compare("-d") == 0 || std::string(argv[i]).compare("--days") == 0) {
            if (argv[i + 1] != nullptr) {
                days = std::atoi(argv[i + 1]);
                i++;

                if (days <= 0) {
                    std::cerr << "Invalid amount of days: " << days << "\n";
                    exit(3);
                }
            } else {
                std::cerr << "Missing amount of days for parameter " << argv[i] << "\n";
                exit(2);
            }
        } else if (std::string(argv[i]).compare("-i") == 0 || std::string(argv[i]).compare("--input") == 0) {
            if (argv[i + 1] != nullptr) {
                input_path = std::string(argv[i + 1]);
                i++;
            } else {
                std::cerr << "Missing file path for parameter " << argv[i] << "\n";
                exit(2);
            }
        } else {
            std::cerr << "Unknown parameter name: " << argv[i] << "\n";
            exit(1);
        }
    }

    CortisolCytokinesSimulation cortisol_cytokines_simulation;

    if (days > 0) {
        cortisol_cytokines_simulation.setDays(days);
    }

    if (!input_path.empty()) {
        cortisol_cytokines_simulation.setInputPath(input_path);
    }

    cortisol_cytokines_simulation.startSimulation();

    return 0;
}
