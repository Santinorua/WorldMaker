#include "Camera.h"
#include "Input.h"
#include "CoolTime.h"

#include <algorithm>

namespace WorldMaker
{
	const glm::vec3 Camera::s_globalUp = glm::vec3(0.0, 1.0, 0.0);
	float Camera::s_nearPlane = 0.1f;
	float Camera::s_farPlane = 3000.0f;
	float Camera::s_fov = 45.0f;
	float Camera::s_aspectRatio = 800.0f/600.0f;
	glm::vec3 Camera::s_pos = glm::vec3(-40, 55, -50);
	glm::vec3 Camera::s_rot = glm::vec3(-14.3, 38.5, 0);
	float Camera::s_speed = 50.0f;
	float Camera::s_rotationSpeed = 0.1f;
	Frustum Camera::s_frustum;

	glm::mat4 Camera::ProjectionMatrix()
	{
		return glm::perspective(glm::radians(s_fov), s_aspectRatio, s_nearPlane, s_farPlane);
	}
	glm::mat4 Camera::ViewMatrix()
	{
		return glm::lookAt(s_pos, s_pos + Front(), s_globalUp);
	};

	glm::vec3 Camera::Front()
	{
        float pitch = glm::radians(s_rot.x);
        float yaw   = glm::radians(s_rot.y);

        glm::vec3 front;
        front.x = cos(pitch) * sin(yaw);
        front.y = sin(pitch);
        front.z = cos(pitch) * cos(yaw);

        return glm::normalize(front);
	}
	glm::vec3 Camera::Back()
	{
	    return -Front();
	}
	glm::vec3 Camera::Up()
	{
        return glm::normalize(
                glm::cross(Front(), Right())
            );
	}
    glm::vec3 Camera::Down()
    {
        return -Up();
    }
    glm::vec3 Camera::Left()
    {
        return -Right();
    }
    glm::vec3 Camera::Right()
    {
        return glm::normalize(
                glm::cross(s_globalUp, Front())
            );
    }

    bool Camera::CanSeeSphere(glm::vec3 center, float radius){return s_frustum.sphereInside(center, radius);}
    bool Camera::CanSeeBox(glm::vec3 min, glm::vec3 max) { return s_frustum.boxInside(min, max);}
	void Camera::UpdateCameraTransform()
	{
	    static bool firstClick = true;

        if (!Input::GetMouseButtonDown(MouseCode::Mouse_Right))
        {
            if (Input::GetCursorMode()==CursorMode::MouseDisabled)
            {
                Input::SetCursorMode(CursorMode::MouseNormal);
            }
            firstClick = true;
            return;
        }

        if (Input::GetCursorMode() != CursorMode::MouseDisabled)
        {
            Input::SetCursorMode(CursorMode::MouseDisabled);
        }
        glm::vec2 mouseRotation = Vec2(0.0f, 0.0f);
        glm::vec2 mouseTranslation = Input::GetMouseDeltaPix();
        if (firstClick)
        {
            mouseTranslation = {0,0};
            firstClick = false;
        }
        mouseRotation.x -= mouseTranslation.y;
        mouseRotation.y -= mouseTranslation.x;

        s_rot += glm::vec3(mouseRotation.x, mouseRotation.y, 0) * s_rotationSpeed;
       	s_rot.x = std::clamp(s_rot.x, -89.0f, 89.0f);

       	glm::vec3 horizontalDirection = {0, 0, 0};
       	if (Input::GetKey(KeyCode::W_Key)) horizontalDirection.z += 1;
       	if (Input::GetKey(KeyCode::S_Key)) horizontalDirection.z -= 1;
       	if (Input::GetKey(KeyCode::A_Key)) horizontalDirection.x -= 1;
       	if (Input::GetKey(KeyCode::D_Key)) horizontalDirection.x += 1;

       	s_pos += static_cast<glm::vec3>(Front() * horizontalDirection.z * CoolTime::DeltaTime() * s_speed);
       	s_pos += static_cast<glm::vec3>(Left() * horizontalDirection.x * CoolTime::DeltaTime() * s_speed);
    }
}
