#include <random>

#include "components.hpp"

static std::random_device rd;
static std::mt19937 eng(rd());
static std::uniform_real_distribution<float> dist(0, 10);

fish::fish(uint8_t group) {
    this->group = group;
    this->hueShift = (float) (group % 5) / 5.0f;
    this->timeOffset = dist(eng);
}