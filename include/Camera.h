#pragma once

#include "glm/glm.hpp"
#include<iostream>

class Camera
{
private:
    glm::vec3 m_Position;

    float m_Pitch; // rotation up/down
    float m_Yaw;   // rotation left/right

    float m_Speed;
    float m_Sensitivity;

public:
    Camera(glm::vec3 position);
    ~Camera();

    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;

    inline glm::vec3 GetPosition() const { return m_Position; }

    void MoveForward(float deltaTime);
    void MoveBackward(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);

    void AddYaw(float offset);
    void AddPitch(float offset);
    
};