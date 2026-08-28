#version 440 core

const int SSBOType_vertices = 0;
const int SSBOType_indices = 1;
const int SSBOType_materials = 2;

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

flat in ivec3 frag_triMaterialIndices;
smooth in vec3 frag_baryWeight;

uniform vec4 u_globalLightColor;
uniform vec3 u_globalLightDir;
uniform float u_lightOffset;

uniform sampler2DArray u_texture2DArray;

void main()
{
    Material materialA = materials[frag_triMaterialIndices[0]];
    Material materialB = materials[frag_triMaterialIndices[1]];
    Material materialC = materials[frag_triMaterialIndices[2]];

    vec4 diffuseA = texture(u_texture2DArray, vec3(frag_uv, materialA.diffuseLayer));
    vec4 diffuseB = texture(u_texture2DArray, vec3(frag_uv, materialB.diffuseLayer));
    vec4 diffuseC = texture(u_texture2DArray, vec3(frag_uv, materialC.diffuseLayer));
    vec4 diffuse = diffuseA * frag_baryWeight.x + diffuseB * frag_baryWeight.y + diffuseC * frag_baryWeight.z;

    float shininess = materialA.shininess * frag_baryWeight.x + materialB.shininess * frag_baryWeight.y + materialC.shininess * frag_baryWeight.z;

    vec4 specularA = texture(u_texture2DArray, vec3(frag_uv, materialA.specularLayer));
    vec4 specularB = texture(u_texture2DArray, vec3(frag_uv, materialB.specularLayer));
    vec4 specularC = texture(u_texture2DArray, vec3(frag_uv, materialC.specularLayer));
    vec4 specular = specularA * frag_baryWeight.x + specularB * frag_baryWeight.y + specularC * frag_baryWeight.z;

    vec4 lightColor = vec4(u_globalLightColor[0], u_globalLightColor[1], u_globalLightColor[2], u_globalLightColor[3]);

    float similarity = -dot(u_globalLightDir, frag_normal);
    float intensity = min(((similarity + 1.0) * 0.5) * shininess, 1);
    vec4 intensityVec = vec4(intensity, intensity, intensity, 1.0);

    final_color = (frag_color * diffuse * 0.8 + specular * 0.2) * lightColor * intensityVec;
}
