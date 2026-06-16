#pragma once

#include "Frustum.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Vec2.h"
#include "Vec3.h"

namespace WorldMaker
{
	class Camera
	{
		static const glm::vec3 s_globalUp;
		static float s_nearPlane;
		static float s_farPlane;
		static float s_fov;
		static float s_aspectRatio;
		static glm::vec3 s_pos;
		static glm::vec3 s_rot;
		static float s_speed;
		static float s_rotationSpeed;
		static Frustum s_frustum;
	public:
		static inline glm::vec3 Position() { return s_pos;}
		static inline glm::vec3 Rotation() { return s_rot;}
		static inline float NearPlane() { return s_nearPlane; }
		static inline float FarPlane() { return s_farPlane; }
		static inline float FOV() { return s_fov; }
		static inline float AspectRatio() { return s_aspectRatio; }

		static bool CanSeeSphere(glm::vec3 center, float radius);
		static bool CanSeeBox(glm::vec3 min, glm::vec3 max);
		static void UpdateCameraTransform();
		static glm::mat4 ProjectionMatrix();
		static glm::mat4 ViewMatrix();
		static glm::vec3 Front();
		static glm::vec3 Back();
		static glm::vec3 Up();
		static glm::vec3 Down();
		static glm::vec3 Left();
		static glm::vec3 Right();
	};
}
