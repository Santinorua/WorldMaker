#pragma once

#include "ChunkRenderUnit.h"
#include "CoreGenerator.h"
#include <atomic>
#include <climits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace WorldMaker::ChunkGeneration {

using ChunkArray = std::vector<std::pair<glm::ivec2, ChunkRenderUnit*>>;

// Captured on the main thread. Workers never read editable globals or touch GL.
struct GenerationSettings {
    WorldGenerator generator;
    std::vector<Biome> biomes;
    std::vector<Feature> features;
    int resolution;
    double scale;
    explicit GenerationSettings(const WorldGenerator& source);
};

struct ChunkRequest {
    glm::ivec2 position;
    int lod;
};

struct ModelInstance {
    std::string path;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct ChunkData {
    ChunkRequest request;
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<ModelInstance> models;
    double tallest = 0;
    double lowest = 0;
};

bool GenerateChunkData(const GenerationSettings& settings, ChunkRequest request,
                       ChunkData& data, const std::atomic_bool& cancelled);

// Requests are supplied in priority order. Replacing them cancels obsolete work;
// reset also invalidates running/finished work from a previous world revision.
class ChunkWorkerPool {
public:
    explicit ChunkWorkerPool(unsigned int workerCount = 0);
    ~ChunkWorkerPool();
    void request(const std::vector<ChunkRequest>& requests,
                 std::shared_ptr<const GenerationSettings> settings);
    bool tryPop(ChunkData& data);
    void reset();
    void shutdown();
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

class ChunkStreamer {
public:
    // Call every frame, including when the camera is stationary.
    void update(ChunkArray& chunks, const WorldGenerator& generator, int renderDistance,
                glm::vec3 cameraPosition, bool force = false);
    void shutdown() { m_workers.shutdown(); m_modelCache.clear(); }
private:
    ChunkWorkerPool m_workers;
    // Camera movement keeps the active world; UI edits apply on regeneration.
    std::shared_ptr<const GenerationSettings> m_settings;
    // ResourceManager holds weak references. Keep streamed assets alive so that
    // leaving/re-entering a forest doesn't reload the same model from disk.
    std::unordered_map<std::string, ModelSPtr> m_modelCache;
    glm::ivec2 m_lastPosition = {INT_MAX, INT_MAX};
    int m_lastDistance = -1;
    int m_lastResolution = -1;
    double m_lastScale = -1;
    uint64_t m_lastSeed = 0;
};

glm::ivec2 GetGenerationRange(int pos, int renderDistance);
glm::ivec2 GetChunkPos(glm::vec3 pos);

}
