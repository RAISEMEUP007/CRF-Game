// QuadTreeTests.cpp

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "QuadTree.hpp"

TEST_CASE("queries for points in the tree") {
    QuadTree tree({0.0f, 0.0f, 100.0f, 100.0f});

    tree.insert({0, 10.0f, 10.0f});
    tree.insert({0, 20.0f, 20.0f});
    tree.insert({0, 30.0f, 30.0f});
    tree.insert({0, 40.0f, 40.0f});
    tree.insert({0, 50.0f, 50.0f});

    std::vector<EntityPosition> points = tree.query({0.0f, 0.0f, 100.0f, 100.0f});

    REQUIRE(points.size() == 5);
}

