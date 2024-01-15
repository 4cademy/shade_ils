//
// Created by Marcel Beyer on 13.01.2024.
//

#include <vector>

//local includes
#include "ls.h"
#include "mts_ls1.h"
#include "l_bfgs_b.h"
#include "objective_functions.h"

float ls_prob = 0.5f;

Individual LS(Individual individual, int DIM, float MIN, float MAX, int FUNCTION_NO, int FUNCTION_EVALS) {
    // Local search here
    mts_ls1(DIM, MIN, MAX, FUNCTION_NO, FUNCTION_EVALS, individual.solution);
    individual.fitness = objective_function_no(individual.solution, DIM, FUNCTION_NO);
    return individual;
}