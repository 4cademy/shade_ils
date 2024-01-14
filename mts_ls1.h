//
// Created by Marcel Beyer on 13.01.2024.
//

#ifndef SHADE_ILS_MTS_LS1_H
#define SHADE_ILS_MTS_LS1_H

#include "individual.h"

typedef struct{
    std::vector<float> solution;
    float fitness;
    unsigned evals;
} LSresult;

void mts_ls1(int DIM, float MIN, float MAX, int FUNCTION_NO, unsigned maxevals, std::vector<float>& sol);
LSresult mts_ls1_improve_dim(int DIM, float MIN, float MAX, int FUNCTION_NO, std::vector<float>& sol, float best_fitness, unsigned i, std::vector<float> & SR );

#endif //SHADE_ILS_MTS_LS1_H
