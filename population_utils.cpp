//
// Created by Marcel Beyer on 12.01.2024.
//

#include <vector>
#include <cstdlib>
#include "population_utils.h"
#include "objective_functions.h"

std::vector<std::vector<float>> initialize_population(const int POPSIZE, const int DIM, const float MIN, const float MAX) {
    std::vector<std::vector<float>> population(POPSIZE, std::vector<float>(DIM));

    // Randomly initialize individuals in the population
    for (int i = 0; i < POPSIZE; ++i) {
        for (int j = 0; j < DIM; ++j) {
            population[i][j] = (rand() / (RAND_MAX + 1.0f)) * (MAX - MIN) + MIN;
        }
    }

    return population;
}

void update_fitness(const std::vector<std::vector<float>>& population, std::vector<float>& fitness, const int POPSIZE, const int DIM, const int FUNCTION_NO) {
    // Evaluate fitness of individuals in the population
    for (int i = 0; i < POPSIZE; ++i) {
        fitness[i] = objective_function_no(population[i], DIM, FUNCTION_NO);
    }
}