#include "cortisol_cytokines.hpp"

#include <fmt/ranges.h>
#include <matplot/matplot.h>

#include <cmath>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

#include "utilities.hpp"

#ifndef NDEBUG
    #include <fmt/chrono.h>
    #include <fmt/color.h>

    #include <chrono>
#endif

void CortisolCytokines::setParameters(const nlohmann::basic_json<> &input_path) {
    CortisolCytokines::values.setValues(input_path);
}

void CortisolCytokines::setDefaultParameters() {
    CortisolCytokines::values.setDefaultValues();
}

void CortisolCytokines::operator()(const std::vector<double> &x, std::vector<double> &dxdt, const double T) const {
    const double A = x[0];
    const double MA = x[1];
    const double MR = x[2];
    const double IL10 = x[3];
    const double IL6 = x[4];
    const double IL8 = x[5];
    const double TNF = x[6];
    const double COR = x[7];

    const double DADT = CortisolCytokines::values.beta_a * A *
                            (1 - (A / CortisolCytokines::values.k_a)) -
                        CortisolCytokines::values.m_a * A * MA;

    const double DMADT = (CortisolCytokines::values.k_m + CortisolCytokines::values.k_mtnf *
                                                              (pow(TNF, CortisolCytokines::values.h_mtnf) /
                                                               (pow(CortisolCytokines::values.n_mtnf, CortisolCytokines::values.h_mtnf) + pow(TNF, CortisolCytokines::values.h_mtnf))
                                                              ) *
                                                              (pow(CortisolCytokines::values.n_m10, CortisolCytokines::values.h_m10) /
                                                               (pow(CortisolCytokines::values.n_m10, CortisolCytokines::values.h_m10) + pow(IL10, CortisolCytokines::values.h_m10))
                                                              )
                         ) * MR *
                             A -
                         CortisolCytokines::values.k_ma * MA;

    const double DMRDT = -(CortisolCytokines::values.k_m + CortisolCytokines::values.k_mtnf *
                                                               (pow(TNF, CortisolCytokines::values.h_mtnf) /
                                                                (pow(CortisolCytokines::values.n_mtnf, CortisolCytokines::values.h_mtnf) + pow(TNF, CortisolCytokines::values.h_mtnf))
                                                               ) *
                                                               (pow(CortisolCytokines::values.n_m10, CortisolCytokines::values.h_m10) /
                                                                (pow(CortisolCytokines::values.n_m10, CortisolCytokines::values.h_m10) + pow(IL10, CortisolCytokines::values.h_m10))
                                                               )
                         ) * MR *
                             A +
                         CortisolCytokines::values.k_mr * MR * (1 - MR / CortisolCytokines::values.mr_max);

    const double DIL10DT = (CortisolCytokines::values.k_10m + CortisolCytokines::values.k_106 *
                                                                  (pow(IL6, CortisolCytokines::values.h_106) /
                                                                   (pow(CortisolCytokines::values.n_106, CortisolCytokines::values.h_106) + pow(IL6, CortisolCytokines::values.h_106))
                                                                  )
                           ) * MA -
                           CortisolCytokines::values.k_10 * (IL10 - CortisolCytokines::values.q_il10);

    const double DIL6DT = (CortisolCytokines::values.k_6m + CortisolCytokines::values.k_6tnf *
                                                                (pow(TNF, CortisolCytokines::values.h_6tnf) /
                                                                 (pow(CortisolCytokines::values.n_6tnf, CortisolCytokines::values.h_6tnf) + pow(TNF, CortisolCytokines::values.h_6tnf))
                                                                ) *
                                                                (pow(CortisolCytokines::values.n_66, CortisolCytokines::values.h_66) /
                                                                 (pow(CortisolCytokines::values.n_66, CortisolCytokines::values.h_66) + pow(IL6, CortisolCytokines::values.h_66))
                                                                ) *
                                                                (pow(CortisolCytokines::values.n_610, CortisolCytokines::values.h_610) /
                                                                 (pow(CortisolCytokines::values.n_610, CortisolCytokines::values.h_610) + pow(IL10, CortisolCytokines::values.n_610))
                                                                )
                          ) * MA -
                          CortisolCytokines::values.klt6 * COR *
                              (1 - COR / (COR + CortisolCytokines::values.kmct)) -
                          CortisolCytokines::values.k_6 * (IL6 - CortisolCytokines::values.q_il6);

    const double DIL8DT = (CortisolCytokines::values.k_8m + CortisolCytokines::values.k_8tnf *
                                                                (pow(TNF, CortisolCytokines::values.h_8tnf) /
                                                                 (pow(TNF, CortisolCytokines::values.h_8tnf) + pow(CortisolCytokines::values.n_8tnf, CortisolCytokines::values.h_8tnf))
                                                                ) *
                                                                (pow(CortisolCytokines::values.n_810, CortisolCytokines::values.h_610) /
                                                                     (pow(CortisolCytokines::values.n_810, CortisolCytokines::values.h_810)) +
                                                                 pow(IL10, CortisolCytokines::values.h_810)
                                                                )
                          ) * MA -
                          CortisolCytokines::values.k_8 * (IL8 - CortisolCytokines::values.q_il8);

    const double DTNFDT = (CortisolCytokines::values.k_tnfm *
                           (pow(CortisolCytokines::values.n_tnf6, CortisolCytokines::values.h_tnf6) /
                            (pow(CortisolCytokines::values.n_tnf6, CortisolCytokines::values.h_tnf6) + pow(IL6, CortisolCytokines::values.h_tnf6))
                           ) *
                           (pow(CortisolCytokines::values.n_tnf10, CortisolCytokines::values.h_tnf10) /
                            (pow(CortisolCytokines::values.n_tnf10, CortisolCytokines::values.h_tnf10) + pow(IL10, CortisolCytokines::values.h_tnf10))
                           )
                          ) * MA -
                          CortisolCytokines::values.klt * COR *
                              (1 - COR / (COR + CortisolCytokines::values.kmct)) -
                          CortisolCytokines::values.k_tnf * (TNF - CortisolCytokines::values.q_tnf);

    const double DCORDT = CortisolCytokines::values.ktc *
                              (TNF / (TNF + CortisolCytokines::values.kmtc)) *
                              (CortisolCytokines::values.cmax - COR) * Utilities::findClosestValue<double>(CortisolCytokines::values.gluc, T - int(T)) -
                          CortisolCytokines::values.kcd * COR;

    dxdt[0] = DADT;
    dxdt[1] = DMADT;
    dxdt[2] = DMRDT;
    dxdt[3] = DIL10DT;
    dxdt[4] = DIL6DT;
    dxdt[5] = DIL8DT;
    dxdt[6] = DTNFDT;
    dxdt[7] = DCORDT;
}

