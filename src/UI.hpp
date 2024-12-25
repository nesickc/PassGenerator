#pragma once
#include "imgui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class UI
{
public:
    UI();
    ~UI();

    void Render();

    bool IsClosing() const { return m_closing; }

private:
    void MainUIBegin();
    void MainUIEnd();

    void RenderImguiDemoWindow();

    void InitImGui();
    void ShutdownImGui();
    void InitGLFW();
    void ShutdownGLFW();
    void InitOpenGL();
    void ShutdownOpenGL();

private:
    static void framebufferSizeCallback( GLFWwindow* window, int width, int height );

private:
    bool m_closing = false;
    GLFWwindow* m_window = nullptr;
    ImVec4 m_clearColor = { 0.45f, 0.55f, 0.60f, 1.00f };
};