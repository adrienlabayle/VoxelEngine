#include "Test.h"
#include "imgui/imgui.h"

namespace test {

	Test::Test(GLFWwindow* window)
		: m_Window(window)
	{

	}

	TestMenu::TestMenu(GLFWwindow* window, Test*& currentTestPointer)
		: Test(window), m_CurrentTest(currentTestPointer)
	{

	}

	void TestMenu::OnImGuiRender()
	{
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
				m_CurrentTest = test.second();
		}
	}
}