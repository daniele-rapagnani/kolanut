#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Texture.h"

#include <Tmx.h>

#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <cmath>

namespace kola {
namespace graphics {

struct hash_pair final {
    template<class TFirst, class TSecond>
    size_t operator()(const std::pair<TFirst, TSecond>& p) const noexcept {
        uintmax_t hash = std::hash<TFirst>{}(p.first);
        hash <<= sizeof(uintmax_t) * 4;
        hash ^= std::hash<TSecond>{}(p.second);
        return std::hash<uintmax_t>{}(hash);
    }
};

class Tilemap
{
public:
    struct Tile
    {
        const Tmx::MapTile* tile = nullptr;
        
        Recti rect;
        Vec2f position;
        Vec2f scale;
        std::shared_ptr<Texture> texture = nullptr;
    };

    struct Tileset
    {
        std::string imagePath;
        std::shared_ptr<Texture> texture;
    };

    using MapCachePoint = std::pair<int32_t, int32_t>;
    using MapCacheData = std::unordered_map<
        MapCachePoint, 
        std::unordered_map<uint32_t, Tile>, 
        hash_pair
    >;

    class MapCache
    {
    public:
        using iterator = MapCacheData::iterator;
        using const_iterator = MapCacheData::const_iterator;

    public:
        void setCellSize(const Sizei& size)
        { this->cellSize = size; }

        const Sizei& getCellSize()
        { return this->cellSize; }

        iterator begin()
        { return this->data.begin(); }

        iterator end()
        { return this->data.end(); }

        const_iterator cbegin()
        { return this->data.cbegin(); }

        const_iterator cend()
        { return this->data.cend(); }

        void clear()
        { this->data.clear(); }

        bool remove(MapCachePoint point, uint32_t layer)
        {
            auto it = this->data.find(point);

            if (it == this->data.end())
            {
                return false;
            }

            it->second.erase(layer);
            return true;
        }

        bool remove(Recti rect, uint32_t layer)
        {
            bool found = false;

            for (long x = 0; x < rect.size.x; x++)
            {
                for (long y = 0; y < rect.size.y; y++)
                {
                    found = found || remove({ rect.origin.x + x, rect.origin.y + y }, layer);
                }
            }

            return found;
        }

        void setCell(MapCachePoint point, const Tile& tile, uint32_t layer)
        {
            this->data[point][layer] = tile;
        }

        const Tile* getCell(MapCachePoint point, uint32_t layer)
        {
            if (this->data.find(point) == this->data.end())
            {
                return nullptr;
            }

            std::unordered_map<uint32_t, Tile>& tiles = this->data[point];

            if (tiles.find(layer) == tiles.end())
            {
                return nullptr;
            }

            return &tiles[layer];
        }

        MapCachePoint getPointFromWorldCoords(const Vec2f& worldPos)
        {
            MapCachePoint point;
            point.first = static_cast<int32_t>(round(worldPos.x)) / getCellSize().x;
            point.second = static_cast<int32_t>(round(worldPos.y)) / getCellSize().y;

            return point;
        }
    
    private:
        Sizei cellSize;
        MapCacheData data;
    };

public:
    bool load(const std::string& dir, const std::string& fileName);

    const Tilemap::Tile* getTileAt(uint32_t layer, const Vec2f& position);
    uint32_t getLayersCount() const;

    bool instantiateLayer(uint32_t layer, const Vec2f& position, const Vec2f& scale);
    bool instantiate(const Vec2f& position, const Vec2f& scale);

    bool draw();

    const std::vector<Tmx::Object*>& getObjectsInGroup(uint32_t group);
    Tmx::Object* getObjectByName(uint32_t group, const std::string& name);
    std::vector<Tmx::Object*> getObjectsByType(uint32_t group, const std::string& type);
    uint32_t getObjectsGroupCount() const;

    MapCache& getMapCache()
    { return this->mapCache; }

private:
    Tmx::Map map;
    MapCache mapCache;
    std::unordered_map<unsigned int, Tileset> tilesets;
};

} // namespace graphics
} // namespace kola