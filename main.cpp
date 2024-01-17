#include <iostream>
#include <cstdlib>
#include <ctime>

// local includes
#include "shade_ils.h"


int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cout << "Usage: <population size> <dimensionality> <min> <max> <function number> <max evaluations>" << std::endl;
        return 1;
    }

    const int POPSIZE = std::stoi(argv[1]);  // Population size
    const int DIM = std::stoi(argv[2]);  // Dimensionality of the problem
    const float MIN = std::stof(argv[3]);  // Minimum value for each gene
    const float MAX = std::stof(argv[4]);  // Maximum value for each gene
    const int FUNCTION_NO = std::stoi(argv[5]);  // Number of the objective function to be optimized
    const int FUNCTION_EVALS = std::stoi(argv[6]);  // Maximum number of function evaluations

    std::cout << "MAIN" << std::endl;
    std::cout << "POPSIZE: " << POPSIZE << std::endl;
    std::cout << "DIM: " << DIM << std::endl;
    std::cout << "MIN: " << MIN << std::endl;
    std::cout << "MAX: " << MAX << std::endl;
    std::cout << "FUNCTION_NO: " << FUNCTION_NO << std::endl;
    std::cout << "FUNCTION_EVALS: " << FUNCTION_EVALS << std::endl;

    // Run SHADE-ILS algorithm
    SHADE_ILS(POPSIZE, DIM, MIN, MAX, FUNCTION_NO, FUNCTION_EVALS);
    return 0;
}
