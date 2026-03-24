#include "NoiseRenderUnit.h"

namespace WorldMaker
{
	NoiseRenderUnit::NoiseRenderUnit(const int& p_width, const int& p_height, std::vector<double>& p_data) : width(p_width), height(p_height)
	{
		colorVector = p_data;
		std::vector<float> hola;
		for (double d : p_data)
		{
			hola.push_back(static_cast<float>(d));
		}
		noiseTex = std::make_shared<Texture2D>(width, height, hola.data());
		vertices->addBatchData(quad);
		vertices->submitData();
		indices->addBatchData(quadIndices);
		indices->submitData();
	}
	void NoiseRenderUnit::ChangeNoise(const int& p_width, const int& p_height, std::vector<double>& p_data)
	{
		colorVector = p_data;
		std::vector<float> hola;
		for (double d : p_data)
		{
			hola.push_back(static_cast<float>(d));
		}
		noiseTex = std::make_shared<Texture2D>(width, height, hola.data());
		vertices->flush();
		vertices->addBatchData(quad);
		vertices->submitData();
		indices->flush();
		indices->addBatchData(quadIndices);
		indices->submitData();
	}
}
