#ifndef __CORTISOL_CYTOKINES_HPP__
#define __CORTISOL_CYTOKINES_HPP__

#include <nlohmann/json.hpp>
#include <vector>

#include "cortisol_cytokines_values.hpp"
#include "utilities.hpp"

class CortisolCytokines {
    private:
        // boost's odeint integration function constantly destroys and recreates the object,
        // as such, storing the parameters as class members will frequently cause them to be
        // removed and added to memory
        // while this isn't an issue for most members the glucose parameter being a map causes
        // it to reinsert and re-sort the binary tree it uses for it's iternal structure 100s of
        // times for each day of the simulation causing a huge processing overhead
        // to prevent this we store them on a separate class that is statically defined inline
        // and thus isn't removed from memory
        static inline CortisolCytokinesValues values = CortisolCytokinesValues();


        static inline Utilities::GetClosestValue<double> get_closest_value = Utilities::GetClosestValue<double>();

    public:
        inline CortisolCytokines(){};
        void setParameters(const nlohmann::basic_json<> &input_path);
        void setDefaultParameters();
        void operator()(const std::vector<double> &x, std::vector<double> &dxdt, const double T) const;
        static void plotResults(const std::vector<std::vector<double>> &states, const std::vector<double> &times);
};

#endif
