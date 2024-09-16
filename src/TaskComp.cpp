#include "../include/TodoList/TaskComp.hpp"

#include "wx/checkbox.h"
#include "wx/colour.h"
#include "wx/dc.h"
#include "wx/gdicmn.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"
#include <cstdint>
#include <sys/types.h>
#include <wx/log.h>

#include <algorithm>
#include <map>



static void showSizer(wxBoxSizer* sizer, bool state = true);
static void onReturnPressed(TaskComp* taskComp);
static void onEscapePressed(TaskComp* taskComp);

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr,std::pair<uint32_t,TaskProjectComp*> taskProject ,const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      task(taskPtr)
       {

    taskProjects.insert_or_assign(taskProject.first,taskProject.second);
    SetName("Task");
    SetMinSize(DEFAULT_SIZE);
    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();
    SetSizerAndFit(mainSizer);
}

void TaskComp::allocateControls() {
    mainSizer = new wxBoxSizer(wxHORIZONTAL);

    checkBox = new wxCheckBox(this, wxID_ANY, "");

    taskText = new wxStaticText(this, wxID_ANY, task->taskText);

    auto textStyle = wxTE_RICH | wxTE_LEFT | wxTE_WORDWRAP | wxTE_MULTILINE;

    textCtrl = new wxTextCtrl(this, wxID_ANY, task->taskText, wxDefaultPosition, wxDefaultSize, textStyle);
}

void TaskComp::setControlsLayout() {
    textCtrl->Hide();
    auto sizerFlags = wxSizerFlags();
    mainSizer->Add(checkBox, sizerFlags.Border(wxALL, 10));
    mainSizer->Add(taskText, sizerFlags.Border(wxALL, 10));
    mainSizer->Add(textCtrl, sizerFlags.Border(wxALL, 10));
}

void TaskComp::setBindings() {
    this->Bind(wxEVT_LEFT_DCLICK, &TaskComp::onPanelDoubleLeftClick, this);
    Bind(wxEVT_PAINT, &TaskComp::onPaint, this);
    textCtrl->Bind(
        wxEVT_KEY_DOWN, [this](wxKeyEvent& ev) { std::invoke(&TaskComp::onKeyPressedTextCtrl, this, std::ref(ev)); },
        textCtrl->GetId());
}

void TaskComp::setStyle() {
    SetBackgroundColour(wxTransparentColor);
    textCtrl->SetBackgroundColour(GetBackgroundColour());
    SetWindowStyle(GetWindowStyle() | wxNO_BORDER);
}

void TaskComp::onPanelDoubleLeftClick(wxMouseEvent& ev) {
    auto& children = mainSizer->GetChildren();
    std::for_each(children.begin(), children.end(), [](auto& child) { child->Show(false); });
    textCtrl->Show();
    textCtrl->SetFocus();
    ev.Skip();
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
    showSizer(mainSizer, true);
    textCtrl->Hide();
    taskText->SetLabel(textCtrl->GetValue());
}

static void onReturnPressed(TaskComp* taskComp) {

    if (taskComp == nullptr) {
        wxLogDebug("Got Null");
        exit(0);
    }

    showSizer(taskComp->mainSizer, true);
    taskComp->textCtrl->Hide();
    taskComp->taskText->SetLabel(taskComp->textCtrl->GetValue());
}

void TaskComp::onPaint(wxPaintEvent& ev) {
    wxPaintDC dc(this);

    // Draw the background
    auto color = wxColor(250, 250, 250);
    dc.SetBrush(color);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRoundedRectangle(wxRect(wxDefaultPosition, GetSize()), 10);
}

