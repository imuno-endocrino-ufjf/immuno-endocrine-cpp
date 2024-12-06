#import "cortisol_cytokines.hpp"

#include <matplot/freestanding/plot.h>

#include <cmath>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

CortisolCytokines::CortisolCytokines() {
    setDefaultParameters();
}

CortisolCytokines::CortisolCytokines(std::filesystem::path input_path) {
    setParameters(input_path);
}

void CortisolCytokines::setParameters(std::filesystem::path input_path) {
    try {
        std::ifstream file_stream(input_path);
        auto json = nlohmann::json::parse(file_stream);

        k_6 = json.at("k_6");
        k_6m = json.at("k_6M");
        k_6tnf = json.at("k_6TNF");
        k_8 = json.at("k_8");
        k_8m = json.at("k_8M");
        k_8tnf = json.at("k_8TNF");
        k_10 = json.at("k_10");
        k_10m = json.at("k_10M");
        k_tnf = json.at("k_TNF");
        k_tnfm = json.at("k_TNFM");
        k_mtnf = json.at("k_MTNF");
        q_il6 = json.at("q_IL6");
        q_il8 = json.at("q_IL8");
        q_il10 = json.at("q_IL10");
        q_tnf = json.at("q_TNF");
        n_106 = json.at("n_106");
        n_610 = json.at("n_610");
        n_66 = json.at("n_66");
        n_6tnf = json.at("n_6TNF");
        n_tnf6 = json.at("n_TNF6");
        n_810 = json.at("n_810");
        n_8tnf = json.at("n_8TNF");
        n_m10 = json.at("n_M10");
        n_tnf10 = json.at("n_TNF10");
        n_mtnf = json.at("n_MTNF");
        h_106 = json.at("h_106");
        h_610 = json.at("h_610");
        h_66 = json.at("h_66");
        h_6tnf = json.at("h_6TNF");
        h_tnf6 = json.at("h_TNF6");
        h_810 = json.at("h_810");
        h_8tnf = json.at("h_8TNF");
        h_m10 = json.at("h_M10");
        h_tnf10 = json.at("h_TNF10");
        h_mtnf = json.at("h_MTNF");
        k_106 = json.at("k_106");

        ktc = json.at("ktc");
        kmct = json.at("kmct");
        kmtc = json.at("kmtc");
        kcd = json.at("kcd");
        klt = json.at("klt");
        klt6 = json.at("klt6");
        cmax = json.at("Cmax");

        beta_a = json.at("beta_A");
        k_a = json.at("k_A");
        m_a = json.at("m_A");
        mr_max = json.at("MR_max");
        k_ma = json.at("k_MA");
        k_mr = json.at("k_MR");
        k_m = json.at("k_M");

        gluc = json.at("gluc");
    } catch (const nlohmann::json::parse_error &exception) {
        std::cerr << "Error reading from file, using default values.\n";
        setDefaultParameters();
    } catch (const nlohmann::json::basic_json::out_of_range &exception) {
        std::cerr << "Error reading attribute from file, using default values.\n";
        setDefaultParameters();
    }
}

void CortisolCytokines::setDefaultParameters() {
    k_6 = 4.64;
    k_6m = 0.01;
    k_6tnf = 0.81;
    k_8 = 0.464;
    k_8m = 0.056;
    k_8tnf = 0.56;
    k_10 = 1.1;
    k_10m = 0.19;
    k_tnf = 200;
    k_tnfm = 1.5;
    k_mtnf = 8.65;
    q_il6 = 0.6;
    q_il8 = 0.2;
    q_il10 = 0.15;
    q_tnf = 0.14;
    n_106 = 560;
    n_610 = 34.8;
    n_66 = 560;
    n_6tnf = 185;
    n_tnf6 = 560;
    n_810 = 17.4;
    n_8tnf = 185;
    n_m10 = 4.35;
    n_tnf10 = 17.4;
    n_mtnf = 0.1;
    h_106 = 3.68;
    h_610 = 4;
    h_66 = 1;
    h_6tnf = 2;
    h_tnf6 = 2;
    h_810 = 1.5;
    h_8tnf = 3;
    h_m10 = 0.3;
    h_tnf10 = 3;
    h_mtnf = 3.16;
    k_106 = 0.0191;

    ktc = 3.43;
    kmct = 8.69;
    kmtc = 2.78;
    kcd = 1.55;
    klt = 3.35;
    klt6 = 1.0;
    cmax = 3;

    beta_a = 0.02;
    k_a = 50.0;
    m_a = 0.9;
    mr_max = 5;
    k_ma = 2.51;
    k_mr = 6;
    k_m = 1.414;

    gluc = 1;
};

