#version 450 core

out vec4 final_color;

uniform sampler2D u_texture;
in vec2 frag_texCoord;

void main()
{
    final_color = vec4(1, 1, 1, 1);
}
