#pragma once

#include "Vertex.h"
#include "ChunkRenderUnit.h"
#include <vector>
#include "PerlinNoise.h"
#include "CoreGenerator.h"

namespace WorldMaker {

namespace ChunkGeneration {

using ChunkArray = std::vector<std::pair<glm::ivec2, ChunkRenderUnit*>>;

ChunkRenderUnit* GenerateChunk(WorldGenerator& fractal, int x_chunk, int z_chunk);
void RegenerateChunks(ChunkArray& chunks, WorldGenerator& generator, int render_distance, glm::vec3 camera_pos);

/* Debug */
glm::ivec2 GetGenerationRange(int pos, int render_distance);
glm::ivec2 GetChunkPos(glm::vec3 pos);

}

}
