#include "kolanut/graphics/Tilemap.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/core/Logging.h"

#include <cassert>
#include <limits>

namespace kola {
namespace graphics {

bool Tilemap::load(const std::string& dir, const std::string& fileName)
{
    this->map.ParseFile(dir + fileName);

    if (this->map.HasError())
    {
        knM_logError(
            "Can't load tilemap from '" << fileName << "'." << std::endl
            << "Error code: " << map.GetErrorCode()
            << "Error message: " << map.GetErrorText()
        );

        return false;
    }

    this->mapCache.setCellSize(Sizei { 
        std::numeric_limits<long>::max(),
        std::numeric_limits<long>::max()
    });

    for (unsigned int i = 0; i < this->map.GetNumTilesets(); ++i)
    {
        const Tmx::Tileset* tileset = this->map.GetTileset(i);

        Tileset& ts = this->tilesets[tileset->GetFirstGid()];

        ts.imagePath = dir + tileset->GetImage()->GetSource();
        ts.texture = di::get<Renderer>()->loadTexture(ts.imagePath);

        if (!ts.texture)
        {
            knM_logError("Can't load tileset image '" << ts.imagePath << "'");
            return false;
        }

        const Sizei& cacheCellSize = this->mapCache.getCellSize();

        if (
            tileset->GetTileWidth() < cacheCellSize.x 
            || tileset->GetTileHeight() < cacheCellSize.y
        )
        {
            this->mapCache.setCellSize(Sizei {
                tileset->GetTileWidth(),
                tileset->GetTileHeight()
            });
        }
    }

    return true;
}

bool Tilemap::instantiateLayer(uint32_t layer, const Vec2f& position, const Vec2f& scale)
{
    if (layer >= this->map.GetNumTileLayers())
    {
        return false;
    }

    const Tmx::TileLayer* layerObj = this->map.GetTileLayer(layer);

    for (unsigned int y = 0; y < layerObj->GetHeight(); ++y)
    {
        for (unsigned int x = 0; x < layerObj->GetWidth(); ++x)
        {
            if (layerObj->GetTileTilesetIndex(x, y) == -1)
            {
                continue;
            }

            unsigned int gid = layerObj->GetTileGid(x, y);

            if (gid == 0)
            {
                continue;
            }

            const Tmx::Tileset* tileset = this->map.FindTileset(gid);
            assert(tileset);

            assert(this->tilesets.find(tileset->GetFirstGid()) != this->tilesets.end());
            const Tileset& ts = this->tilesets[tileset->GetFirstGid()];

            assert(gid >= tileset->GetFirstGid());
            unsigned int gidIdx = (gid - tileset->GetFirstGid());

            Tile tile;
            tile.position = position 
                + Vec2f { 
                    tileset->GetTileWidth() * x * scale.x, 
                    tileset->GetTileHeight() * y * scale.y 
                }
            ;
            
            tile.rect = {
                (gidIdx % tileset->GetColumns()) * tileset->GetTileWidth(),
                (gidIdx / tileset->GetColumns()) * tileset->GetTileHeight(),
                tileset->GetTileWidth(),
                tileset->GetTileHeight()
            };

            tile.scale = scale;
            tile.texture = ts.texture;
            tile.tile = &layerObj->GetTile(x, y);

            this->mapCache.setCell(
                this->mapCache.getPointFromWorldCoords(tile.position),
                std::move(tile),
                layer
            );
        }
    }
    
    return true;
}

bool Tilemap::instantiate(const Vec2f& position, const Vec2f& scale)
{
    this->mapCache.clear();
    
    for (unsigned int i = 0; i < getLayersCount(); ++i)
    {
        instantiateLayer(i, position, scale);
    }

    return false;
}

bool Tilemap::draw()
{
    std::shared_ptr<graphics::Renderer> renderer = di::get<graphics::Renderer>();

    for (auto it = this->mapCache.cbegin(); it != this->mapCache.cend(); ++it)
    {
        const std::unordered_map<uint32_t, Tile>& tiles = it->second;

        for (uint32_t layer = 0; layer < this->map.GetNumTileLayers(); layer++)
        {
            auto itt = tiles.find(layer);

            if (itt == tiles.end())
            {
                continue;
            }

            const Tile& tile = itt->second;

            renderer->draw(
                tile.texture,
                tile.position,
                0.0f,
                tile.scale,
                Vec2f { 0.0f, 0.0f },
                tile.rect
            );   
        }
    }

    return true;
}

const Tilemap::Tile* Tilemap::getTileAt(uint32_t layer, const Vec2f& position)
{
    if (layer >= this->map.GetNumTileLayers())
    {
        return nullptr;
    }

    const Tile* tile = this->getMapCache().getCell(
        this->getMapCache().getPointFromWorldCoords(position),
        layer
    );

    return tile;
}

const std::vector<Tmx::Object*>& Tilemap::getObjectsInGroup(uint32_t group)
{
    static const std::vector<Tmx::Object*> emptyGroup;

    if (group >= this->map.GetNumObjectGroups())
    {
        return emptyGroup;
    }

    const Tmx::ObjectGroup* objGroup = this->map.GetObjectGroup(group);

    return objGroup->GetObjects();
}

Tmx::Object* Tilemap::getObjectByName(uint32_t group, const std::string& name)
{
    auto objects = getObjectsInGroup(group);

    for (auto obj : objects)
    {
        if (obj->GetName() == name)
        {
            return obj;
        }
    }

    return nullptr;
}

std::vector<Tmx::Object*> Tilemap::getObjectsByType(uint32_t group, const std::string& type)
{
    auto objects = getObjectsInGroup(group);
    std::vector<Tmx::Object*> foundObjs = {};

    for (auto obj : objects)
    {
        if (obj->GetType() == type)
        {
            foundObjs.push_back(obj);
        }
    }

    return foundObjs;
}

uint32_t Tilemap::getObjectsGroupCount() const
{
    return this->map.GetNumObjectGroups();
}

uint32_t Tilemap::getLayersCount() const
{
    return this->map.GetNumTileLayers();
}
    
} // namespace graphics
} // namespace kola