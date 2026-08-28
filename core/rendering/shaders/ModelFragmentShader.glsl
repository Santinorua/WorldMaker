#version 440 core

layout(binding = 0) uniform sampler2D u_diffuse;
layout(binding = 1) uniform sampler2D u_specular;

out vec4 final_color;

smooth in vec4 frag_pos;
smooth in vec4 frag_color;
smooth in vec3 frag_normal;
smooth in vec2 frag_uv;

uniform vec4 u_globalLightColor;
uniform vec3 u_globalLightDir;
uniform float u_lightOffset;
uniform float u_shininess;

void main()
{
    vec4 diffuse = texture(u_diffuse, frag_uv);
    if (diffuse.a < 0.5) discard;
    vec4 specular = texture(u_specular, frag_uv);

    vec4 lightColor = vec4(u_globalLightColor[0], u_globalLightColor[1], u_globalLightColor[2], u_globalLightColor[3]);

    float similarity = normalize(-dot(u_globalLightDir, frag_normal));
    float lambert = clamp(((similarity + 1.0) * 0.5) * u_shininess, 0.0, 1.0);
    float intensity = mix(u_lightOffset, 1.0, lambert);
    vec4 intensityVec = vec4(intensity, intensity, intensity, 1.0);
    final_color = (frag_color * diffuse * 0.8 + specular * 0.2) * lightColor * intensityVec;
}
