#ifndef __CORTISOL_CYTOKINES_SIMULATION_HPP__
#define __CORTISOL_CYTOKINES_SIMULATION_HPP__

// #include "simulation.hpp"

#include <filesystem>

class CortisolCytokinesSimulation {
    private:
        int days;
        std::filesystem::path input_path;
        bool plot;
        bool csv;
        std::filesystem::path output_path;

    public:
        CortisolCytokinesSimulation(std::filesystem::path input_path = std::filesystem::path(), int days = 36500, bool plot = true, bool csv = true);
        void setDays(int days);
        void setInputPath(std::filesystem::path input_path);
        void setPlot(bool plot);
        void setCsv(bool csv);
        void startSimulation() const;
        void setOutputPath(const std::filesystem::path& path);
};

#endif
