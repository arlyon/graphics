//
// Created by Alexander Lyon on 2019-10-29.
//

#include "octree.hpp"
#include "../components/components.hpp"

bool octree::inside(position position) {
    return
            abs(this->origin.x - position.position.x) <= this->length / 2 &&
            abs(this->origin.y - position.position.y) <= this->length / 2 &&
            abs(this->origin.z - position.position.z) <= this->length / 2;
}

void octree::add(entt::entity entity, position position) {
    if (this->occupantCount == MAX_OCCUPANTS) {
        this->subDivide();
    }

    if (this->subDivided) {
        octree *node = this->getQuadrant(position);
        node->add(entity, position);
    } else {
        for (auto &occupant : this->occupants) {
            if (occupant == nullptr) {
                occupant = new Location{position, entity};
                this->occupantCount += 1;
                return;
            }
        }
        throw "Unexpected state, no free elements";
    }
}

octree::octree(glm::vec3 *origin, float length, octree *parent) : octree(origin, length) {
    this->parent = parent;
}

octree::octree(glm::vec3 *origin, float length) {
    this->origin = *origin;
    this->length = length;
}

void octree::subDivide() {
    if (this->subDivided) return;
    else this->subDivided = true;

    float offset = this->length / 4;
    for (int i = 0; i < 8; i++) {
        auto vec = new glm::vec3(this->origin);
        if (i % 2 == 0) vec->x += offset;
        else vec->x -= offset;
        if (i % 4 < 2) vec->y += offset;
        else vec->y -= offset;
        if (i < 4) vec->z += offset;
        else vec->z -= offset;
        this->children[i] = new octree(vec, this->length / 2, this);
    }

    for (auto &i : this->occupants) {
        if (i != nullptr) {
            octree *node = this->getQuadrant(i->pos);
            node->add(i->entity, i->pos);
            i = nullptr;
            this->occupantCount -= 1;
        }
    }
}

uint8_t cubeSphereIntersection(glm::vec3 cube, float cubeRadius, glm::vec3 sphere, float sphereRadius) {
    bool intersect[6] = {};
    bool inside[6] = {};
    float planes[6] = {
            cube.x - cubeRadius,
            cube.x + cubeRadius,
            cube.y - cubeRadius,
            cube.y + cubeRadius,
            cube.z - cubeRadius,
            cube.z + cubeRadius
    };

    // calculate if sphere is "inside" each plane
    if (sphere.x - sphereRadius > planes[0]) inside[0] = true;
    if (sphere.x + sphereRadius < planes[1]) inside[1] = true;
    if (sphere.y - sphereRadius > planes[2]) inside[2] = true;
    if (sphere.y + sphereRadius < planes[3]) inside[3] = true;
    if (sphere.z - sphereRadius > planes[4]) inside[4] = true;
    if (sphere.z + sphereRadius < planes[5]) inside[5] = true;
    if (std::all_of(std::begin(inside), std::end(inside), [](bool x) {return x;})) return 2;

    // calculate intersections
    if (abs(sphere.x - planes[0]) < sphereRadius) intersect[0] = true;
    if (abs(sphere.x - planes[1]) < sphereRadius) intersect[1] = true;
    if (abs(sphere.y - planes[2]) < sphereRadius) intersect[2] = true;
    if (abs(sphere.y - planes[3]) < sphereRadius) intersect[3] = true;
    if (abs(sphere.z - planes[4]) < sphereRadius) intersect[4] = true;
    if (abs(sphere.z - planes[5]) < sphereRadius) intersect[5] = true;
    if (std::any_of(std::begin(intersect), std::end(intersect), [](bool x) {return x;})) return 1;

    return 0;
}

/**
 * todo(arlyon) if an octant is completely inside the radius, don't check distances.
 * @param point
 * @param distance
 * @return
 */
std::vector<entt::entity *> octree::radiusSearch(glm::vec3 point, float distance) {
    auto entities = std::vector<entt::entity *>{};
    if (!cubeSphereIntersection(this->origin, this->length / 2, point, distance)) return entities;

    return entities;
}

// looking top down, with north facing
// up, the indices are as follows
//
//     0,1             4,5
//     2,3             6,7
// (first layer)  (second layer)
//
//
// 0 and 2 are x+ from 1, 3
// 0 and 1 are y+ from 2, 3
// 0 and 2 are z+ from 4, 6
octree *octree::getQuadrant(position position) {
    int quadrant = 0;
    if (position.position.x < this->origin.x) quadrant += 1;
    if (position.position.y < this->origin.y) quadrant += 2;
    if (position.position.z < this->origin.z) quadrant += 4;

    auto node = this->children[quadrant];

#ifndef NDEBUG
    if (!node->inside(position)) {
        auto m = {
                this->children[0]->inside(position),
                this->children[1]->inside(position),
                this->children[2]->inside(position),
                this->children[3]->inside(position),
                this->children[4]->inside(position),
                this->children[5]->inside(position),
                this->children[6]->inside(position),
                this->children[7]->inside(position),
        };
        auto x = node->origin - position.position;
        auto l = glm::length(x);
        auto y = 1;
    }
#endif

    return node;
}

octree *octree::getNode(position position) {
    auto node = this->getQuadrant(position);
    if (node->subDivided) return node->getNode(position);
    else return node;
}