#include "../include/TodoList/TaskComp.hpp"

#include "wx/checkbox.h"
#include "wx/sizer.h"
#include <wx/log.h>

#include <algorithm>
#include <map>

static void showSizer(wxBoxSizer* sizer, bool state = true);
static void onReturnPressed(TaskComp* taskComp);
static void onEscapePressed(TaskComp* taskComp);

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr, const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      m_task(taskPtr) {
    SetName("Task");
    SetMinSize(DEFAULT_SIZE);
    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();
    SetSizerAndFit(m_mainSizer);
}

void TaskComp::allocateControls() {
    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);

    m_checkBox = new wxCheckBox(this, wxID_ANY, "");

    m_taskText = new wxStaticText(this, wxID_ANY, m_task->taskText);

    auto textStyle = wxTE_RICH | wxTE_LEFT | wxTE_WORDWRAP | wxTE_MULTILINE;
    m_textCtrl = new wxTextCtrl(this, wxID_ANY, m_task->taskText, wxDefaultPosition, GetSize(), textStyle);
}

void TaskComp::setControlsLayout() {
    m_textCtrl->Hide();
    auto sizerFlags = wxSizerFlags(wxSizerFlags());
    m_mainSizer->Add(m_checkBox, sizerFlags.Border(wxALL, 10));
    m_mainSizer->Add(m_taskText, sizerFlags.Center());
    m_mainSizer->Add(m_textCtrl, sizerFlags.Center());
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

void TaskComp::onKeyPressedTextCtrl(wxKeyEvent& keyEv) {
    const std::map<int, std::function<void(TaskComp*)>> keyToHandler = {
        {WXK_RETURN, &onReturnPressed},
        {WXK_ESCAPE, &TaskComp::cancelTextInsertion},
    };

    if (auto handlerIt = keyToHandler.find(keyEv.GetKeyCode()); handlerIt != keyToHandler.end()) {
        auto [_, handler] = *handlerIt;
        std::invoke(handler, this);
    } else {
        keyEv.Skip();
    }
}

static void showSizer(wxBoxSizer* sizer, bool state) {

    if (sizer == nullptr) {
        wxLogDebug("Got Null");
        exit(0);
    }

    auto& children = sizer->GetChildren();
    std::for_each(children.begin(), children.end(), [state](auto& child) { child->Show(state); });
}

void TaskComp::cancelTextInsertion() {
    showSizer(m_mainSizer, true);
    m_textCtrl->Hide();
    m_taskText->SetLabel(m_textCtrl->GetValue());
}

static void onReturnPressed(TaskComp* taskComp) {

    if (taskComp == nullptr) {
        wxLogDebug("Got Null");
        exit(0);
    }

    showSizer(taskComp->m_mainSizer, true);
    taskComp->m_textCtrl->Hide();
    taskComp->m_taskText->SetLabel(taskComp->m_textCtrl->GetValue());
}

void TaskComp::show() {
    auto& children = m_mainSizer->GetChildren();
    std::for_each(children.begin(), children.end(), [](auto& child) { child->Show(true); });
    // Show(true);
}

void TaskComp::hide() {
    auto& children = m_mainSizer->GetChildren();
    std::for_each(children.begin(), children.end(), [](auto& child) { child->Show(false); });
    Show(false);
}

void TaskCompList::show() {
    std::for_each(m_taskCompVec.begin(), m_taskCompVec.end(), [](TaskComp* taskCompPtr) { taskCompPtr->show(); });
}

void TaskCompList::hide() {
    std::for_each(m_taskCompVec.begin(), m_taskCompVec.end(), [](TaskComp* taskCompPtr) { taskCompPtr->hide(); });
}
