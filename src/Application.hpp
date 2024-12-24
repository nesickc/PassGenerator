#pragma once
#include "UI.hpp"

class Application
{
public:
    Application();
    ~Application() = default;

    void run();

private:
    UI m_ui;
};