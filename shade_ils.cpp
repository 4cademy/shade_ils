//
// Created by Marcel Beyer on 12.01.2024.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <thread>

// local includes
#include "shade_ils.h"
#include "population_utils.h"
#include "ls.h"
#include "shade.h"
#include "objective_functions.h"
#include "individual.h"

// create random number generator by seeding with device specific random number and hash of thread id
std::random_device shade_ils_rd;
const std::random_device::result_type seed = shade_ils_rd();
std::mt19937_64 shade_ils_gen(seed + std::hash<std::thread::id>{}(std::this_thread::get_id()));

void shade_reset(std::vector<std::vector<float>>& population, std::vector<float>& fitness, Individual& current_best, const int POPSIZE, const int DIM, const float MIN, const float MAX) {
    // select random solution from population
    std::uniform_int_distribution<int> uni_int(0, POPSIZE - 1);
    int random_index = uni_int(shade_ils_gen);
    std::vector<float> random_solution = population[random_index];

    // reset current best
    std::normal_distribution<float> normal_float_dist(0, 1);
    for (int i = 0; i < DIM; i++) {
        float rand = normal_float_dist(shade_ils_gen);
        current_best.solution[i] = random_solution[i] + rand * 0.1 * (MAX-MIN);
        // clip to bounds
        current_best.solution[i] = std::min(current_best.solution[i], MAX);
        current_best.solution[i] = std::max(current_best.solution[i], MIN);

        current_best.fitness = objective_function_no(current_best.solution, DIM, 1);
    }

    // reset population
    population = initialize_population(POPSIZE, DIM, MIN, MAX);
    update_fitness(population, fitness, POPSIZE, DIM, 1);

    // ToDo: reset LS parameters
}

// SHADE-ILS algorithm
void SHADE_ILS(const int POPSIZE, const int DIM, const float MIN, const float MAX, const int FUNCTION_NO, const int FUNCTION_EVALS) {
    // Initialization
    std::vector<std::vector<float>> population = initialize_population(POPSIZE, DIM, MIN, MAX);
    std::vector<float> fitness(POPSIZE, HUGE_VALF);
    std::vector<float> initial_solution(DIM, (MAX+MIN)/2.0f);
    float initial_fitness = objective_function_no(initial_solution, DIM, FUNCTION_NO);
    Individual current_best{initial_solution, initial_fitness};
    current_best = LS(current_best, DIM, MIN, MAX, FUNCTION_NO, 25'000);
    int fe = 25'000;
    std::cout << "Initial best solution: " << current_best.fitness << std::endl;

    Individual best_solution{current_best};

    float improvement;
    int reset_counter = 0;
    float previous_best_fitness = current_best.fitness;

    // Main loop
    while (fe < FUNCTION_EVALS) {
        // SHADE
        current_best = SHADE(population, fitness, current_best, POPSIZE, DIM, MIN, MAX, FUNCTION_NO, 25'000);
        fe += 25'000;
        std::cout << fe << " SHADE: " << current_best.fitness << std::endl;

        improvement = (previous_best_fitness - current_best.fitness) / previous_best_fitness;
        previous_best_fitness = current_best.fitness;

        // LS
        current_best = LS(current_best, DIM, MIN, MAX, FUNCTION_NO, 25'000);
        fe += 25'000;
        std::cout << fe << " LS: " << current_best.fitness << std::endl;

        if (current_best.fitness < best_solution.fitness) {
            best_solution = current_best;
        }

        // RESET
        if (improvement < 0.05f) {
            reset_counter++;
        } else {
            reset_counter = 0;
        }

        if (reset_counter >= 3) {
            reset_counter = 0;
            shade_reset(population, fitness, current_best, POPSIZE, DIM, MIN, MAX);
            std::cout << "RESET" << std::endl;
        }


    }

    for (int i = 0; i < POPSIZE; ++i) {
        std::cout << "Individual " << i << ": " << fitness[i] << std::endl;
    }

    std::cout << "SHADE_ILS" << std::endl;
}