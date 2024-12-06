#ifndef __CORTISOL_CYTOKINES_SIMULATION_HPP__
#define __CORTISOL_CYTOKINES_SIMULATION_HPP__

// #include "simulation.hpp"

#include <filesystem>

class CortisolCytokinesSimulation {
    private:
        int days;
        std::filesystem::path input_path;

    public:
        CortisolCytokinesSimulation(std::filesystem::path input_path = std::filesystem::path(), int days = 36500);
        void setDays(int days);
        void setInputPath(std::filesystem::path input_path);
        void startSimulation();
};

#endif
