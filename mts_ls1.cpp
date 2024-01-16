/*
 * Implements the MTS-LS1 indicated in MTS
 * http://sci2s.ugr.es/EAMHCO/pdfs/contributionsCEC08/tseng08mts.pdf
 * Lin-Yu Tseng; Chun Chen, "Multiple trajectory search for Large Scale Global Optimization," Evolutionary Computation, 2008. CEC 2008. (IEEE World Congress on Computational Intelligence). IEEE Congress on , vol., no., pp.3052,3059, 1-6 June 2008
 * doi: 10.1109/CEC.2008.4631210
 *
 * adapted from https://github.com/mateuz/MTS-LS1
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

#include "mts_ls1.h"
#include "objective_functions.h"
#include <sys/time.h>
#include <cstdio>
#include <unistd.h>

using namespace std;


LSresult mts_ls1_improve_dim(int DIM, float MIN, float MAX, int FUNCTION_NO, vector<float>& sol, float best_fitness, unsigned i, vector<float> & SR ){
    vector<float> newsol = sol;
    newsol[i] -= SR[i];

    float initial_fit = best_fitness;

    newsol[i] = min(newsol[i], MAX);
    newsol[i] = max(newsol[i], MIN);

    float fitness_newsol = objective_function_no(newsol, DIM, FUNCTION_NO);

    unsigned evals = 1;

    if( fitness_newsol < best_fitness ){
        best_fitness = fitness_newsol;
        sol = newsol;
    } else if( fitness_newsol > best_fitness ){
        newsol[i] = sol[i];
        newsol[i] += 0.5f * SR[i];

        newsol[i] = min(newsol[i], MAX);
        newsol[i] = max(newsol[i], MIN);

        fitness_newsol = objective_function_no(newsol, DIM, FUNCTION_NO);
        evals++;

        if( fitness_newsol < best_fitness ){
            best_fitness = fitness_newsol;
            sol = newsol;
        }
    }
    //printf("%1.20E -> %1.20E\n", initial_fit, best_fitness);
    return LSresult{sol, best_fitness, evals};
}

void mts_ls1(int DIM, float MIN, float MAX, int FUNCTION_NO, unsigned maxevals, vector<float>& sol){
    //unsigned int n_dim = ndim;
    unsigned totalevals = 0;
    float best_fitness = objective_function_no(sol, DIM, FUNCTION_NO);

    vector<float> SR(sol.size());
    for( unsigned i = 0; i < SR.size(); i++ ){
        SR[i] = (MAX - MIN) * 0.4f;
    }

    LSresult result;
    LSresult current_best = {sol, best_fitness, 0};

    vector<float> improvement(DIM, 0.0);

    vector<float> dim_sorted(DIM);
    iota(dim_sorted.begin(), dim_sorted.end(), 0);

    float improve;
    //warm-up
    if( totalevals < maxevals ){
        next_permutation(dim_sorted.begin(), dim_sorted.end());
        for( auto it = dim_sorted.begin(); it != dim_sorted.end(); it++ ){
            result = mts_ls1_improve_dim(DIM, MIN, MAX, FUNCTION_NO, sol, best_fitness, *it, SR);
            totalevals += result.evals;
            improve = max(current_best.fitness - result.fitness, 0.0f);
            improvement[*it] = improve;

            if( improve > 0.0 ){
                // printf("[1] %.10lf > %.10lf ~ improve: %.20lf\n", current_best.fitness, result.fitness, improve);
                current_best = result;
            } else {
                SR[*it] /= 2.0f;
            }
        }
    }

    iota(dim_sorted.begin(), dim_sorted.end(), 0);
    sort(dim_sorted.begin(), dim_sorted.end(), [&](unsigned i1, unsigned i2) { return improvement[i1] > improvement[i2]; });

    int i, d = 0, next_d, next_i;
    while( totalevals < maxevals ){
        i = dim_sorted[d];
        result = mts_ls1_improve_dim(DIM, MIN, MAX, FUNCTION_NO, current_best.solution, current_best.fitness, i, SR);
        totalevals += result.evals;
        improve = max(current_best.fitness - result.fitness, 0.0f);
        improvement[i] = improve;
        next_d = (d+1)%DIM;
        next_i = dim_sorted[next_d];

        if( improve > 0.0 ){
            // printf("[2] %.10lf > %.10lf ~ improve: %.20lf\n", current_best.fitness, result.fitness, improve);
            current_best = result;

            if( improvement[i] < improvement[next_i] ){
                iota(dim_sorted.begin(), dim_sorted.end(), 0);
                sort(dim_sorted.begin(), dim_sorted.end(), [&](unsigned i1, unsigned i2) { return improvement[i1] > improvement[i2]; });
            }
        } else {
            SR[i] /= 2.0;
            d = next_d;
            if( SR[i] < 1e-15 ){
                SR[i] = (MAX - MIN) * 0.4f;
            }
        }
    }
    sol = current_best.solution;
}
