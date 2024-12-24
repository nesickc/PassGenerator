#include "UI.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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

    RenderImguiDemoWindow();

    MainUIEnd();
}

void UI::MainUIBegin()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize( ImGui::GetIO().DisplaySize );
    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );

    auto windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin( "My First Tool", &m_closing, windowFlags );
    if ( ImGui::BeginMenuBar() )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( ImGui::MenuItem( "Open..", "Ctrl+O" ) ) { /* Do stuff */ }
            if ( ImGui::MenuItem( "Save", "Ctrl+S" ) ) { /* Do stuff */ }
            if ( ImGui::MenuItem( "Close", "Ctrl+W" ) ) { m_closing = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void UI::MainUIEnd()
{
    ImGui::End();
    ImGui::Render();
    glViewport( 0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y );
    glClearColor( m_clearColor.x * m_clearColor.w,
                  m_clearColor.y * m_clearColor.w,
                  m_clearColor.z * m_clearColor.w,
                  m_clearColor.w );

    glClear( GL_COLOR_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    glfwSwapBuffers( m_window );
    glfwPollEvents();
    m_closing = m_closing || glfwWindowShouldClose( m_window );
}

void UI::RenderImguiDemoWindow()
{
    ImGui::ShowDemoWindow();
}

void UI::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( m_window, true );
    ImGui_ImplOpenGL3_Init();
    io.DisplaySize = ImVec2( 1920, 1080 );
}

void UI::ShutdownImGui()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::InitGLFW()
{
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    m_window = glfwCreateWindow( 1920, 1080, "Spectrum Analyzer", nullptr, nullptr );

    // get error
    if ( !m_window )
    {
        // get glfw error
        const char* description;
        int code = glfwGetError( &description );
        std::cout << "GLFW error: " << code << " " << description << std::endl;

        glfwTerminate();
        throw std::runtime_error( "Failed to create GLFW window" );
    }
    glfwMakeContextCurrent( m_window );
    glfwSetFramebufferSizeCallback( m_window, framebufferSizeCallback );
}

void UI::ShutdownGLFW()
{
    glfwDestroyWindow( m_window );
    glfwTerminate();
}

void UI::InitOpenGL()
{
    gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // for black background
}

void UI::ShutdownOpenGL()
{


}

void UI::framebufferSizeCallback( GLFWwindow* window, int width, int height )
{

    glViewport( 0, 0, width, height );
}
