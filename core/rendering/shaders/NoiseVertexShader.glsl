#version 440 core

// #extension GL_ARB_bindless_texture : require
// #extension GL_ARB_gpu_shader_int64 : enable

const int SSBOType_vertices = 0;
const int SSBOType_indices = 1;

struct VertexData
{
    double position[3];
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

void main()
{
    uint realIndex = indices[gl_VertexID];
    vec4 position = vec4(GetPosition(realIndex), 1.0);
    gl_Position = position;
    frag_texCoord = vec2(position.x + 0.5, position.y + 0.5);
};
