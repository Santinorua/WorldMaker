#include "Camera.h"
#include "Input.h"
#include "CoolTime.h"

#include <algorithm>

namespace WorldMaker
{
	const glm::vec3 Camera::s_up = glm::vec3(0.0, 1.0, 0.0);
	float Camera::s_nearPlane = 0.1f;
	float Camera::s_farPlane = 3000.0f;
	float Camera::s_fov = 45.0f;
	float Camera::s_aspectRatio = 800.0f/600.0f;
	glm::vec3 Camera::s_pos = glm::vec3(0,0,0);
	glm::vec3 Camera::s_rot = glm::vec3(0,0,0);
	float Camera::s_speed = 50.0f;
	float Camera::s_rotationSpeed = 100.0f;

	glm::mat4 Camera::ProjectionMatrix()
	{
		return glm::perspective(glm::radians(s_fov), s_aspectRatio, s_nearPlane, s_farPlane);
	}
	glm::mat4 Camera::ViewMatrix()
	{
		glm::vec3 pos = s_pos;
		glm::vec3 eulerRadians = glm::radians(s_rot);
		glm::vec3 direction;

		float pitch = eulerRadians.x;
		float yaw = eulerRadians.y;

		direction.x = cos(pitch) * sin(yaw);
		direction.y = sin(pitch);
		direction.z = cos(pitch) * cos(yaw);

		direction = glm::normalize(direction);
		return glm::lookAt(pos, pos + direction, s_up);
	};

	void Camera::UpdateCameraTransform()
	{
		Vec2 rotation = Vec2(0.0f, 0.0f);

		if (Input::GetKey(KeyCode::Down_Arrow)) rotation.x -= 1;
		if (Input::GetKey(KeyCode::Up_Arrow)) rotation.x += 1;
		if (Input::GetKey(KeyCode::Left_Arrow)) rotation.y += 1;
		if (Input::GetKey(KeyCode::Right_Arrow)) rotation.y -= 1;


		s_rot += static_cast<glm::vec3>(rotation * CoolTime::DeltaTime() * s_rotationSpeed);
		s_rot.x = std::clamp(s_rot.x, -89.0f, 89.0f);

		Vec3 horizontalDirection = {0, 0, 0};
		if (Input::GetKey(KeyCode::W_Key)) horizontalDirection.z += 1;
		if (Input::GetKey(KeyCode::S_Key)) horizontalDirection.z -= 1;
		if (Input::GetKey(KeyCode::A_Key)) horizontalDirection.x -= 1;
		if (Input::GetKey(KeyCode::D_Key)) horizontalDirection.x += 1;

		Vec3 direction = Vec3::GetDirectionFromEuler(s_rot);

		Vec3 left = Vec3::Up().CrossProduct(direction).Normalized();
		s_pos += static_cast<glm::vec3>(direction * horizontalDirection.z * CoolTime::DeltaTime() * s_speed);
		s_pos += static_cast<glm::vec3>(left * -horizontalDirection.x * CoolTime::DeltaTime() * s_speed);

	}
}
