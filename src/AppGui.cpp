#include "../include/TodoList/AppGui.hpp"
#include "../include/TodoList/Database.hpp"
#include "../include/TodoList/MainFrame.hpp"

#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/string.h"
#include <wx/sysopt.h>

wxIMPLEMENT_APP(TodoList::Gui::AppGui);

namespace TodoList::Gui {

bool AppGui::OnInit() {
    m_views.mainFrame = new MainFrame(nullptr, wxID_ANY, APP_NAME, wxDefaultPosition, wxDefaultSize);
    m_views.mainFrame->Show();
    return true;
    Core::Database db("BestDb", "~/Desktop/");
    db.newTable("Table", {{"id", "INTEGER PRIMARY KEY AUTOINCREMENT"}});
}

} // namespace TodoList::Gui