void CortisolCytokines::plotResults(const std::vector<std::vector<double>> &states, const std::vector<double> &times) {
    const std::array<std::string, 8> FILE_NAMES = {"antigen", "active_macrophage", "resting_macrophage", "il10", "il6", "il8", "tnf", "cortisol"};
    std::array<std::vector<double>, 8> separated_states;

    for (const auto &state : states) {
        for (int i = 0; i < 8; i++) {
            separated_states[i].push_back(state[i]);
        }
    }

#ifndef NDEBUG
    auto previous_plot_time = std::chrono::high_resolution_clock::now();
#endif

    for (int i = 0; i < 8; i++) {
        auto figure = matplot::figure(true);
        auto axes = figure->current_axes();
        axes->plot(times, separated_states[i]);

        const std::filesystem::path FILE_PATH = "output/" + FILE_NAMES[i] + ".png";
        figure->save(FILE_PATH.string());

#ifndef NDEBUG
        auto current_plot_time = std::chrono::high_resolution_clock::now();
        auto current_plot_duration = std::chrono::duration_cast<std::chrono::microseconds>(current_plot_time - previous_plot_time);

        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->{} plotting done. Plotting duration: {} ({})\n", FILE_NAMES[i], current_plot_duration, std::chrono::duration_cast<std::chrono::seconds>(current_plot_duration));

        previous_plot_time = current_plot_time;
#endif
    }
};
