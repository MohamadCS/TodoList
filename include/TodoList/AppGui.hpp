#pragma once
#include "Defines.hpp"
#include "LoginFrame.hpp"
#include "MainFrame.hpp"
#include "SignupFrame.hpp"
#include "wx/event.h"

#include <variant>
#include <wx/app.h>
#include <wx/string.h>

namespace TodoList::Gui {

using View = std::variant<MainFrame*, LoginFrame*, SignupFrame*>;

class AppGui : public wxApp {
public:
    AppGui() = default;
    ~AppGui() override = default;

    bool OnInit() override;

private:
    std::map<Utility::Views, View> m_views;
    View m_currentFrame;

    void onChangeView(wxCommandEvent& evt);

    const wxString APP_NAME = "TodoList";
};

}; // namespace TodoList::Gui
