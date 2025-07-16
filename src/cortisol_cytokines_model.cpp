#include "cortisol_cytokines_model.hpp"

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

void CortisolCytokinesModel::setParameters(const nlohmann::basic_json<> &json_file) {
    CortisolCytokinesModel::values.setValues(json_file);
    CortisolCytokinesModel::get_closest_value.setValues(CortisolCytokinesModel::values.gluc);
}

void CortisolCytokinesModel::setDefaultParameters() {
    CortisolCytokinesModel::values.setDefaultValues();
    CortisolCytokinesModel::get_closest_value.setValues(CortisolCytokinesModel::values.gluc);
}

void CortisolCytokinesModel::operator()(const std::vector<double> &x, std::vector<double> &dxdt, const double T) const {
    const double A = x[0];
    const double MA = x[1];
    const double MR = x[2];
    const double IL10 = x[3];
    const double IL6 = x[4];
    const double IL8 = x[5];
    const double TNF = x[6];
    const double COR = x[7];

    const double DADT = CortisolCytokinesModel::values.beta_a * A *
                            (1 - (A / CortisolCytokinesModel::values.k_a)) -
                        CortisolCytokinesModel::values.m_a * A * MA;

    const double DMADT = (CortisolCytokinesModel::values.k_m + CortisolCytokinesModel::values.k_mtnf *
                                                                   (pow(TNF, CortisolCytokinesModel::values.h_mtnf) /
                                                                    (pow(CortisolCytokinesModel::values.n_mtnf, CortisolCytokinesModel::values.h_mtnf) + pow(TNF, CortisolCytokinesModel::values.h_mtnf))
                                                                   ) *
                                                                   (pow(CortisolCytokinesModel::values.n_m10, CortisolCytokinesModel::values.h_m10) /
                                                                    (pow(CortisolCytokinesModel::values.n_m10, CortisolCytokinesModel::values.h_m10) + pow(IL10, CortisolCytokinesModel::values.h_m10))
                                                                   )
                         ) * MR *
                             A -
                         CortisolCytokinesModel::values.k_ma * MA;

    const double DMRDT = -(CortisolCytokinesModel::values.k_m + CortisolCytokinesModel::values.k_mtnf *
                                                                    (pow(TNF, CortisolCytokinesModel::values.h_mtnf) /
                                                                     (pow(CortisolCytokinesModel::values.n_mtnf, CortisolCytokinesModel::values.h_mtnf) + pow(TNF, CortisolCytokinesModel::values.h_mtnf))
                                                                    ) *
                                                                    (pow(CortisolCytokinesModel::values.n_m10, CortisolCytokinesModel::values.h_m10) /
                                                                     (pow(CortisolCytokinesModel::values.n_m10, CortisolCytokinesModel::values.h_m10) + pow(IL10, CortisolCytokinesModel::values.h_m10))
                                                                    )
                         ) * MR *
                             A +
                         CortisolCytokinesModel::values.k_mr * MR * (1 - MR / CortisolCytokinesModel::values.mr_max);

    const double DIL10DT = (CortisolCytokinesModel::values.k_10m + CortisolCytokinesModel::values.k_106 *
                                                                       (pow(IL6, CortisolCytokinesModel::values.h_106) /
                                                                        (pow(CortisolCytokinesModel::values.n_106, CortisolCytokinesModel::values.h_106) + pow(IL6, CortisolCytokinesModel::values.h_106))
                                                                       )
                           ) * MA -
                           CortisolCytokinesModel::values.k_10 * (IL10 - CortisolCytokinesModel::values.q_il10);

    const double DIL6DT = (CortisolCytokinesModel::values.k_6m + CortisolCytokinesModel::values.k_6tnf *
                                                                     (pow(TNF, CortisolCytokinesModel::values.h_6tnf) /
                                                                      (pow(CortisolCytokinesModel::values.n_6tnf, CortisolCytokinesModel::values.h_6tnf) + pow(TNF, CortisolCytokinesModel::values.h_6tnf))
                                                                     ) *
                                                                     (pow(CortisolCytokinesModel::values.n_66, CortisolCytokinesModel::values.h_66) /
                                                                      (pow(CortisolCytokinesModel::values.n_66, CortisolCytokinesModel::values.h_66) + pow(IL6, CortisolCytokinesModel::values.h_66))
                                                                     ) *
                                                                     (pow(CortisolCytokinesModel::values.n_610, CortisolCytokinesModel::values.h_610) /
                                                                      (pow(CortisolCytokinesModel::values.n_610, CortisolCytokinesModel::values.h_610) + pow(IL10, CortisolCytokinesModel::values.n_610))
                                                                     )
                          ) * MA -
                          CortisolCytokinesModel::values.klt6 * COR *
                              (1 - COR / (COR + CortisolCytokinesModel::values.kmct)) -
                          CortisolCytokinesModel::values.k_6 * (IL6 - CortisolCytokinesModel::values.q_il6);

    const double DIL8DT = (CortisolCytokinesModel::values.k_8m + CortisolCytokinesModel::values.k_8tnf *
                                                                     (pow(TNF, CortisolCytokinesModel::values.h_8tnf) /
                                                                      (pow(TNF, CortisolCytokinesModel::values.h_8tnf) + pow(CortisolCytokinesModel::values.n_8tnf, CortisolCytokinesModel::values.h_8tnf))
                                                                     ) *
                                                                     (pow(CortisolCytokinesModel::values.n_810, CortisolCytokinesModel::values.h_610) /
                                                                          (pow(CortisolCytokinesModel::values.n_810, CortisolCytokinesModel::values.h_810)) +
                                                                      pow(IL10, CortisolCytokinesModel::values.h_810)
                                                                     )
                          ) * MA -
                          CortisolCytokinesModel::values.k_8 * (IL8 - CortisolCytokinesModel::values.q_il8);

    const double DTNFDT = (CortisolCytokinesModel::values.k_tnfm *
                           (pow(CortisolCytokinesModel::values.n_tnf6, CortisolCytokinesModel::values.h_tnf6) /
                            (pow(CortisolCytokinesModel::values.n_tnf6, CortisolCytokinesModel::values.h_tnf6) + pow(IL6, CortisolCytokinesModel::values.h_tnf6))
                           ) *
                           (pow(CortisolCytokinesModel::values.n_tnf10, CortisolCytokinesModel::values.h_tnf10) /
                            (pow(CortisolCytokinesModel::values.n_tnf10, CortisolCytokinesModel::values.h_tnf10) + pow(IL10, CortisolCytokinesModel::values.h_tnf10))
                           )
                          ) * MA -
                          CortisolCytokinesModel::values.klt * COR *
                              (1 - COR / (COR + CortisolCytokinesModel::values.kmct)) -
                          CortisolCytokinesModel::values.k_tnf * (TNF - CortisolCytokinesModel::values.q_tnf);

    const double DCORDT = CortisolCytokinesModel::values.ktc *
                              (TNF / (TNF + CortisolCytokinesModel::values.kmtc)) *
                              (CortisolCytokinesModel::values.cmax - COR) * CortisolCytokinesModel::get_closest_value.find(T - int(T)) -
                          CortisolCytokinesModel::values.kcd * COR;

    dxdt[0] = DADT;
    dxdt[1] = DMADT;
    dxdt[2] = DMRDT;
    dxdt[3] = DIL10DT;
    dxdt[4] = DIL6DT;
    dxdt[5] = DIL8DT;
    dxdt[6] = DTNFDT;
    dxdt[7] = DCORDT;
}

