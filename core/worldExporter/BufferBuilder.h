#pragma once

#include <vector>

#include "tiny_gltf.h"

namespace WorldMaker
{
    struct BufferBuilder
    {
        tinygltf::Buffer buffer;

        template <typename T>
        size_t AddBlock(std::vector<T>& data)
        {
            size_t offset = buffer.data.size();
            size_t byteLength = data.size() * sizeof(T);

            size_t oldSize = buffer.data.size();
            buffer.data.resize(oldSize + byteLength);
            memcpy(buffer.data.data() + oldSize, data.data(), byteLength);

            return offset;
        }
    };
}
