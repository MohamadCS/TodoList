#include "../include/TodoList/AppGui.hpp"
#include "../include/TodoList/Database.hpp"
#include "../include/TodoList/Defines.hpp"
#include "../include/TodoList/MainFrame.hpp"
#include "../include/TodoList/Utils.hpp"

#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/string.h"
#include <variant>
#include <wx/sysopt.h>

wxIMPLEMENT_APP(TodoList::Gui::AppGui);

namespace TodoList::Gui {

void AppGui::onChangeView(wxCommandEvent& evt) {
    auto* view = static_cast<Utility::Views*>(evt.GetClientData());
    LOG("Changin View");
    std::visit([](auto& view) { view->Hide(); }, m_currentFrame);

    m_currentFrame = m_views.at(*view);

    if (auto pFrame = std::get_if<MainFrame*>(&m_currentFrame)) {
        (*pFrame)->setup();
    }

    std::visit([](auto& view) { view->Show(); }, m_currentFrame);

    delete view;
    evt.Skip();
}

bool AppGui::OnInit() {
    Bind(EVT_CHANGE_VIEW, &TodoList::Gui::AppGui::onChangeView, this);

    m_views.insert_or_assign(Utility::Views::MAIN_FRAME,
                             new MainFrame(nullptr, wxID_ANY, APP_NAME, wxDefaultPosition, wxDefaultSize));

    m_views.insert_or_assign(Utility::Views::LOGIN_FRAME,

                             new LoginFrame(nullptr, wxID_ANY, APP_NAME, wxDefaultPosition, wxDefaultSize));

    m_views.insert_or_assign(Utility::Views::SIGNUP_FRAME,
                             new SignupFrame(nullptr, wxID_ANY, APP_NAME, wxDefaultPosition, wxDefaultSize));

    m_currentFrame = m_views[Utility::Views::LOGIN_FRAME];
    std::visit([](auto& view) { view->Show(); }, m_currentFrame);
    return true;
}

} // namespace TodoList::Gui
