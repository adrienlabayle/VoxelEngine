#pragma once
#define GLFW_INCLUDE_NONE // to not have inclusion errors
#include <GLFW/glfw3.h>

#include "Renderer.h"

#include <vector>
#include <functional>
#include <string>
#include <iostream>

#include <Vertex.h>

namespace test {

	class Test
	{
	public:
		Test(GLFWwindow* window);
		virtual ~Test() {}

		virtual void OnUpdate() {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
		virtual void OnResize() {}
	protected:
		GLFWwindow* m_Window=nullptr;
	};



	class TestMenu : public Test
	{
	public:
		TestMenu(GLFWwindow* window, Test*& currentTestPointer);

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "Registering test " << name << std::endl;

			m_Tests.push_back(std::make_pair(name, [window = m_Window]() { return new T(window); }));
		}

	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}