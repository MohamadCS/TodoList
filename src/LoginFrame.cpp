#include "../include/TodoList/LoginFrame.hpp"
#include "../include/TodoList/Defines.hpp"
#include "../include/TodoList/Utils.hpp"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/msgdlg.h"

namespace TodoList::Gui {

void LoginFrame::onLoginButtonPressed(wxCommandEvent& ev) {
    LOG("Button pressed");
    auto& appCore = Core::App::instance();

    bool rv = appCore.login(m_emailEntry->GetValue().ToStdString(), m_passwordEntry->GetValue().ToStdString());

    if (!rv) {
        wxMessageBox("Wrong Email or Password");
        return;
    }

    wxCommandEvent changeViewEvt{EVT_CHANGE_VIEW};
    auto* data = new Utility::Views(Utility::Views::MAIN_FRAME);
    changeViewEvt.SetClientData(data);
    wxPostEvent(this, std::move(changeViewEvt));
}

void LoginFrame::onSignupButtonPressed(wxCommandEvent& ev) {
    LOG("Button pressed");
    wxCommandEvent changeViewEvt{EVT_CHANGE_VIEW};
    auto* data = new Utility::Views(Utility::Views::SIGNUP_FRAME);
    changeViewEvt.SetClientData(data);
    wxPostEvent(this, std::move(changeViewEvt));
}

void LoginFrame::setup() {
    m_emailEntry = new wxTextCtrl(this, wxID_ANY, "");
    m_passwordEntry = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    m_loginButton = new wxButton(this, wxID_ANY, "Login");
    m_signupButton = new wxButton(this, wxID_ANY, "Signup");
    m_emailText = new wxStaticText(this, wxID_ANY, "Email");
    m_passwordText = new wxStaticText(this, wxID_ANY, "Password");

    auto sizerFlags = wxSizerFlags().Proportion(1);

    GetSizer()->Add(m_emailText, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_emailEntry, sizerFlags.Left());
    GetSizer()->Add(m_passwordText, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_passwordEntry, sizerFlags.Left());

    GetSizer()->Add(m_loginButton, sizerFlags.Right().CenterVertical());
    GetSizer()->Add(m_signupButton, sizerFlags.Left().CenterVertical());

    m_loginButton->Bind(wxEVT_BUTTON, &TodoList::Gui::LoginFrame::onLoginButtonPressed, this);
    m_signupButton->Bind(wxEVT_BUTTON, &TodoList::Gui::LoginFrame::onSignupButtonPressed, this);
}

} // namespace TodoList::Gui
