// QuadTree.cpp

#include "QuadTree.hpp"
#include <algorithm>

QuadTree::QuadTree(const Rectangle& boundary)
    : lowestX(std::numeric_limits<float>::infinity()),
      lowestY(std::numeric_limits<float>::infinity()),
      highestX(-std::numeric_limits<float>::infinity()),
      highestY(-std::numeric_limits<float>::infinity()) {
    arrayList.reserve(10);
    Node rootNode;
    rootNode.boundary = boundary;
    arrayList.push_back(rootNode);
}

void QuadTree::reset() {
    arrayList.clear();
    Rectangle newBoundary = {
        lowestX - gutter,
        lowestY - gutter,
        (highestX - lowestX) + gutter * 2.0f,
        (highestY - lowestY) + gutter * 2.0f
    };
    Node rootNode;
    rootNode.boundary = newBoundary;
    arrayList.push_back(rootNode);
    lowestX = std::numeric_limits<float>::infinity();
    lowestY = std::numeric_limits<float>::infinity();
    highestX = -std::numeric_limits<float>::infinity();
    highestY = -std::numeric_limits<float>::infinity();
}

bool QuadTree::insert(const EntityPosition& point) {
    lowestX = std::min(lowestX, point.x);
    lowestY = std::min(lowestY, point.y);
    highestX = std::max(highestX, point.x);
    highestY = std::max(highestY, point.y);
    return insertPosition(point, 0);
}

bool QuadTree::insertPosition(const EntityPosition& point, unsigned position) {
    if (!pointInsideBoundary(point, position)) return false;

    Node& node = arrayList[position];

    if (node.total_elements < capacity && !node.divided) {
        node.points[node.total_elements] = point;
        node.total_elements += 1;
        return true;
    }

    if (!node.divided) {
        subdivide(position);
    }

    if (insertPosition(point, nw(position))) return true;
    if (insertPosition(point, ne(position))) return true;
    if (insertPosition(point, sw(position))) return true;
    if (insertPosition(point, se(position))) return true;

    // Should not reach here
    return false;
}

void QuadTree::subdivide(unsigned position) {
    Node& node = arrayList[position];
    float halfWidth = node.boundary.width / 2.0f;
    float halfHeight = node.boundary.height / 2.0f;
    float x = node.boundary.x;
    float y = node.boundary.y;

    Rectangle nwBoundary = { x, y, halfWidth, halfHeight };
    Rectangle neBoundary = { x + halfWidth, y, halfWidth, halfHeight };
    Rectangle swBoundary = { x, y + halfHeight, halfWidth, halfHeight };
    Rectangle seBoundary = { x + halfWidth, y + halfHeight, halfWidth, halfHeight };

    node.nw = arrayList.size();
    Node nwNode;
    nwNode.boundary = nwBoundary;
    arrayList.push_back(nwNode);

    node.ne = arrayList.size();
    Node neNode;
    neNode.boundary = neBoundary;
    arrayList.push_back(neNode);

    node.sw = arrayList.size();
    Node swNode;
    swNode.boundary = swBoundary;
    arrayList.push_back(swNode);

    node.se = arrayList.size();
    Node seNode;
    seNode.boundary = seBoundary;
    arrayList.push_back(seNode);

    node.divided = true;
}

std::vector<EntityPosition> QuadTree::query(const Rectangle& range) {
    std::vector<EntityPosition> buffer;
    queryPositionOnBuffer(range, 0, buffer);
    return buffer;
}

void QuadTree::queryPositionOnBuffer(const Rectangle& range, unsigned position, std::vector<EntityPosition>& buffer) {
    if (!checkCollision(arrayList[position].boundary, range)) {
        return;
    }

    Node& node = arrayList[position];

    for (uint32_t i = 0; i < node.total_elements; ++i) {
        EntityPosition& p = node.points[i];
        if (p.x >= range.x && p.x <= range.x + range.width &&
            p.y >= range.y && p.y <= range.y + range.height) {
            buffer.push_back(p);
        }
    }

    if (node.divided) {
        queryPositionOnBuffer(range, nw(position), buffer);
        queryPositionOnBuffer(range, ne(position), buffer);
        queryPositionOnBuffer(range, sw(position), buffer);
        queryPositionOnBuffer(range, se(position), buffer);
    }
}

bool QuadTree::pointInsideBoundary(const EntityPosition& point, unsigned position) {
    Rectangle& boundary = arrayList[position].boundary;
    return (point.x >= boundary.x &&
            point.x <= boundary.x + boundary.width &&
            point.y >= boundary.y &&
            point.y <= boundary.y + boundary.height);
}

bool QuadTree::checkCollision(const Rectangle& a, const Rectangle& b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

unsigned QuadTree::nw(unsigned position) {
    return arrayList[position].nw;
}

unsigned QuadTree::ne(unsigned position) {
    return arrayList[position].ne;
}

unsigned QuadTree::sw(unsigned position) {
    return arrayList[position].sw;
}

unsigned QuadTree::se(unsigned position) {
    return arrayList[position].se;
}

