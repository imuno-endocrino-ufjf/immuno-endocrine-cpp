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
        figure->backend()->run_command("unset warnings");
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

void CortisolCytokinesModel::plotDailyAverage(const std::vector<std::vector<double>> &states, const std::vector<double> &times) {
    const std::array<std::string, 8> FILE_NAMES = {"antigen", "active_macrophage", "resting_macrophage", "il10", "il6", "il8", "tnf", "cortisol"};
    std::array<std::vector<double>, 8> separated_states;

#ifndef NDEBUG
    auto start_average_calculation_time = std::chrono::high_resolution_clock::now();

    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->Starting average calculation.\n");
#endif

    int index = 0;
    // - 1 to prevent the last day from being plotted and messing up the graph
    // due to the way the simulation is ran the last day will always only have 1 value,
    // that value tends to be considerably different from the daily average and, thus,
    // causes a spike downwards on the graph of most of the metrics
    while (index < times.size() - 1) {
        int current_index = index + 1;

        std::array<double, 8> sums;
        for (int j = 0; j < sums.size(); j++) {
            sums[j] = states[index][j];
        }

        while (current_index < times.size() && int(times[current_index]) - int(times[index]) < 1) {
            for (int j = 0; j < sums.size(); j++) {
                sums[j] += states[current_index][j];
            }

            current_index++;
        }

        for (int j = 0; j < sums.size(); j++) {
            separated_states[j].push_back(sums[j] / (current_index - index + 1));
        }

        index = current_index;
    }

    std::vector<int> days;
    for (int index = 0; index < separated_states[0].size(); index++) {
        days.push_back(index);
    }

#ifndef NDEBUG
    auto end_average_calculation_time = std::chrono::high_resolution_clock::now();
    auto average_calculation_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_average_calculation_time - start_average_calculation_time);

    fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->Daily average calculation done. Calculation duration: {} ({})\n", average_calculation_duration, std::chrono::duration_cast<std::chrono::seconds>(average_calculation_duration));
#endif

#ifndef NDEBUG
    auto previous_plot_time = std::chrono::high_resolution_clock::now();
#endif

    for (int i = 0; i < 8; i++) {
        auto figure = matplot::figure(true);
        figure->backend()->run_command("unset warnings");
        auto axes = figure->current_axes();
        axes->plot(days, separated_states[i]);

        const std::filesystem::path FILE_PATH = "output/" + FILE_NAMES[i] + "_average" + ".png";
        figure->save(FILE_PATH.string());

#ifndef NDEBUG
        auto current_plot_time = std::chrono::high_resolution_clock::now();
        auto current_plot_duration = std::chrono::duration_cast<std::chrono::microseconds>(current_plot_time - previous_plot_time);

        fmt::print(fg(fmt::color::dark_golden_rod) | fmt::emphasis::bold, "->{} average plotting done. Plotting duration: {} ({})\n", FILE_NAMES[i], current_plot_duration, std::chrono::duration_cast<std::chrono::seconds>(current_plot_duration));

        previous_plot_time = current_plot_time;
#endif
    }
};
