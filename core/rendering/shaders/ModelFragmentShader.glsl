#version 440 core

const int SSBOType_vertices = 0;
const int SSBOType_indices = 1;
const int SSBOType_materials = 2;
const int SSBOType_modelMatrices = 3;

struct Material
{
    float diffuseLayer;
    float specularLayer;
    float shininess;
};

layout(binding = SSBOType_materials, std430) readonly buffer ssbo2
{
    Material materials[];
};

out vec4 final_color;

smooth in vec4 frag_pos;
smooth in vec4 frag_color;
flat in double frag_materialIndex;
smooth in vec3 frag_normal;
smooth in vec2 frag_uv;

uniform vec4 u_globalLightColor;
uniform vec3 u_globalLightPos;
uniform float u_lightOffset;

uniform sampler2DArray u_texture2DArray;

void main()
{
    Material material = materials[int(frag_materialIndex)];
    vec4 diffuse = texture(u_texture2DArray, vec3(frag_uv, material.diffuseLayer));
    vec4 specular = texture(u_texture2DArray, vec3(frag_uv, material.specularLayer));

    vec4 lightColor = vec4(u_globalLightColor[0], u_globalLightColor[1], u_globalLightColor[2], u_globalLightColor[3]);

    vec3 lightPos = vec3(u_globalLightPos[0], u_globalLightPos[1], u_globalLightPos[2]);
    vec3 lightDir = normalize(vec3(frag_pos) - lightPos);

    float similarity = -dot(lightDir, frag_normal);
    float intensity = min(((similarity + 1.0) * 0.5) * material.shininess, 1);
    vec4 intensityVec = vec4(intensity, intensity, intensity, 1.0);
    // final_color = frag_color * lightColor * intensityVec;
    final_color = (frag_color * diffuse * 0.8 + specular * 0.2) * lightColor * intensityVec;
}
