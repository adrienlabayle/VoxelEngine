#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position)
    : m_Position(position),
    m_Pitch(0.0f),
    m_Yaw(-90.0f),
    m_Speed(15.0f),
    m_Sensitivity(0.1f)
{

}

Camera::~Camera()
{

}

void Camera::MoveForward(float dt)
{
    m_Position += GetForward() * m_Speed * dt;
}

void Camera::MoveBackward(float dt)
{
    m_Position -= GetForward() * m_Speed * dt;
}

void Camera::MoveRight(float dt)
{
    m_Position += GetRight() * m_Speed * dt;
}

void Camera::MoveLeft(float dt)
{
    m_Position -= GetRight() * m_Speed * dt;
}

void Camera::MoveUp(float dt)
{
    m_Position += glm::vec3(0, 1, 0) * m_Speed * dt;
}

void Camera::MoveDown(float dt)
{
    m_Position -= glm::vec3(0, 1, 0) * m_Speed * dt;
}



void Camera::AddYaw(float offset)
{
    m_Yaw += offset * m_Sensitivity;
}

void Camera::AddPitch(float offset)
{
    m_Pitch += offset * m_Sensitivity;

    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;
}



glm::vec3 Camera::GetForward() const
{
    glm::vec3 Forward;

    Forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    Forward.y = sin(glm::radians(m_Pitch));
    Forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    return glm::normalize(Forward);
}

glm::vec3 Camera::GetRight() const
{
    return glm::normalize(glm::cross(GetForward(), glm::vec3(0, 1, 0)));
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + GetForward(), glm::vec3(0, 1, 0));
}