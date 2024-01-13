//
// Created by Marcel Beyer on 13.01.2024.
//

//local includes
#include <vector>
#include "ls.h"
#include "mts_ls1.h"
#include "l_bfgs_b.h"
#include "objective_functions.h"

struct Individual {
    std::vector<float> solution{};
    float fitness{};
};

float ls_prob = 0.5f;

Individual LS(const std::vector<float>& solution, int DIM, int MIN, int MAX, int FUNCTION_NO, int FUNCTION_EVALS) {
    Individual individual;
    individual.solution = solution;
    individual.fitness = objective_function_no(solution, DIM, FUNCTION_NO);

    // Local search here

    return individual;
}