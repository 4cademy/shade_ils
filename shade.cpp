//
// Created by Marcel Beyer on 15.01.2024.
//

#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include "shade.h"
#include "individual.h"
#include "objective_functions.h"

std::random_device shade_rd;
const std::random_device::result_type seed = shade_rd();
std::mt19937_64 shade_gen(seed + std::hash<std::thread::id>{}(std::this_thread::get_id()));

int H = 100;    // history size

std::vector<float> generate_trial_vector(const std::vector<std::vector<float>>& population, const Individual& current_best, const int parent_index, const float f, const float cr, const int POPSIZE, const int DIM, const float MIN, const float MAX) {
    std::uniform_int_distribution<int> uni_int(0, POPSIZE - 1);
    std::uniform_real_distribution<float> uni_float(0, 1);
    unsigned j = uni_int(shade_gen);
    unsigned donor1_index = uni_int(shade_gen);
    unsigned donor2_index = uni_int(shade_gen);

    while (donor1_index == parent_index) {
        donor1_index = uni_int(shade_gen);
    }

    while ((donor2_index == donor1_index) || (donor2_index == parent_index)) {
        donor2_index = uni_int(shade_gen);
    }

    std::vector<float> trial_vector(DIM);
    for (unsigned i = 0; i < DIM; i++) {
        float old_gene = population[parent_index][i];
        float diff = f * ((current_best.solution[i] - old_gene) + (population[donor1_index][i] - population[donor2_index][i]));
        float new_gene = old_gene + diff;
        if (new_gene < MIN) {
            new_gene = (MIN + new_gene)/2;
        } else if (new_gene > MAX) {
            new_gene = (MAX + new_gene)/2;
        }

        if ((j == i) || (uni_float(shade_gen) < cr)) {
            trial_vector[i] = new_gene;
        } else {
            trial_vector[i] = old_gene;
        }
    }

    return trial_vector;
}

void SHADE(std::vector <std::vector<float>>& population, std::vector<float>& fitness, const Individual& current_best, const int POPSIZE, const int DIM, const float MIN, const float MAX, const int FUNCTION_NO, const int FUNCTION_EVALS) {
    std::vector<float> M_CR(H, 0.5f);
    std::vector<float> M_F(H, 0.5f);
    int memory_counter = 0;

    int fe = 0;
    while (fe < FUNCTION_EVALS) {
        std::uniform_int_distribution<int> uni_int(0, H - 1);

        std::uniform_real_distribution<float> uni_float(0, 1);

        std::vector<float> trial_cr(POPSIZE);
        std::vector<float> trial_f(POPSIZE);
        std::vector<std::vector<float>> trial_pop(POPSIZE, std::vector<float>(DIM));
        std::vector<float> trial_fitness(POPSIZE);

        std::vector<float> S_CR;
        std::vector<float> S_F;
        std::vector<float> delta_fitness;

        // generate trial population
        for (int i = 0; i < POPSIZE; ++i) {
            int ri = uni_int(shade_gen);

            std::normal_distribution<float> normal_float_dist(M_CR[ri], 0.1);

            std::cauchy_distribution<float> cauchy_dist(M_F[ri], 0.1);

            float cr = normal_float_dist(shade_gen);
            if (cr < 0) {
                cr = 0;
            } else if (cr > 1) {
                cr = 1;
            }
            trial_cr[i] = cr;

            float f = cauchy_dist(shade_gen);
            while (f < 0) {
                f = cauchy_dist(shade_gen);
            }
            if (f > 1) {
                f = 1;
            }
            trial_f[i] = f;

            // generate trial vector
            std::vector<float> trial_vector = generate_trial_vector(population, current_best, i, f, cr, POPSIZE, DIM, MIN, MAX);

            // evaluate trial vector
            trial_fitness[i] = objective_function_no(trial_vector, DIM, FUNCTION_NO);
        }

        for (int i = 0; i < POPSIZE; ++i) {
            if (trial_fitness[i] <= fitness[i]) {
                if(trial_fitness[i] < fitness[i]) {
                    // ToDo: Move original individual to archive
                    S_CR.push_back(trial_cr[i]);
                    S_F.push_back(trial_f[i]);
                    delta_fitness.push_back(fabsf(fitness[i] - trial_fitness[i]));
                }

                // replace original individual with trial individual
                for (int j = 0; j < DIM; j++) {
                    population[i][j] = trial_pop[i][j];
                }
                fitness[i] = trial_fitness[i];
            } else {
                // original individual stays in population
            }
        }

        // ToDo: Whenever the size of the archive exceeds |A|, randomly selected individuals are deleted so that |A| ≤ |P|

        if (!S_CR.empty() and !S_F.empty()) {
            // ToDo: Update M_CR and M_F
        }
        ++fe;
    }
}