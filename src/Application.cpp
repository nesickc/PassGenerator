#include "Application.hpp"

Application::Application() : m_ui()
{
}

void Application::run()
{
    while (!m_ui.IsClosing())
    {
        m_ui.Render();
    }
}
