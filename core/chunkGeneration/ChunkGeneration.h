#pragma once

#include "Vertex.h"
#include "ChunkRenderUnit.h"
#include <vector>
#include "PerlinNoise.h"

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(FractalNoise& fractal, float x_offset, float z_offset);
void RegenerateChunks(std::vector<ChunkRenderUnit*>& chunks, FractalNoise& fractal, int width, int height);

}

}