void CortisolCytokines::printParameters() {
    std::cout << "k_6 = " << k_6 << "\nk_6m = " << k_6m << "\nk_6tnf = " << k_6tnf << "\nk_8 = " << k_8 << "\nk_8m = " << k_8m << "\nk_8tnf = " << k_8tnf << "\nk_10 = " << k_10 << "\nk_10m = " << k_10m << "\nk_tnf = " << k_tnf << "\nk_tnfm = " << k_tnfm << "\nk_mtnf = " << k_mtnf << "\nq_il6 = " << q_il6 << "\nq_il8 = " << q_il8 << "\nq_il10 = " << q_il10 << "\nq_tnf = " << q_tnf << "\nn_106 = " << n_106 << "\nn_610 = " << n_610 << "\nn_66 = " << n_66 << "\nn_6tnf = " << n_6tnf << "\nn_tnf6 = " << n_tnf6 << "\nn_810 = " << n_810 << "\nn_8tnf = " << n_8tnf << "\nn_m10 = " << n_m10 << "\nn_tnf10 = " << n_tnf10 << "\nn_mtnf = " << n_mtnf << "\nh_106 = " << h_106 << "\nh_610 = " << h_610 << "\nh_66 = " << h_66 << "\nh_6tnf = " << h_6tnf << "\nh_tnf6 = " << h_tnf6 << "\nh_810 = " << h_810 << "\nh_8tnf = " << h_8tnf << "\nh_m10 = " << h_m10 << "\nh_tnf10 = " << h_tnf10 << "\nh_mtnf = " << h_mtnf << "\nk_106 = " << k_106 << "\nktc = " << ktc << "\nkmct = " << kmct << "\nkmtc = " << kmtc << "\nkcd = " << kcd << "\nklt = " << klt << "\nklt6 = " << klt6 << "\ncmax = " << cmax << "\nbeta_a = " << beta_a << "\nk_a = " << k_a << "\nm_a = " << m_a << "\nmr_max = " << mr_max << "\nk_ma = " << k_ma << "\nk_mr = " << k_mr << "\nk_m = " << k_m << "\ngluc = " << gluc << "\n";
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

    const double DADT = beta_a * A *
                            (1 - (A / k_a)) -
                        m_a * A * MA;

    const double DMADT = (k_m + k_mtnf *
                                    (pow(TNF, h_mtnf) /
                                     (pow(n_mtnf, h_mtnf) + pow(TNF, h_mtnf))
                                    ) *
                                    (pow(n_m10, h_m10) /
                                     (pow(n_m10, h_m10) + pow(IL10, h_m10))
                                    )
                         ) * MR *
                             A -
                         k_ma * MA;

    const double DMRDT = -(k_m + k_mtnf *
                                     (pow(TNF, h_mtnf) /
                                      (pow(n_mtnf, h_mtnf) + pow(TNF, h_mtnf))
                                     ) *
                                     (pow(n_m10, h_m10) /
                                      (pow(n_m10, h_m10) + pow(IL10, h_m10))
                                     )
                         ) * MR *
                             A +
                         k_mr * MR * (1 - MR / mr_max);

    const double DIL10DT = (k_10m + k_106 *
                                        (pow(IL6, h_106) /
                                         (pow(n_106, h_106) + pow(IL6, h_106))
                                        )
                           ) * MA -
                           k_10 * (IL10 - q_il10);

    const double DIL6DT = (k_6m + k_6tnf *
                                      (pow(TNF, h_6tnf) /
                                       (pow(n_6tnf, h_6tnf) + pow(TNF, h_6tnf))
                                      ) *
                                      (pow(n_66, h_66) /
                                       (pow(n_66, h_66) + pow(IL6, h_66))
                                      ) *
                                      (pow(n_610, h_610) /
                                       (pow(n_610, h_610) + pow(IL10, n_610))
                                      )
                          ) * MA -
                          klt6 * COR *
                              (1 - COR / (COR + kmct)) -
                          k_6 * (IL6 - q_il6);

    const double DIL8DT = (k_8m + k_8tnf *
                                      (pow(TNF, h_8tnf) /
                                       (pow(TNF, h_8tnf) + pow(n_8tnf, h_8tnf))
                                      ) *
                                      (pow(n_810, h_610) /
                                           (pow(n_810, h_810)) +
                                       pow(IL10, h_810)
                                      )
                          ) * MA -
                          k_8 * (IL8 - q_il8);

    const double DTNFDT = (k_tnfm *
                           (pow(n_tnf6, h_tnf6) /
                            (pow(n_tnf6, h_tnf6) + pow(IL6, h_tnf6))
                           ) *
                           (pow(n_tnf10, h_tnf10) /
                            (pow(n_tnf10, h_tnf10) + pow(IL10, h_tnf10))
                           )
                          ) * MA -
                          klt * COR *
                              (1 - COR / (COR + kmct)) -
                          k_tnf * (TNF - q_tnf);

    const double DCORDT = ktc *
                              (TNF / (TNF + kmtc)) *
                              (cmax - COR) * gluc -
                          kcd * COR;

    dxdt[0] = DADT;
    dxdt[1] = DMADT;
    dxdt[2] = DMRDT;
    dxdt[3] = DIL10DT;
    dxdt[4] = DIL6DT;
    dxdt[5] = DIL8DT;
    dxdt[6] = DTNFDT;
    dxdt[7] = DCORDT;
}

void CortisolCytokines::plotResults(const std::vector<std::vector<double>> &states, const std::vector<double> &times, int current_loop) {
    const std::array<std::string, 8> FILE_NAMES = {"antigen", "active_macrophage", "resting_macrophage", "il10", "il6", "il8", "tnf", "cortisol"};
    std::array<std::vector<double>, 8> separated_states;

    for (const auto &state : states) {
        for (int i = 0; i < 8; i++) {
            separated_states[i].push_back(state[i]);
        }
    }

    for (int i = 0; i < 8; i++) {
        auto figure = matplot::plot(times, separated_states[i]);

        const std::filesystem::path FILE_PATH = "output/" + FILE_NAMES[i] + "_" + std::to_string(current_loop) + ".svg";
        matplot::save(FILE_PATH);
    }
};
