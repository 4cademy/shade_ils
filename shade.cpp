//
// Created by Marcel Beyer on 15.01.2024.
//

#include <vector>
#include <random>
#include <thread>
#include <iostream>
#include <algorithm>
#include "shade.h"
#include "individual.h"
#include "objective_functions.h"
#include "population_utils.h"


// create random number generator by seeding with device specific random number and hash of thread id
std::random_device shade_rd;
const std::random_device::result_type seed = shade_rd();
std::mt19937_64 shade_gen(seed + std::hash<std::thread::id>{}(std::this_thread::get_id()));

int H = 100;                                 // size of memory for cr and f
std::vector<float> M_CR(H, 0.5f);   // memory for cr
std::vector<float> M_F(H, 0.5f);    // memory for f
int memory_counter = 0;                      // counter for next memory position to be updated

std::vector<std::vector<float>> archive;     // archive for storing not selected parents

// this function calculates the weighted arithmetic mean of a vector of values
float weighted_arithmetic_mean(const std::vector<float>& values, const std::vector<float>& weights){
    if (values.size() != weights.size()) {
        std::cout << "Error: values and weights must have the same size to calculate weighted arithmetic mean." << std::endl;
        return 0;
    } else {
        size_t length = values.size();
        float total_weight = 0;
        for(auto& weight : weights){
            total_weight += weight;
        }

        float mean = 0;
        for(int i = 0; i < length; i++){
            mean += (values[i] * weights[i]/total_weight);
        }
        return mean;
    }
}

// this function calculates the weighted Lehmer mean of a vector of values
float weighted_lehmer_mean(const std::vector<float>& values, const std::vector<float>& weights, int power){
    if (values.size() != weights.size()) {
        std::cout << "Error: values and weights must have the same size to calculate weighted lehmer mean." << std::endl;
        return 0;
    } else {
        size_t length = values.size();
        float dividend = 0;
        float divisor = 0;
        for(int i = 0; i < length; i++){
            dividend += (powf(values[i], (float)power) * weights[i]);
            divisor += (powf(values[i], (float)(power - 1)) * weights[i]);
        }
        float mean = dividend/divisor;
        return mean;
    }
}

std::vector<int> get_indices_sorted_by_fitness(const std::vector<std::vector<float>>& population, const std::vector<float>& fitness) {
    std::vector<int> indices(population.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int i1, int i2) {return fitness[i1] < fitness[i2];});
    return indices;
}

std::vector<float> generate_trial_vector(const std::vector<std::vector<float>>& population, const std::vector<float>& fitness, const Individual& current_best, const int parent_index, const float f, const float cr, const int POPSIZE, const int DIM, const float MIN, const float MAX) {
    std::uniform_int_distribution<int> uni_int(0, POPSIZE - 1);
    std::uniform_real_distribution<float> uni_float(0, 1);
    unsigned j = uni_int(shade_gen);

    // get first donor vector from population
    unsigned donor1_index = uni_int(shade_gen);
    while (donor1_index == parent_index) {
        donor1_index = uni_int(shade_gen);
    }
    std::vector<float> donor1 = population[donor1_index];

    // get second donor vector from population or archive
    uni_int = std::uniform_int_distribution<int>(0, POPSIZE + (int)archive.size() - 1);
    unsigned donor2_index = uni_int(shade_gen);
    while ((donor2_index == donor1_index) || (donor2_index == parent_index)) {
        donor2_index = uni_int(shade_gen);
    }
    std::vector<float> donor2;
    if (donor2_index >= POPSIZE) {
        donor2_index -= POPSIZE;
        donor2 = archive[donor2_index];
    }else{
        donor2 = population[donor2_index];
    }

    //current-to-pbest/1/bin
    // Todo: move sorting out of this function
    std::vector<int> sorted_indices = get_indices_sorted_by_fitness(population, fitness);
    uni_int = std::uniform_int_distribution<int>(0, (int)(POPSIZE * 0.2) - 1);
    int pbest_index = sorted_indices[uni_int(shade_gen)];
    std::vector<float> pbest = population[pbest_index];

    std::vector<float> trial_vector(DIM);
    for (unsigned i = 0; i < DIM; i++) {
        float old_gene = population[parent_index][i];
        float diff = f * ((pbest[i] - old_gene) + (donor1[i] - donor2[i]));
        float new_gene = old_gene + diff;
        if (new_gene < MIN) {
            new_gene = (MIN + new_gene)/2;
        } else if (new_gene > MAX) {
            new_gene = (MAX + new_gene)/2;
        }

        // binomial crossover
        if ((j == i) || (uni_float(shade_gen) < cr)) {
            trial_vector[i] = new_gene;
        } else {
            trial_vector[i] = old_gene;
        }
    }

    return trial_vector;
}

