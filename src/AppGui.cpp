#include "../include/TodoList/AppGui.hpp"
#include "../include/TodoList/MainFrame.hpp"
#include "wx/string.h"

bool AppGui::OnInit() {
    m_views.mainFrame = new MainFrame(nullptr, wxID_ANY, APP_NAME);
    m_currentFrame = m_views.mainFrame;
    return true;
}
