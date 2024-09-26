#pragma once

#include "AppCore.hpp"
#include "Events.hpp"

#include "wx/event.h"
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

namespace TodoList::Gui {
class LoginFrame : public wxFrame {
public:
    template <class... Args>
    LoginFrame(Args... args);

    ~LoginFrame() override = default;

private:
    wxTextCtrl* m_emailEntry;
    wxTextCtrl* m_passwordEntry;
    wxStaticText* m_emailText;
    wxStaticText* m_passwordText;

    wxButton* m_loginButton;
    wxButton* m_signupButton;
    void onLoginButtonPressed(wxCommandEvent&);
    void onSignupButtonPressed(wxCommandEvent&);

    void setup();
    const wxSize DEFAULT_FRAME_DIMS = wxSize(800, 600);
    const int SIDEBAR_WIDTH = 200;
};

template <class... Args>
LoginFrame::LoginFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...) {
    SetSizer(new wxGridSizer(2,3,40)); // TODO: Change to wxFlexGridSizer

    setup();

    Fit();
    SetClientSize(DEFAULT_FRAME_DIMS);
    SetMinClientSize(DEFAULT_FRAME_DIMS);
}
} // namespace TodoList::Gui
