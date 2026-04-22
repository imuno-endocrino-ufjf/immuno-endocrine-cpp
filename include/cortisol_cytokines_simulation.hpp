#ifndef __CORTISOL_CYTOKINES_SIMULATION_HPP__
#define __CORTISOL_CYTOKINES_SIMULATION_HPP__

// #include "simulation.hpp"

#include <filesystem>

class CortisolCytokinesSimulation {
    private:
        int days;
        std::filesystem::path input_path;
        bool plot_all_values;
        bool plot_daily_averages;
        bool csv;

    public:
        CortisolCytokinesSimulation(std::filesystem::path input_path = std::filesystem::path(), int days = 36500, bool plot_all_values = true, bool plot_daily_averages = true, bool csv = true);
        void setDays(int days);
        void setInputPath(std::filesystem::path input_path);
        void setPlotAllValues(bool plot_all_values);
        void setPlotDailyAverages(bool plot_daily_averages);
        void setCsv(bool csv);
        void startSimulation() const;
};

#endif
