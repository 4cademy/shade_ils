//
// Created by Marcel Beyer on 15.01.2024.
//

#ifndef SHADE_ILS_SHADE_H
#define SHADE_ILS_SHADE_H

#include "individual.h"

Individual SHADE(std::vector <std::vector<float>>& population, std::vector<float>& fitness, const Individual& current_best, int POPSIZE, int DIM, float MIN, float MAX, int FUNCTION_NO, int FUNCTION_EVALS);

#endif //SHADE_ILS_SHADE_H
