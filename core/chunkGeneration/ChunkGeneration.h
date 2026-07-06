#pragma once

#include "Vertex.h"
#include "ChunkRenderUnit.h"
#include <vector>
#include "PerlinNoise.h"
#include "CoreGenerator.h"

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(WorldGenerator& fractal, float x_offset, float z_offset);
void RegenerateChunks(std::vector<ChunkRenderUnit*>& chunks, WorldGenerator& generator, int render_distance, glm::vec3 camera_pos);

/* Debug */
glm::ivec2 GetGenerationRange(int pos, int render_distance);
glm::ivec2 GetChunkPos(glm::vec3 pos);

}

}
