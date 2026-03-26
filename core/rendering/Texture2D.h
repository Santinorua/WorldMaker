#pragma once

#include "Texture.h"
#include <map>
#include <typeinfo>

namespace WorldMaker
{
	class Texture2D : public Texture
	{
	private:
		int m_width, m_height, m_BPP; // Bits per pixel

	public:
		float* m_floatLocalBuffer;
		unsigned char* m_unsignedCharLocalBuffer;
		Texture2D(const std::string& relativePath); // Relative path from the project root directory
		Texture2D(int width, int height, float* data);
		~Texture2D();

		void bind(unsigned int slot = 0) const override;
		void unbind() const override;

		GLenum texType() const override { return GL_TEXTURE_2D; }

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }
	};
	using Texture2DSPtr = std::shared_ptr<Texture2D>;
}