Individual SHADE(std::vector <std::vector<float>>& population, std::vector<float>& fitness, const Individual& current_best, const int POPSIZE, const int DIM, const float MIN, const float MAX, const int FUNCTION_NO, const int FUNCTION_EVALS) {
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

        // put current best into population
        int min_index = get_min_index(fitness, POPSIZE);
        population[min_index] = current_best.solution;
        fitness[min_index] = current_best.fitness;

        // generate trial population
        for (int i = 0; i < POPSIZE; ++i) {
            int ri = uni_int(shade_gen);

            // set distribution parameters
            std::normal_distribution<float> normal_float_dist(M_CR[ri], 0.1);
            std::cauchy_distribution<float> cauchy_dist(M_F[ri], 0.1);

            // generate new evaluation parameters cr and f for each individual
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
            std::vector<float> trial_vector = generate_trial_vector(population, fitness, current_best, i, f, cr, POPSIZE, DIM, MIN, MAX);

            // copy trial vector to trial population
            trial_pop[i] = trial_vector;

            // evaluate trial vector
            trial_fitness[i] = objective_function_no(trial_vector, DIM, FUNCTION_NO);

            // update function evaluation counter every time the objective function is called
            fe++;
        }

        for (int i = 0; i < POPSIZE; ++i) {
            if (trial_fitness[i] <= fitness[i]) {
                if(trial_fitness[i] < fitness[i]) {
                    // add the not chosen parent to archive
                    archive.push_back(population[i]);

                    // temporarily store cr and f for calculating weighted arithmetic mean and weighted Lehmer mean in the next step
                    S_CR.push_back(trial_cr[i]);
                    S_F.push_back(trial_f[i]);
                    delta_fitness.push_back(fabsf( trial_fitness[i] - fitness[i]));
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

        // trim archive to the size of population
        while (archive.size() > POPSIZE) {
            uni_int = std::uniform_int_distribution<int>(0, (int)archive.size() - 1);
            int index = uni_int(shade_gen);
            archive.erase(archive.begin() + index);
        }

        // if better solution was found S_CR and S_F are not empty -> update memories M_CR and M_F with weighted arithmetic mean of S_CR and weighted Lehmer mean of S_F
        if (!S_CR.empty() and !S_F.empty()) {
            float mean_cr = weighted_arithmetic_mean(S_CR, delta_fitness);
            float mean_f = weighted_lehmer_mean(S_F, delta_fitness, 2);
            M_CR[memory_counter] = mean_cr;
            M_F[memory_counter] = mean_f;
            memory_counter++;
            if (memory_counter >= H) {
                memory_counter = 0;
            }
        }
    }

    // find and return the best individual
    int best_index = 0;
    float best_fitness = fitness[0];
    for (int i = 1; i < POPSIZE; ++i) {
        if (fitness[i] < best_fitness) {
            best_fitness = fitness[i];
            best_index = i;
        }
    }

    Individual best_individual;
    best_individual.solution = population[best_index];
    best_individual.fitness = best_fitness;
    return best_individual;

}