void CortisolCytokinesModel::plotResults(const std::vector<std::vector<double>> &states, const std::vector<double> &times) {
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

nlohmann::json CortisolCytokinesModel::toJson(const std::vector<double>& initial_conditions) const {
    nlohmann::json j;

    // Serializa condições iniciais
    j["initial_conditions"] = {
        {"antigens", initial_conditions[0]},
        {"active_macrophages", initial_conditions[1]},
        {"resting_macrophages", initial_conditions[2]},
        {"il-10", initial_conditions[3]},
        {"il-6", initial_conditions[4]},
        {"il-8", initial_conditions[5]},
        {"tnf-alpha", initial_conditions[6]},
        {"cortisol", initial_conditions[7]}
    };

    // Serializa parâmetros
    j["parameters"] = {
        {"k_6", values.k_6}, {"k_6M", values.k_6m}, {"k_6TNF", values.k_6tnf},
        {"k_8", values.k_8}, {"k_8M", values.k_8m}, {"k_8TNF", values.k_8tnf},
        {"k_10", values.k_10}, {"k_10M", values.k_10m}, {"k_TNF", values.k_tnf},
        {"k_TNFM", values.k_tnfm}, {"k_MTNF", values.k_mtnf},
        {"q_IL6", values.q_il6}, {"q_IL8", values.q_il8}, {"q_IL10", values.q_il10}, {"q_TNF", values.q_tnf},
        {"n_106", values.n_106}, {"n_610", values.n_610}, {"n_66", values.n_66}, {"n_6TNF", values.n_6tnf},
        {"n_TNF6", values.n_tnf6}, {"n_810", values.n_810}, {"n_8TNF", values.n_8tnf},
        {"n_M10", values.n_m10}, {"n_TNF10", values.n_tnf10}, {"n_MTNF", values.n_mtnf},
        {"h_106", values.h_106}, {"h_610", values.h_610}, {"h_66", values.h_66}, {"h_6TNF", values.h_6tnf},
        {"h_TNF6", values.h_tnf6}, {"h_810", values.h_810}, {"h_8TNF", values.h_8tnf},
        {"h_M10", values.h_m10}, {"h_TNF10", values.h_tnf10}, {"h_MTNF", values.h_mtnf},
        {"k_106", values.k_106},
        {"ktc", values.ktc}, {"kmct", values.kmct}, {"kmtc", values.kmtc}, {"kcd", values.kcd},
        {"klt", values.klt}, {"klt6", values.klt6}, {"Cmax", values.cmax},
        {"beta_A", values.beta_a}, {"k_A", values.k_a}, {"m_A", values.m_a},
        {"MR_max", values.mr_max}, {"k_MA", values.k_ma}, {"k_MR", values.k_mr}, {"k_M", values.k_m},
        {"glucose", values.gluc} // É um std::map<double, double>
    };

    return j;
}

