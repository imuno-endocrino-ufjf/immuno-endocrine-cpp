#ifndef __CORTISOL_CYTOKINES_HPP__
#define __CORTISOL_CYTOKINES_HPP__

#include <filesystem>
#include <vector>

class CortisolCytokines {
    private:
        double k_6;
        double k_6m;
        double k_6tnf;
        double k_8;
        double k_8m;
        double k_8tnf;
        double k_10;
        double k_10m;
        double k_tnf;
        double k_tnfm;
        double k_mtnf;
        double q_il6;
        double q_il8;
        double q_il10;
        double q_tnf;
        double n_106;
        double n_610;
        double n_66;
        double n_6tnf;
        double n_tnf6;
        double n_810;
        double n_8tnf;
        double n_m10;
        double n_tnf10;
        double n_mtnf;
        double h_106;
        double h_610;
        double h_66;
        double h_6tnf;
        double h_tnf6;
        double h_810;
        double h_8tnf;
        double h_m10;
        double h_tnf10;
        double h_mtnf;
        double k_106;

        double ktc;
        double kmct;
        double kmtc;
        double kcd;
        double klt;
        double klt6;
        double cmax;

        double beta_a;
        double k_a;
        double m_a;
        double mr_max;
        double k_ma;
        double k_mr;
        double k_m;

        double gluc;

    public:
        CortisolCytokines();
        CortisolCytokines(std::filesystem::path input_path);
        void setParameters(std::filesystem::path input_path);
        void setDefaultParameters();
        void printParameters();
        void operator()(const std::vector<double> &x, std::vector<double> &dxdt, const double T) const;
        static void plotResults(const std::vector<std::vector<double>> &states, const std::vector<double> &times, int current_loop);
};

#endif
