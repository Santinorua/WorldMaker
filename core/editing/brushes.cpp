#include <brushes.h>
#include <Camera.h>

namespace WorldMaker {

glm::vec3 RaycastChunk(const ChunkRenderUnit& chunk, const Ray& ray)
{
	auto bounding_box = chunk.getBoundingBox();

	float t_min = -std::numeric_limits<float>::infinity();
	float t_max = std::numeric_limits<float>::infinity();

	for (int i = 0; i < 3; i++) {
		float from = ray.from[i];
		float dir = ray.direction[i];

		if (std::abs(dir) < 1e-8) {
			if (from < bounding_box.first[i] || from > bounding_box.second[i]) {
				return glm::vec3(0, 0, 0);
			}
			continue;
		}

		float t1 = (bounding_box.first[i] - from) / dir;
		float t2 = (bounding_box.second[i] - from) / dir;

		if (t1 > t2)
			std::swap(t1, t2);

		t_min = std::max(t_min, t1);
		t_max = std::min(t_max, t2);

		if (t_min > t_max) {
			return glm::vec3(0, 0, 0);
		}
	}

	if (t_max < 0.0f) {
		return glm::vec3(0, 0, 0);
	}

	float t = (t_min >= 0.0f) ? t_min : t_max;
	return ray.from + t * ray.direction;
}

glm::vec3 ScreenRaycast(const ChunkRenderUnit& chunk, glm::vec2 mouse_pos, glm::uvec2 screen_size)
{
	float x = (2.0f * static_cast<float>(mouse_pos.x)) / screen_size.x - 1.0f;
	float y = 1.0f - (2.0f * static_cast<float>(mouse_pos.y)) / screen_size.y;

	glm::vec4 clipped = glm::vec4(x, y, -1.0f, 1.0f);
	glm::vec4 eye = glm::inverse(Camera::ProjectionMatrix()) * clipped;
	eye.z = -1.0;
	eye.w = 0;

	glm::vec3 ray_world = glm::vec3(glm::inverse(Camera::ViewMatrix()) * eye);

	Ray ray = Ray { Camera::Position(), glm::normalize(ray_world) };

	return RaycastChunk(chunk, ray);
}

}
