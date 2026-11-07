#pragma once
#include "flow_data_registry.h"
#include "flow_material.h"
#include "flow_model.h"
#include "flow_sprite.h"

namespace flow
{
    class AssetDatabase
    {
        DataRegistry<Model> _models;
        DataRegistry<Sprite> _sprites;
        DataRegistry<Material> _materials;

    public:
        ~AssetDatabase();
        Model* LoadModel(const std::string& assetLocation);
        Material* LoadMaterial(const std::string& assetLocation);
        Sprite* LoadSprite(const std::string& assetLocation);

        bool AssetPathExists(const std::string& assetLocation);

        template <typename T>
        bool AssetLoaded(const std::string& assetLocation)
        {
            return DataRegistry<T>::registry.contains(assetLocation);
        }
    };
}
