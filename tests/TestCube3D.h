#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Atlas.h"
#include "Mesh.h"
#include "Camera.h"
#include "World.h"

#include <memory>

namespace test {

	class TestCube3D : public Test
	{
	public:
		TestCube3D(GLFWwindow* window);
		~TestCube3D();

		void OnUpdate() override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnResize() override;

		float GetDeltaTime();
		void GetMouseOffset(double mouseX, double mouseY);

	private:
		std::shared_ptr<Atlas> m_Texture;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<World> m_World;

		glm::mat4 m_Proj, m_View, m_Model;
	};
}