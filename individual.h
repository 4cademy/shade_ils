//
// Created by marce on 14.01.2024.
//

#ifndef SHADE_ILS_INDIVIDUAL_H
#define SHADE_ILS_INDIVIDUAL_H

#include <vector>

struct Individual {
    std::vector<float> solution{};
    float fitness{};
};

#endif //SHADE_ILS_INDIVIDUAL_H
