#version 440 core

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

void main()
{
    uint realIndex = indices[gl_VertexID];
    mat4 vp = u_projection * u_view;
    vec4 position = vec4(GetPosition(realIndex), 1.0);
    frag_pos = position;
    frag_normal = normalize(GetNormal(realIndex));
    gl_Position = vp * position;
};
