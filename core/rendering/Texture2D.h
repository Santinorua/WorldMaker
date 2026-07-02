#pragma once

#include "Texture.h"
#include <map>
#include <typeinfo>

namespace WorldMaker
{
	class Texture2D : public Texture
	{
	private:
		int m_width, m_height, m_BPP = 0; // Bits per pixel
		unsigned int m_texture2DArrayLayer;
	public:
		float* m_floatLocalBuffer;
		unsigned char* m_unsignedCharLocalBuffer;
		Texture2D(const std::string& relativePath); // Relative path from the project root directory
		Texture2D(int width, int height, float* data);
		~Texture2D();

		void bind(unsigned int slot = 0) const override;
		void unbind() const override;

		GLenum texType() const override { return GL_TEXTURE_2D; }

		unsigned int texture2DArrayLayer() const { return m_texture2DArrayLayer; };
		int width() const { return m_width; }
		int height() const { return m_height; }
		std::string test() { return m_path; }
	};
	using Texture2DSPtr = std::shared_ptr<Texture2D>;
	using Texture2DWPtr = std::weak_ptr<Texture2D>;
}
