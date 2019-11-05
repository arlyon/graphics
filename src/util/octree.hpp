//
// Created by Alexander Lyon on 2019-10-29.
//

#pragma once

#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "../components/components.hpp"

struct Location {
    position pos;
    entt::entity entity;
};

#define MAX_OCCUPANTS 10

/**
 * Octree!
 */
class octree {
public:
    octree(glm::vec3 *origin, float length);

    std::vector<entt::entity*> radiusSearch(glm::vec3 point, float distance);
    void add(entt::entity entity, position position);
    void subDivide();
    bool inside(position position);
private:
    octree(glm::vec3 *origin, float length, octree *parent);
    octree *getQuadrant(position position); // gets which octant of the current node that the point is in
    octree *getNode(position position); // gets the exact node the point is in

    octree *parent = nullptr;
    glm::vec3 origin;
    float length; // the diameter of the cube

    // an octree has either a list of 8 children or a vector of occupants
    Location *occupants[MAX_OCCUPANTS] = {};
    uint8_t occupantCount = 0;

    octree* children[8] = {};
    bool subDivided = false;
};
