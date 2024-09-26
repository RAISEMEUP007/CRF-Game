// QuadTree.hpp

#pragma once

#include <vector>
#include <limits>
#include <cstdint>

struct EntityPosition {
    uint32_t entity;
    float x;
    float y;
};

struct Rectangle {
    float x;
    float y;
    float width;
    float height;
};

struct Node {
    uint32_t total_elements = 0;
    EntityPosition points[4];
    Rectangle boundary;
    bool divided = false;
    unsigned nw = 0;
    unsigned ne = 0;
    unsigned sw = 0;
    unsigned se = 0;
};

class QuadTree {
public:
    QuadTree(const Rectangle& boundary);
    void reset();
    bool insert(const EntityPosition& point);
    std::vector<EntityPosition> query(const Rectangle& range);

private:
    void subdivide(unsigned position);
    bool insertPosition(const EntityPosition& point, unsigned position);
    void queryPositionOnBuffer(const Rectangle& range, unsigned position, std::vector<EntityPosition>& buffer);
    bool pointInsideBoundary(const EntityPosition& point, unsigned position);
    bool checkCollision(const Rectangle& a, const Rectangle& b);
    unsigned nw(unsigned position);
    unsigned ne(unsigned position);
    unsigned sw(unsigned position);
    unsigned se(unsigned position);

    std::vector<Node> arrayList;
    uint32_t capacity = 4;
    float gutter = 20.0f;

    float lowestX;
    float lowestY;
    float highestX;
    float highestY;
};

