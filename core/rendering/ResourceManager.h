#pragma once

#include "FileFunctions.h"
#include "Renderer.h"
#include "Texture.h"
#include "Material.h"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <memory>

namespace WorldMaker
{
    inline std::string diffuseTexEmptyPath = "core/rendering/assets/textures/empty.png";
    inline std::string diffuseTexDefaultPath = "core/rendering/assets/textures/default.png";
    inline std::string specularTexDefaultPath = "core/rendering/assets/textures/default.png";
    inline std::string diffuseTexDefaultGrassPath = "core/rendering/assets/textures/defaultGrass.png";

    class ResourceManager
    {
    public:
        static void Init();
        static void Shutdown();


        // Textures handling ---------------------------------------------------------------------------------
        template <typename T>
        static std::shared_ptr<T> LoadTexture(const std::string& relativePath)
        {
            auto it = s_textureCache.find(relativePath);
            if (it != s_textureCache.end())
            {
                if (!it->second.expired()) return std::static_pointer_cast<T>(it->second.lock());
                else s_textureCache.erase(it);
            }

            std::shared_ptr<T> newTexture = std::make_shared<T>(relativePath);

            s_textureCache[relativePath] = newTexture;
            return newTexture;
        }
        template <typename T>
        static std::shared_ptr<T> LoadTexture(const std::vector<std::string>& relativePaths)
        {
            std::string unifiedPath = UnifyPaths(relativePaths);
            auto it = s_textureCache.find(unifiedPath);
            if (it != s_textureCache.end())
            {
                if (!it->second.expired()) return std::static_pointer_cast<T>(it->second.lock());
                else s_textureCache.erase(it);
            }

            std::shared_ptr<T> newTexture = std::make_shared<T>(relativePaths);
            s_textureCache[unifiedPath] = newTexture;
            return newTexture;
        }

        static TextureSPtr GetTexture(const std::string& relativePath);
        static TextureSPtr GetTexture(const std::vector<std::string>& relativePaths);
        static void RemoveTextureIfExpired(const std::string& relativePath);
        static void RemoveTextureIfExpired(const std::vector<std::string>& relativePaths);

        static MaterialSPtr CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath = diffuseTexDefaultPath);
        static MaterialSPtr GetMaterial(unsigned int materialId);
        static void RemoveMaterialIfExpired(unsigned int materialId);

    private:

        static bool s_inited;
        static bool s_ended;
public:
        static std::unordered_map<std::string, TextureWPtr> s_textureCache;
        static std::unordered_map<unsigned int, MaterialWPtr> s_materialCache;
    };
}
