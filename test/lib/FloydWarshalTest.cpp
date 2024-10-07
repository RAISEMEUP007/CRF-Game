
#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include "FloydWarshal.h"

TEST_CASE("Floyd-Warshall generates a connection between two points", "[generate_connection]") {
    FloydWarshal fw(3);
    fw.clean();
    fw.addEdge(0, 1, 1);
    fw.addEdge(1, 2, 1);
    fw.generate();
    REQUIRE(fw.hasPath(0, 2));
}

TEST_CASE("Floyd-Warshall has the correct distance between two points", "[correct_distance]") {
    FloydWarshal fw(3);
    fw.clean();
    fw.addEdge(0, 1, 1);
    fw.addEdge(1, 2, 1);
    fw.generate();
    REQUIRE(fw.value(0, 2) == 2);
}

TEST_CASE("Floyd-Warshall maps the correct value with mapping", "[with_mapping][value]") {
    FloydWarshal fw(3);
    fw.clean();
    fw.addEdgeWithMapping(99, 100, 1);
    fw.addEdgeWithMapping(100, 191, 1);
    fw.generate();
    REQUIRE(fw.valueWithMapping(99, 191) == 2);
}

TEST_CASE("Floyd-Warshall returns the correct next node with mapping", "[with_mapping][next]") {
    FloydWarshal fw(3);
    fw.clean();
    fw.addEdgeWithMapping(99, 100, 1);
    fw.addEdgeWithMapping(100, 191, 1);
    fw.generate();
    REQUIRE(fw.nextWithMapping(99, 191) == 100);
}

TEST_CASE("Floyd-Warshall has path with mapping", "[with_mapping][hasPath]") {
    FloydWarshal fw(3);
    fw.clean();
    fw.addEdgeWithMapping(99, 100, 1);
    fw.addEdgeWithMapping(100, 191, 1);
    fw.generate();
    REQUIRE(fw.hasPathWithMapping(99, 191));
}

TEST_CASE("Floyd-Warshall resizes correctly", "[resize]") {
    FloydWarshal fw(3);
    fw.clean();
    fw.addEdge(0, 1, 1);
    fw.addEdge(1, 2, 1);
    fw.generate();
    REQUIRE(fw.hasPath(0, 2));

    fw.resize(4);
    fw.clean();
    fw.addEdge(0, 1, 1);
    fw.addEdge(1, 2, 1);
    fw.addEdge(2, 3, 1);
    fw.generate();
    REQUIRE(fw.hasPath(1, 3));
}
