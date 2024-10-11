// #include <flecs.h>

#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>

#include "../Components.hpp"
#include "../interfaces/IExecutes.hpp"

using flecs::world;

class Renderer : public IExecutes {
 public:
    explicit Renderer(const std::vector<std::tuple<SpriteLocation, std::string>>& components);
    void LoadTextures(const std::vector<std::tuple<SpriteLocation, std::string>>& components);

    void Execute(world* ecs) override;
    const std::unordered_map<std::string, MappingPosition>& getSpriteMap();
 private:
    std::unordered_map<std::string, MappingPosition> sprite_map_;
    std::vector<Texture2D> textures_;
};
