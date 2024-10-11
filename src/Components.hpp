// Copyright [year] <Copyright Owner>

#ifndef SRC_COMPONENTS_HPP_
#define SRC_COMPONENTS_HPP_

#include <string>

#include "raylib.h"

enum SpriteLocation {
    CHARACTERS,
    ROOMS
};

struct MappingPosition {
    SpriteLocation location = CHARACTERS;
    int width = 0;
    int height = 0;
    int x = 0;
    int y = 0;
};

struct Animation {
    std::string name{};
    unsigned actual_frame = 0;
    unsigned total_frames = 0;
    float current_frame = 0.0f;
};

struct Render {
    int z_index = 0;
    Vector2 position{};
    MappingPosition sprite{};
};

#endif  // SRC_COMPONENTS_HPP_
