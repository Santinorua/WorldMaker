#version 440 core

const int SSBOType_vertices = 0;
const int SSBOType_indices = 1;
const int SSBOType_materials = 2;

out vec4 final_color;

uniform uint u_chunkID;

int GetChunkIDColor(int pos)
{
	return int(uint(u_chunkID >> 8 * pos) & 0xff);
}

void main()
{
    final_color = vec4(GetChunkIDColor(0)/255.0, GetChunkIDColor(1)/255.0, GetChunkIDColor(2)/255.0, 1.0);
}
