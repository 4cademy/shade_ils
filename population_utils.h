//
// Created by Marcel Beyer on 12.01.2024.
//

#ifndef SHADE_ILS_POPULATION_UTILS_H
#define SHADE_ILS_POPULATION_UTILS_H

std::vector<std::vector<float>> initialize_population(int POPSIZE, int DIM, float MIN, float MAX);
void update_fitness(const std::vector<std::vector<float>>& population, std::vector<float>& fitness, int POPSIZE, int DIM, int FUNCTION_NO);
int get_min_index(const std::vector<float>& fitness, int POPSIZE);
int get_max_index(const std::vector<float>& fitness, int POPSIZE);

#endif //SHADE_ILS_POPULATION_UTILS_H
