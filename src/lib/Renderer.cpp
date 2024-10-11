#include <flecs.h>

#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <algorithm>

#include "../Components.hpp"
#include "Renderer.hpp"


Renderer::Renderer(const std::vector<std::tuple<SpriteLocation, std::string>>& components) {
    for (const auto& component : components) {
        SpriteLocation location;
        std::string path;
        std::tie(location, path) = component;

        // Open and parse the JSON file
        std::ifstream json_file(path);
        if (!json_file.is_open()) {
            std::cerr << "Could not open JSON file: " << path << std::endl;
            continue;
        }

        nlohmann::json json_data;
        json_file >> json_data;

        // Check if "frames" exists
        if (json_data.contains("frames")) {
            auto frames = json_data["frames"];

            for (auto it = frames.begin(); it != frames.end(); ++it) {
                std::string key_with_ext = it.key();  // e.g., "attack/thief_0001.png"

                // Remove the ".png" extension
                std::string key = key_with_ext.substr(0, key_with_ext.find_last_of('.'));

                // Get frame data
                auto frame_info = it.value()["frame"];
                int x = frame_info["x"];
                int y = frame_info["y"];
                int w = frame_info["w"];
                int h = frame_info["h"];

                // Create and populate MappingPosition
                MappingPosition sprite_pos;
                sprite_pos.location = location;
                sprite_pos.width = w;
                sprite_pos.height = h;
                sprite_pos.x = x;
                sprite_pos.y = y;

                // Insert into the map
                sprite_map_[key] = sprite_pos;
            }
        } else {
            std::cerr << "No 'frames' key found in JSON file: " << path << std::endl;
        }
    }
}

void Renderer::LoadTextures(const std::vector<std::tuple<SpriteLocation, std::string>>& components) {
    // If textures are already loaded, unload them first
    if (!textures_.empty()) {
        for (auto& texture : textures_) {
            UnloadTexture(texture);
        }
        textures_.clear();
    }

    textures_.resize(components.size());
    // No need to reserve space for unordered_map
    for (const auto& component : components) {
        SpriteLocation location;
        std::string path;
        std::tie(location, path) = component;

        Texture2D texture = LoadTexture(path.c_str());
        textures_[location] = texture;
    }
}

const std::unordered_map<std::string, MappingPosition>& Renderer::getSpriteMap() {
    return sprite_map_;
}

int compare_renders(flecs::entity_t e1, const Render* a, flecs::entity_t e2, const Render* b) {
    if (a->z_index < b->z_index) return -1;
    if (a->z_index > b->z_index) return 1;
    return 0;
}

void Renderer::Execute(world* ecs) {
  ecs->system<Render>("Render System")
      .order_by<Render>(compare_renders)
      .each([this](flecs::entity e, Render& render) {
        // Draw the texture
        DrawTexturePro(textures_[render.sprite.location],
                       {.x = static_cast<float>(render.sprite.x),
                        .y = static_cast<float>(render.sprite.y),
                        .width = static_cast<float>(render.sprite.width),
                        .height = static_cast<float>(render.sprite.height)},
                       {.x = render.position.x,
                        .y = render.position.y,
                        .width = static_cast<float>(render.sprite.width),
                        .height = static_cast<float>(render.sprite.height)},
                       {static_cast<float>(render.sprite.width),
                        static_cast<float>(render.sprite.height)},
                       0.0f, WHITE);
      });

  ecs->system<Render, Animation>().each(
      [this](flecs::entity e, Render& render, Animation& animation) {
        if (animation.current_frame > 0.2) {
          animation.current_frame = 0.0f;
          animation.actual_frame++;
          if (animation.actual_frame > animation.total_frames) {
            animation.actual_frame = 1;
          }
          std::ostringstream oss;
          oss << animation.name.substr(0, animation.name.find_last_of('_') + 1)
              << std::setw(4) << std::setfill('0') << animation.actual_frame;
          animation.name = oss.str();
          render.sprite = sprite_map_[animation.name];
        } else {
          animation.current_frame += GetFrameTime();
        }
      });
}
