#include "../include/TodoList/TaskComp.hpp"
#include "wx/checkbox.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/sizer.h"
#include <algorithm>
#include <functional>
#include <map>
#include <wx/log.h>

static void showSizer(wxBoxSizer& sizer, bool state = true);

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr, const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      m_task(taskPtr) {

    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();
    SetSizerAndFit(m_mainSizer);
}

void TaskComp::setControlsLayout() {
    m_textCtrl->Hide();
    auto sizerFlags = wxSizerFlags(wxSizerFlags());
    m_mainSizer->Add(m_checkBox, sizerFlags);
    m_mainSizer->Add(m_taskText, sizerFlags);
    m_mainSizer->Add(m_textCtrl, sizerFlags.Expand());
}

void TaskComp::allocateControls() {
    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);

    m_checkBox = new wxCheckBox(this, wxID_ANY, "");

    m_taskText = new wxStaticText(this, wxID_ANY, m_task->taskText);

    auto textStyle = wxTE_RICH | wxTE_LEFT | wxTE_WORDWRAP | wxTE_MULTILINE;
    m_textCtrl =
        new wxTextCtrl(this, wxID_ANY, m_task->taskText, wxDefaultPosition, wxSize(this->GetSize()), textStyle);
}

void TaskComp::setBindings() {
    this->Bind(wxEVT_LEFT_DCLICK, &TaskComp::onPanelDoubleLeftClick, this);
    m_textCtrl->Bind(
        wxEVT_KEY_DOWN, [this](wxKeyEvent& ev) { std::invoke(&TaskComp::onKeyPressedTextCtrl, this, std::ref(ev)); },
        m_textCtrl->GetId());
}

void TaskComp::setStyle() {
    SetBackgroundColour(wxTransparentColor);
    SetWindowStyle(GetWindowStyle() | wxBORDER_DOUBLE);
}

void TaskComp::onPanelDoubleLeftClick(wxMouseEvent&) {
    auto& children = m_mainSizer->GetChildren();
    std::for_each(children.begin(), children.end(), [](auto& child) { child->Show(false); });
    m_textCtrl->Show();
    m_textCtrl->SetFocus();
}

static void showSizer(wxBoxSizer* sizer, bool state) {
    auto& children = sizer->GetChildren();
    std::for_each(children.begin(), children.end(), [state](auto& child) { child->Show(state); });
}

void TaskComp::onReturnPressed() {
    showSizer(m_mainSizer, true);
    m_textCtrl->Hide();
    m_taskText->SetLabel(m_textCtrl->GetValue());
}

void TaskComp::onKeyPressedTextCtrl(wxKeyEvent& keyEv) {
    const std::map<int, decltype(&TaskComp::onReturnPressed)> keyToHandler = {
        {WXK_RETURN, &TaskComp::onReturnPressed},
        // TODO: handle ESC
    };

    if (auto handlerIt = keyToHandler.find(keyEv.GetKeyCode()); handlerIt != keyToHandler.end()) {
        auto [_, handler] = *handlerIt;
        std::invoke(handler, this);
    } else {
        keyEv.Skip();
    }
}
