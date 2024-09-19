#pragma once

#include <wx/app.h>
#include <wx/string.h>

#include "MainFrame.hpp"

struct Views {
    MainFrame* mainFrame;
};

class AppGui : public wxApp {
public:
    AppGui() = default;
    ~AppGui() override = default;

    bool OnInit() override;

private:
    Views m_views;
    wxFrame* m_currentFrame;

    const wxString APP_NAME = "TodoList";
};

wxIMPLEMENT_APP(AppGui);
