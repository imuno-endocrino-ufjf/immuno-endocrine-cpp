#ifndef __CORTISOL_CYTOKINES_SIMULATION_HPP__
#define __CORTISOL_CYTOKINES_SIMULATION_HPP__

// #include "simulation.hpp"

#include <filesystem>

class CortisolCytokinesSimulation {
    private:
        int days;
        std::filesystem::path input_path;
        bool plot;

    public:
        CortisolCytokinesSimulation(std::filesystem::path input_path = std::filesystem::path(), int days = 36500, bool plot = true);
        void setDays(int days);
        void setInputPath(std::filesystem::path input_path);
        void setPlot(bool plot);
        void startSimulation();
};

#endif
