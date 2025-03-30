#include "UI.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <stdexcept>
#include <iostream>
#include <cmath>

UI::UI()
{
    InitGLFW();
    InitOpenGL();
    InitImGui();
}

UI::~UI()
{
    ShutdownImGui();
    ShutdownGLFW();
    ShutdownOpenGL();
}

void UI::Render()
{
    MainUIBegin();

    RenderControls();

    //RenderImguiDemoWindow();

    MainUIEnd();
}

void UI::MainUIBegin()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    auto windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("My First Tool", &m_closing, windowFlags);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W")) { m_closing = true; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void UI::MainUIEnd()
{
    ImGui::End();
    ImGui::Render();
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(m_clearColor.x * m_clearColor.w,
        m_clearColor.y * m_clearColor.w,
        m_clearColor.z * m_clearColor.w,
        m_clearColor.w);

    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
    glfwPollEvents();
    m_closing = m_closing || glfwWindowShouldClose(m_window);
}

void UI::RenderImguiDemoWindow()
{
    ImGui::ShowDemoWindow();
}

void UI::RenderControls()
{
    // Add input password field
    static char password[64] = "";
    ImGui::InputText("Password", password, sizeof(password), ImGuiInputTextFlags_Password);

    // Get all the available platforms
    static std::vector<std::string> platforms;
    static std::vector<std::string> filteredPlatforms;

    // Add "Set seed" button
    if (ImGui::Button("Set seed"))
    {
        m_passGenerator = std::make_shared<PassGenerator>(password);
        platforms = m_passGenerator->GetPlatformManager().GetPlatforms();
        filteredPlatforms = platforms; // Initialize the filtered list
    }

    // Add platform auto-suggest field
    static char platform[64] = "";

    // Render InputText and filter the platforms list
    if (ImGui::InputText("##PlatformInput", platform, sizeof(platform)))
    {
        filteredPlatforms.clear();
        for (const auto& platformItem : platforms)
        {
            if (strncmp(platformItem.c_str(), platform, strlen(platform)) == 0)
            {
                filteredPlatforms.push_back(platformItem);
            }
        }
    }

    // Render the filtered combo box
    if (ImGui::BeginCombo("Platform", platform))
    {
        for (const auto& filteredPlatform : filteredPlatforms)
        {
            if (ImGui::Selectable(filteredPlatform.c_str()))
            {
                strcpy(platform, filteredPlatform.c_str());
            }
        }
        ImGui::EndCombo();
    }

    // Check if the text in the input field is a new platform
    if (strlen(platform) > 0 && std::find(platforms.begin(), platforms.end(), platform) == platforms.end() && ImGui::Button("Add Platform"))
    {
        // Add the new platform to the list
        platforms.push_back(platform);
    }

    // add "Generate password" button
    if (ImGui::Button("Generate password"))
    {
        try
        {
            if (m_passGenerator)
            {
                m_currentPassword = m_passGenerator->GeneratePassword(platform);
                ImGui::OpenPopup("Password Generated");
            }
        }
        catch (const std::exception& e)
        {
            ImGui::OpenPopup("Error");
        }
    }

    // Modal for password generated
    if (ImGui::BeginPopupModal("Password Generated", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Generated password, use \"Copy to clipboard\" button to use it");
        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Modal for error
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("An error occurred while generating the password.");
        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // add copy to clipboard button
    if (ImGui::Button("Copy to clipboard"))
    {
        // copy to clipboard
        ImGui::LogToClipboard();
        ImGui::LogText("%s", m_currentPassword.c_str());
        ImGui::LogFinish();
    }
}

void UI::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();
    io.DisplaySize = ImVec2(1920, 1080);
}

void UI::ShutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1920, 1080, "Spectrum Analyzer", nullptr, nullptr);

    // get error
    if (!m_window)
    {
        // get glfw error
        const char* description;
        int code = glfwGetError(&description);
        std::cout << "GLFW error: " << code << " " << description << std::endl;

        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
}

void UI::ShutdownGLFW()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void UI::InitOpenGL()
{
    int version = gladLoadGL();
    if (version == 0)
    {
        printf("Failed to initialize OpenGL context\n");
        throw std::runtime_error("Failed to initialize OpenGL context");
        return;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // for black background
}

void UI::ShutdownOpenGL()
{

}

void UI::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}
