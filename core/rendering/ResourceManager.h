#pragma once

#include "FileFunctions.h"
#include "Renderer.h"
#include "Texture.h"
#include "Material.h"
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
        static TextureWPtr LoadTexture(const std::string& relativePath)
        {
            auto it = s_textureCache.find(relativePath);
            if (it != s_textureCache.end())
            {
                it->second.refCount++;
                return it->second.texture;
            }

            TextureSPtr newTexture = std::make_shared<T>(relativePath);

            TextureEntry newEntry;
            newEntry.texture = newTexture;
            newEntry.refCount = 1;

            s_textureCache[relativePath] = newEntry;
            return newTexture;
        }
        template <typename T>
        static TextureWPtr LoadTexture(const std::vector<std::string>& relativePaths)
        {
            std::string unifiedPath = UnifyPaths(relativePaths);
            auto it = s_textureCache.find(unifiedPath);
            if (it != s_textureCache.end())
            {
                it->second.refCount++;
                return it->second.texture;
            }

            TextureSPtr newTexture = std::make_shared<T>(relativePaths);

            TextureEntry newEntry;
            newEntry.texture = newTexture;
            newEntry.refCount = 1;

            s_textureCache[unifiedPath] = newEntry;
            return newTexture;
        }

        template<typename T>
        static void UnloadTexture(const std::string& relativePath)
        {
            auto it = s_textureCache.find(relativePath);
            if (it == s_textureCache.end())
            {
                std::cout << "Warning: There is not such a texture with path " << relativePath << " to unload\n";
                return;
            }
            it->second.refCount--;

            if (it->second.refCount <=0)
            {
                s_textureCache.erase(it);
            }

        }
        static TextureWPtr GetTexture(const std::string& relativePath);
        static TextureWPtr GetTexture(const std::vector<std::string>& relativePaths);

        static MaterialWPtr CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath = diffuseTexDefaultPath, const std::string& reflectionTexPath = reflectionTexDefaultPath);
        static void DestroyMaterial(unsigned int materialId);
        static MaterialWPtr GetMaterial(unsigned int materialId);

    private:

        struct TextureEntry
        {
            TextureSPtr texture;
            unsigned int rendererIndex = 0; // No the OpenGL ID, but the index in the Renderer vector
            int refCount = 0;
        };

public:
        static std::unordered_map<std::string, TextureEntry> s_textureCache;
        static std::vector<MaterialSPtr> s_materialCache;
    };
}
