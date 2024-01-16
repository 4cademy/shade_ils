//
// Created by Marcel Beyer on 12.01.2024.
//

#include <iostream>
#include <vector>

// local includes
#include "shade_ils.h"
#include "population_utils.h"
#include "ls.h"
#include "shade.h"
#include "objective_functions.h"
#include "individual.h"

// SHADE-ILS algorithm
void SHADE_ILS(const int POPSIZE, const int DIM, const float MIN, const float MAX, const int FUNCTION_NO, const int FUNCTION_EVALS) {
    // Initialization
    std::vector<std::vector<float>> population = initialize_population(POPSIZE, DIM, MIN, MAX);
    std::vector<float> fitness(POPSIZE);
    std::vector<float> initial_solution(DIM, (MAX+MIN)/2.0f);
    float initial_fitness = objective_function_no(initial_solution, DIM, FUNCTION_NO);
    Individual current_best{initial_solution, initial_fitness};
    current_best = LS(current_best, DIM, MIN, MAX, FUNCTION_NO, 25'000);
    int fe = 25'000;
    std::cout << "Initial best solution: " << current_best.fitness << std::endl;


    // Main loop
    for (fe; fe < FUNCTION_EVALS; ++fe) {
        current_best = SHADE(population, fitness, current_best, POPSIZE, DIM, MIN, MAX, FUNCTION_NO, 25'000);
        fe += 25'000;
        std::cout << fe << " SHADE: " << current_best.fitness << std::endl;

        current_best = LS(current_best, DIM, MIN, MAX, FUNCTION_NO, 25'000);
        fe += 25'000;
        std::cout << fe << " LS: " << current_best.fitness << std::endl;
    }

    for (int i = 0; i < POPSIZE; ++i) {
        std::cout << "Individual " << i << ": " << fitness[i] << std::endl;
    }

    std::cout << "SHADE_ILS" << std::endl;
}