// RendererTest.cpp

#define CATCH_CONFIG_MAIN
#include <iostream>
#include <fstream>
#include <catch2/catch_all.hpp>
#include <nlohmann/json.hpp>

#include "Renderer.hpp"

TEST_CASE("Renderer processes JSON files correctly", "[Renderer]") {
    // Path to fixtures
    const std::string fixtures_path = "../test/fixtures/";

    // Prepare components vector
    std::vector<std::tuple<SpriteLocation, std::string>> components = {
        {CHARACTERS, fixtures_path + "characters.json"}
    };

    // Instantiate Renderer
    Renderer renderer(components);

    // Retrieve the sprite map
    const auto& sprite_map = renderer.getSpriteMap();

    // Expected keys without .png extension
    std::vector<std::string> expected_keys = {
        "attack/thief_0001",
        "attack/thief_0002",
        "attack/thief_0003"
    };

    SECTION("Sprite map contains the correct number of entries") {
        REQUIRE(sprite_map.size() == expected_keys.size());
    }

    SECTION("Sprite map contains expected keys and values") {
        for (const auto& key : expected_keys) {
            INFO("Checking key: " << key);
            REQUIRE(sprite_map.find(key) != sprite_map.end());

            const MappingPosition& pos = sprite_map.at(key);

            // Expected values based on the sample JSON
            if (key == "attack/thief_0001") {
                REQUIRE(pos.location == CHARACTERS);
                REQUIRE(pos.width == 26);
                REQUIRE(pos.height == 61);
                REQUIRE(pos.x == 645);
                REQUIRE(pos.y == 571);
            } else if (key == "attack/thief_0002") {
                REQUIRE(pos.location == CHARACTERS);
                REQUIRE(pos.width == 29);
                REQUIRE(pos.height == 61);
                REQUIRE(pos.x == 525);
                REQUIRE(pos.y == 511);
            } else if (key == "attack/thief_0003") {
                REQUIRE(pos.location == CHARACTERS);
                REQUIRE(pos.width == 46);
                REQUIRE(pos.height == 58);
                REQUIRE(pos.x == 410);
                REQUIRE(pos.y == 388);
            }
        }
    }
}

TEST_CASE("Renderer handles missing or invalid JSON files gracefully", "[Renderer]") {
    // Path to fixtures
    const std::string fixtures_path = "../test/fixtures/";

    // Prepare components with a non-existent or invalid file
    std::vector<std::tuple<SpriteLocation, std::string>> components = {
        {ROOMS, fixtures_path + "invalid.json"}
    };

    // Redirect std::cerr to suppress error messages during the test
    std::streambuf* original_cerr = std::cerr.rdbuf();
    std::ostringstream captured_cerr;
    std::cerr.rdbuf(captured_cerr.rdbuf());

    // Instantiate Renderer
    Renderer renderer(components);

    // Restore std::cerr
    std::cerr.rdbuf(original_cerr);

    // Retrieve the sprite map
    const auto& sprite_map = renderer.getSpriteMap();

    SECTION("Sprite map is empty when JSON file is missing or invalid") {
        REQUIRE(sprite_map.empty());
    }
}

TEST_CASE("Renderer processes multiple JSON files", "[Renderer]") {
    // Path to fixtures
    const std::string fixtures_path = "../test/fixtures/";

    // Prepare components vector
    std::vector<std::tuple<SpriteLocation, std::string>> components = {
        {CHARACTERS, fixtures_path + "characters.json"},
        {ROOMS, fixtures_path + "rooms.json"}
    };

    // Instantiate Renderer
    Renderer renderer(components);

    // Retrieve the sprite map
    const auto& sprite_map = renderer.getSpriteMap();

    SECTION("Sprite map contains entries from both JSON files") {
        REQUIRE(sprite_map.size() == 4);  // 3 characters + 1 room
        REQUIRE(sprite_map.find("attack/thief_0001") != sprite_map.end());
        REQUIRE(sprite_map.find("attack/thief_0002") != sprite_map.end());
        REQUIRE(sprite_map.find("attack/thief_0003") != sprite_map.end());
        REQUIRE(sprite_map.find("room1") != sprite_map.end());
    }

    SECTION("Entries have correct locations and data") {
        // Character sprites
        const MappingPosition& thief1_pos = sprite_map.at("attack/thief_0001");
        REQUIRE(thief1_pos.location == CHARACTERS);
        REQUIRE(thief1_pos.width == 26);
        REQUIRE(thief1_pos.height == 61);
        REQUIRE(thief1_pos.x == 645);
        REQUIRE(thief1_pos.y == 571);

        // Room sprite
        const MappingPosition& room1_pos = sprite_map.at("room1");
        REQUIRE(room1_pos.location == ROOMS);
        REQUIRE(room1_pos.width == 800);
        REQUIRE(room1_pos.height == 600);
        REQUIRE(room1_pos.x == 300);
        REQUIRE(room1_pos.y == 400);
    }
}
