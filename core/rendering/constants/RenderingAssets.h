#pragma once

#include <iostream>

namespace WorldMaker
{
    inline std::string diffuseTexEmptyPath = "core/rendering/assets/textures/empty.png";
    inline std::string diffuseTexDefaultPath = "core/rendering/assets/textures/default.png";
    inline std::string specularTexDefaultPath = "core/rendering/assets/textures/default.png";
    inline std::string diffuseTexDefaultGrassPath = "core/rendering/assets/textures/defaultGrass.png";
    inline std::string diffuseTexDefaultSandPath = "core/rendering/assets/textures/defaultSand.jpg";
    inline std::string diffuseTexDefaultWaterPath = "core/rendering/assets/textures/water.png";

    inline std::string skyboxDefaultPath = "core/rendering/assets/textures/skybox.png";

    inline std::string noiseVertexShaderPath = "core/rendering/shaders/NoiseVertexShader.glsl";
    inline std::string noiseFragmentShaderPath = "core/rendering/shaders/NoiseFragmentShader.glsl";
    inline std::string TerrainVertexShaderPath = "core/rendering/shaders/TerrainVertexShader.glsl";
    inline std::string terrainFragmentShaderPath = "core/rendering/shaders/TerrainFragmentShader.glsl";
    inline std::string modelVertexShaderPath = "core/rendering/shaders/ModelVertexShader.glsl";
    inline std::string modelFragmentShaderPath = "core/rendering/shaders/ModelFragmentShader.glsl";
    inline std::string bakingVertexShaderPath = "core/rendering/shaders/BakingVertexShader.glsl";
    inline std::string bakingFragmentShaderPath = "core/rendering/shaders/BakingFragmentShader.glsl";
    inline std::string waterVertexShaderPath = "core/rendering/shaders/WaterVertexShader.glsl";
    inline std::string waterFragmentShaderPath = "core/rendering/shaders/WaterFragmentShader.glsl";
    inline std::string skyboxVertexShaderPath = "core/rendering/shaders/SkyboxVertexShader.glsl";
    inline std::string skyboxFragmentShaderPath = "core/rendering/shaders/SkyboxFragmentShader.glsl";
}
