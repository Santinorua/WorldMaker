#version 440 core

const int SSBOType_vertices = 0;
const int SSBOType_indices = 1;
const int SSBOType_materials = 2;
const int SSBOType_modelMatrices = 3;

struct VertexData
{
    double position[3];
    double color[4];
    double materialIndex;
    double uv[2];
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
layout(binding = SSBOType_modelMatrices, std430) readonly buffer ssbo3
{
    mat4 modelMatrices[];
};
uniform mat4 u_view;
uniform mat4 u_projection;

vec3 GetPosition(uint index)
{
    return vec3(
        data[index].position[0],
        data[index].position[1],
        data[index].position[2]
    );
}

vec4 GetColor(uint index)
{
    return vec4(
        data[index].color[0],
        data[index].color[1],
        data[index].color[2],
        data[index].color[3]
    );
}

double GetMaterialIndex(uint index)
{
    return data[index].materialIndex;
}

vec2 GetUV(uint index)
{
    return vec2(
        data[index].uv[0],
        data[index].uv[1]
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

smooth out vec3 frag_normal;
smooth out vec4 frag_pos;
smooth out vec4 frag_color;
smooth out vec2 frag_uv;

void main()
{
    uint realIndex = indices[gl_VertexID];
    mat4 vp = u_projection * u_view;
    vec4 position = vec4(GetPosition(realIndex), 1.0);
    frag_color = GetColor(realIndex);
    frag_uv = GetUV(realIndex);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrices[gl_InstanceID])));
    frag_normal = normalMatrix * GetNormal(realIndex);
    gl_Position = vp * modelMatrices[gl_InstanceID] * position;
    frag_pos = modelMatrices[gl_InstanceID] * position;
}
