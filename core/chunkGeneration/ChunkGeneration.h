#pragma once

#include "Vertex.h"
#include "ChunkRenderUnit.h"
#include <vector>
#include "PerlinNoise.h"
#include "CoreGenerator.h"

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(WorldGenerator& fractal, float x_offset, float z_offset);
void RegenerateChunks(std::vector<ChunkRenderUnit*>& chunks, WorldGenerator& generator, int width, int height);

}

}
