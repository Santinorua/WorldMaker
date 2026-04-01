#version 440 core

out vec4 final_color;

uniform sampler2D u_texture;
in vec2 frag_texCoord;

smooth in vec4 frag_pos;
smooth in vec3 frag_normal;

uniform vec4 u_globalLightColor;
uniform vec3 u_globalLightPos;
uniform float u_lightOffset;

void main()
{
    vec4 lightColor = vec4(u_globalLightColor[0], u_globalLightColor[1], u_globalLightColor[2], u_globalLightColor[3]);

    vec3 lightPos = vec3(u_globalLightPos[0], u_globalLightPos[1], u_globalLightPos[2]);
    vec3 lightDir = normalize(vec3(frag_pos) - lightPos);

    float similarity = -dot(lightDir, frag_normal);
    float intensity = min(((similarity + 1.0) * 0.5), 1);
    vec4 intensityVec = vec4(intensity, intensity, intensity, 1.0);
    final_color = vec4(1, 1, 1, 1) * vec4(lightColor * intensityVec);
}
