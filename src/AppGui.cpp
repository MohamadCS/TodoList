#include "../include/TodoList/AppGui.hpp"
#include "../include/TodoList/MainFrame.hpp"
#include "wx/gdicmn.h"
#include "wx/string.h"
#include <wx/sysopt.h>

namespace TodoList::Gui {
bool AppGui::OnInit() {
    m_views.mainFrame = new MainFrame(nullptr, wxID_ANY, APP_NAME, wxDefaultPosition, wxDefaultSize);
    m_views.mainFrame->Show();
    return true;
}

} // namespace TodoList::Gui
