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

smooth in vec4 frag_color;
flat in double frag_materialIndex;
smooth in vec3 frag_normal;
smooth in vec2 frag_uv;

flat in ivec3 frag_triMaterialIndices;
smooth in vec3 frag_baryWeight;

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

    vec4 specularA = texture(u_texture2DArray, vec3(frag_uv, materialA.specularLayer));
    vec4 specularB = texture(u_texture2DArray, vec3(frag_uv, materialB.specularLayer));
    vec4 specularC = texture(u_texture2DArray, vec3(frag_uv, materialC.specularLayer));
    vec4 specular = specularA * frag_baryWeight.x + specularB * frag_baryWeight.y + specularC * frag_baryWeight.z;

    final_color = (frag_color * diffuse * 0.8 + specular * 0.2);
}
