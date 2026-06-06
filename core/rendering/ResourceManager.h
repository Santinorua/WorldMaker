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
    inline std::string diffuseTexDefaultPath = "core/rendering/assets/textures/defaultGrass.png";
    inline std::string specularTexDefaultPath = "core/rendering/assets/textures/defaultSpecular.png";
    inline std::string reflectionTexDefaultPath = "core/rendering/assets/textures/defaultReflection.png";

    class ResourceManager
    {
    public:
        static void Init();
        static void Shutdown();


        // Textures handling ---------------------------------------------------------------------------------
        template <typename T>
        static Texture* LoadTexture(const std::string& relativePath)
        {
            Texture* texture = GetTexture(relativePath);
            if (texture)
            {
                s_textureCache[relativePath].refCount++;
                return texture;
            }

            TextureEntry newEntry;
            newEntry.texture = std::make_unique<T>(relativePath);
            newEntry.refCount = 1;

            s_textureCache[relativePath] = std::move(newEntry);
            return s_textureCache[relativePath].texture.get();
        }
        template <typename T>
        static Texture* LoadTexture(const std::vector<std::string>& relativePaths)
        {
            std::string unifiedPath = UnifyPaths(relativePaths);
            Texture* texture = GetTexture(unifiedPath);
            if (texture)
            {
                s_textureCache[unifiedPath].refCount++;
                return texture;
            }

            TextureEntry newEntry;
            newEntry.texture = std::make_unique<T>(relativePaths);
            newEntry.refCount = 1;

            s_textureCache[unifiedPath] = std::move(newEntry);
            return s_textureCache[unifiedPath].texture.get();
        }

        template<typename T>
        static void UnloadTexture(const std::string& relativePath)
        {
            Texture* texture = GetTexture(relativePath);
            if (!texture)
            {
                std::cout << "Warning: There is not such a texture with path " << relativePath << " to unload\n";
                return;
            }

            TextureEntry& entry = s_textureCache[relativePath];
            entry.refCount--;

            if (entry.refCount <=0)
            {
                s_textureCache.erase(relativePath);
            }

        }
        static Texture* GetTexture(const std::string& relativePath);
        static Texture* GetTexture(const std::vector<std::string>& relativePaths);

        static Material* CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath = diffuseTexDefaultPath, const std::string& reflectionTexPath = reflectionTexDefaultPath);
        static void DestroyMaterial(unsigned int materialId);
        static Material* GetMaterial(unsigned int materialIndex);

    private:

        struct TextureEntry
        {
            TextureUPtr texture;
            int refCount = 0;
        };

        // struct MaterialEntry
        // {
        // 	MaterialUPtr material;
        //  	int refCount = 0;
        // };
public:
        static std::unordered_map<std::string, TextureEntry> s_textureCache;
        static std::vector<MaterialUPtr> s_materialCache;
    };
}
