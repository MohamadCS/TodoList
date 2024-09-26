
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
class SignupFrame : public wxFrame {
public:
    template <class... Args>
    SignupFrame(Args... args);

    ~SignupFrame() override = default;

private:
    wxTextCtrl* m_firstNameEntry;
    wxTextCtrl* m_lastNameEntry;
    wxTextCtrl* m_emailEntry;
    wxTextCtrl* m_passwordEntry;

    wxStaticText* m_firstNameText;
    wxStaticText* m_lastNameText;
    wxStaticText* m_emailText;
    wxStaticText* m_passwordText;

    wxButton* m_signupButton;
    wxButton* m_backToLoginButton;

    void onSignupButtonPressed(wxCommandEvent&);
    void onLoginButtonPressed(wxCommandEvent&);

    void setup();
    const wxSize DEFAULT_FRAME_DIMS = wxSize(800, 600);
    const int SIDEBAR_WIDTH = 200;
};

template <class... Args>
SignupFrame::SignupFrame(Args... args)
    : wxFrame(std::forward<Args>(args)...) {
    SetSizer(new wxGridSizer(2, 5, 40)); // TODO: Change to wxFlexGridSizer

    setup();

    Fit();
    SetClientSize(DEFAULT_FRAME_DIMS);
    SetMinClientSize(DEFAULT_FRAME_DIMS);
}
} // namespace TodoList::Gui
