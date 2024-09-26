
#include "../include/TodoList/SignupFrame.hpp"
#include "../include/TodoList/Account.hpp"
#include "../include/TodoList/Defines.hpp"
#include "../include/TodoList/Utils.hpp"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/msgdlg.h"

namespace TodoList::Gui {

void SignupFrame::onSignupButtonPressed(wxCommandEvent& ev) {
    LOG("Button pressed");
    auto& appCore = Core::App::instance();

    Core::Account account =
        Core::Account(m_firstNameEntry->GetValue().ToStdString(), m_lastNameEntry->GetValue().ToStdString(),
                      m_emailEntry->GetValue().ToStdString(), m_passwordEntry->GetValue().ToStdString());

    if (!appCore.signup(account)) {
        wxMessageBox("Wrong Email or Password");
        return;
    }

    wxCommandEvent changeViewEvt{EVT_CHANGE_VIEW};
    auto* data = new Utility::Views(Utility::Views::LOGIN_FRAME);
    changeViewEvt.SetClientData(data);
    wxPostEvent(this, std::move(changeViewEvt));
}

void SignupFrame::onLoginButtonPressed(wxCommandEvent& ev) {
    LOG("Button pressed");
    auto& appCore = Core::App::instance();
    wxCommandEvent changeViewEvt{EVT_CHANGE_VIEW};
    auto* data = new Utility::Views(Utility::Views::LOGIN_FRAME);
    changeViewEvt.SetClientData(data);
    wxPostEvent(this, std::move(changeViewEvt));
}

void SignupFrame::setup() {
    m_firstNameEntry = new wxTextCtrl(this, wxID_ANY);
    m_lastNameEntry = new wxTextCtrl(this, wxID_ANY);
    m_emailEntry = new wxTextCtrl(this, wxID_ANY);
    m_passwordEntry = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

    m_firstNameText = new wxStaticText(this, wxID_ANY, "First Name");
    m_lastNameText = new wxStaticText(this, wxID_ANY, "Last Name");
    m_emailText = new wxStaticText(this, wxID_ANY, "Email");
    m_passwordText = new wxStaticText(this, wxID_ANY, "Password");

    m_signupButton = new wxButton(this, wxID_ANY, "Signup");
    m_backToLoginButton = new wxButton(this, wxID_ANY, "Login");

    auto sizerFlags = wxSizerFlags().Proportion(1).CenterVertical();
    GetSizer()->Add(m_firstNameText, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_firstNameEntry, sizerFlags.Left());

    GetSizer()->Add(m_lastNameText, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_lastNameEntry, sizerFlags.Left());

    GetSizer()->Add(m_emailText, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_emailEntry, sizerFlags.Left());

    GetSizer()->Add(m_passwordText, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_passwordEntry, sizerFlags.Left());

    GetSizer()->Add(m_signupButton, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_backToLoginButton, sizerFlags.Left().CenterVertical());

    m_signupButton->Bind(wxEVT_BUTTON, &TodoList::Gui::SignupFrame::onSignupButtonPressed, this);
    m_backToLoginButton->Bind(wxEVT_BUTTON, &TodoList::Gui::SignupFrame::onLoginButtonPressed, this);
}

} // namespace TodoList::Gui
