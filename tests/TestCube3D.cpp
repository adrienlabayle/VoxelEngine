#include "TestCube3D.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

	TestCube3D::TestCube3D(GLFWwindow* window)
        : Test(window), m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f))
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));  // pour gerer le alpha

        // Active le face culling               //here is the reason why we fill the vbo indicies with a certain order for heach faces
        GLCall(glEnable(GL_CULL_FACE));
        GLCall(glCullFace(GL_FRONT));       // on ne dessine que les faces avant
        GLCall(glFrontFace(GL_CCW));       // orientation antihoraire = face avant
        
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // pour ne pas que le curser de la souri soit visible et que lon soit bloquer par les bord de lecran

        glEnable(GL_DEPTH_TEST);  // allow opengl to applie depth notion
        


        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();

        m_Texture = std::make_shared<Atlas>("res/textures/ATLAS.png", 16, 16);  // WATCHOUT it must follow the atlas structure !
        m_Shader->SetUniform1i("u_Texture", 0);

        m_World = std::make_unique<World>(20, m_Texture, 8); //RenderDistance, Atlas, Seed

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        m_Proj = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 1000.0f);  // FOV, Width, Height, near, far

        m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 140.0f, 0.0f));
	}

	TestCube3D::~TestCube3D()
	{
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
	}



	void TestCube3D::OnUpdate()
	{
        // here i do glfw stuff plus rebuild mesh for chunk who change and reput the data in the buffers if then

        float DeltaTime = GetDeltaTime();


        if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
            m_Camera->MoveForward(DeltaTime);

        if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
            m_Camera->MoveBackward(DeltaTime);

        if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
            m_Camera->MoveRight(DeltaTime);

        if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
            m_Camera->MoveLeft(DeltaTime);

        if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_Camera->MoveUp(DeltaTime);

        if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            m_Camera->MoveDown(DeltaTime);



        double mouseX, mouseY;
        glfwGetCursorPos(m_Window, &mouseX, &mouseY);
        GetMouseOffset(mouseX, mouseY);
	}



	void TestCube3D::OnRender()
	{
        GLCall(glClearColor(70.0f/255.0f, 190.0f/255.0f, 220.0f/255.0f, 1.0f));  //blue sky
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        Renderer renderer;

        m_Texture->Bind(0);

        m_View = m_Camera->GetViewMatrix();
        glm::mat4 mvp = m_Proj * m_View * m_Model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        m_World->Draw(m_Camera->GetPosition(), m_Shader.get(), m_Camera->GetViewMatrix(), m_Proj);
	}



	void TestCube3D::OnImGuiRender()
	{
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Coords : %.1fX   %.1fY   %.1fZ", m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z);
        ImGui::Text("GetHeight(x, z) : %d", m_World->GetHeight((int)std::floor(m_Camera->GetPosition().x), (int)std::floor(m_Camera->GetPosition().z)));
	}



    void TestCube3D::OnResize()
    {
        /*
        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);

        glViewport(0, 0, width, height);
        m_Proj = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 100.0f);
        */
    }



    float TestCube3D::GetDeltaTime()
    {
        static float lastFrame = 0.0f;      // static var, so stay in memory (initialised just at the 1st call of the fct)
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return deltaTime;
    }

    void TestCube3D::GetMouseOffset(double mouseX, double mouseY)
    {
        static double lastX = mouseX;   // static var, so stay in memory
        static double lastY = mouseY;

        float xOffset = mouseX - lastX;
        float yOffset = lastY - mouseY;

        lastX = mouseX;
        lastY = mouseY;

        m_Camera->AddYaw(xOffset);
        m_Camera->AddPitch(yOffset);
    }
}