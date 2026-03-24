#version 450 core

// #extension GL_ARB_bindless_texture : require
// #extension GL_ARB_gpu_shader_int64 : enable

out vec4 final_color;

uniform sampler2D u_texture;
in vec2 frag_texCoord;

void main()
{
    final_color = texture(u_texture, frag_texCoord);
}
