#include "../include/TodoList/AppGui.hpp"
#include "../include/TodoList/MainFrame.hpp"
#include "wx/string.h"

bool AppGui::OnInit() {
    m_views.mainFrame = new MainFrame(nullptr, wxID_ANY, APP_NAME);
    m_views.mainFrame->setup();
    m_views.mainFrame->Show();
    return true;
}
