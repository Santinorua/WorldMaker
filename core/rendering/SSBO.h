#pragma once

#include "OpenGLUtils.h"
#include "DebugUtils.h"
#include "Renderer.h"
#include <vector>
#include <memory>

namespace WorldMaker
{
    enum SSBOType
    {
        vertices = 0,
        indices = 1,
        materials = 2,
        modelMatrices = 3
    };

    template<typename T>
	class SSBO
	{
	private:
		unsigned int m_glName;
		unsigned int m_maxBytes = 0;
		unsigned int m_currentBytes = 0;
		unsigned int m_usage = GL_DYNAMIC_STORAGE_BIT;
	public:
		std::vector<T> m_data = {};

		void resize(unsigned int elementsToSupport)
		{
		    ASSERT(elementsToSupport >= currentElements());
            unsigned int newSSBO;
            GLCall(glGenBuffers(1, &newSSBO));
			GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO));
			GLCall(
     			glBufferStorage
     			(
    				GL_SHADER_STORAGE_BUFFER,
    				sizeof(T) * elementsToSupport,
    				0,
    				m_usage
     			));
			GLCall(glBindBuffer(GL_COPY_READ_BUFFER, m_glName));
			GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO));
			GLCall(glCopyBufferSubData(
                GL_COPY_READ_BUFFER,
                GL_COPY_WRITE_BUFFER,
                0,
                0,
                m_currentBytes
            ));
            m_maxBytes = elementsToSupport * sizeof(T);
            GLCall(glDeleteBuffers(1, &m_glName));
            m_glName = newSSBO;
		}
		SSBO(unsigned int maxCount, unsigned int usage)
		{
			GLCall(glGenBuffers(1, &m_glName));
			bind();
			GLCall(
				glBufferStorage
				(
					GL_SHADER_STORAGE_BUFFER,
					sizeof(T) * maxCount,
					0,
					usage
				));
			m_maxBytes = maxCount * sizeof(T);
			m_usage = usage;
		}
		~SSBO()
		{
			GLCall(glDeleteBuffers(1, &m_glName));
		}

		void bind()
		{
		    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_glName));
		}

		// Should be used for buffers from the renderer batch
		void pushData(const std::vector<T>& data)
		{
		    if (currentElements() + data.size() > maxElements()) resize(currentElements()+data.size());
			m_data.insert(m_data.end(), data.begin(), data.end());
			m_currentBytes += data.size() * sizeof(T);
		}
		void pushData(const T data)
		{
		    if (currentElements() + 1 > maxElements()) resize(currentElements()+1);
            m_data.push_back(data);
            m_currentBytes+=sizeof(T);
		}
		// Shouldn't be used when using batch rendering
		void submitData()
		{
			ASSERT(m_data.size() * sizeof(T) <= m_maxBytes);
			bind();
			GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_data.size()*sizeof(T), m_data.data()));
		}
		void submitSubData(unsigned int beginning, unsigned int end) // indexes of elements, not bytes
		{
    		ASSERT(m_data.size() * sizeof(T) <= m_maxBytes);
            ASSERT(end>=beginning);
    		bind();
            size_t count = end - beginning + 1;
    		GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, beginning*sizeof(T), count*sizeof(T), m_data.data()+beginning));
		}
		// Puts the data from m_data into the actual SSBO
		void bindBufferBase(const int& binding)
		{
			GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_glName));
		}

		void flush()
		{
			m_data.clear();
			m_currentBytes = 0;
		}

		bool checkIfEnoughSpaceForPush(unsigned int elementsToPushAmount)
		{
		    if (currentSize() + elementsToPushAmount * sizeof(T) > maxSize()) return false;
			return true;
		}
		bool checkIfPushIsBiggerThanMaxSize(unsigned int elementsToAddAmount) // Think of a better name
		{
		    return elementsToAddAmount * sizeof(T) > maxSize();
		}
		unsigned int glName() const { return m_glName; }
		unsigned int currentSize() const { return m_currentBytes; }
		unsigned int maxSize() const { return m_maxBytes; }
		unsigned int maxElements() const { return m_maxBytes/sizeof(T); }
		unsigned int currentElements() const { return m_data.size(); }

		friend class ChunkRenderUnit;
	};
	template<typename T>
	using SSBOUPtr = std::unique_ptr<SSBO<T>>;
	template<typename T>
	using SSBOSPtr = std::shared_ptr<SSBO<T>>;
	using MatricesSSBO = SSBOSPtr<glm::mat4>;
}
