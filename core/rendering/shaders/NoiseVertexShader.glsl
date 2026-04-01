#version 440 core

// #extension GL_ARB_bindless_texture : require
// #extension GL_ARB_gpu_shader_int64 : enable

const int SSBOType_vertices = 0;
const int SSBOType_indices = 1;

struct VertexData
{
    double position[3];
    double normal[3];
};

layout(binding = SSBOType_vertices, std430) readonly buffer ssbo0
{
    VertexData data[];
};

layout(binding = SSBOType_indices, std430) readonly buffer ssbo1
{
    uint indices[];
};

//uniform mat4 u_view;
//uniform mat4 u_projection;
out vec2 frag_texCoord;

vec3 GetPosition(uint index)
{
    return vec3(
        data[index].position[0],
        data[index].position[1],
        data[index].position[2]
    );
}

vec3 GetNormal(uint index)
{
    return vec3(
        data[index].normal[0],
        data[index].normal[1],
        data[index].normal[2]
    );
}

void main()
{
    uint realIndex = indices[gl_VertexID];
    vec4 position = vec4(GetPosition(realIndex), 1.0);
    gl_Position = position;
    if (realIndex == 0) frag_texCoord = vec2(0.0, 0.0);
    else if (realIndex == 1) frag_texCoord = vec2(1.0, 0.0);
    else if (realIndex == 2) frag_texCoord = vec2(1.0, 1.0);
    else if (realIndex == 3) frag_texCoord = vec2(0.0, 1.0);
};
