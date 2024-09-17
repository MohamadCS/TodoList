#include "../include/TodoList/TaskComp.hpp"

#include "wx/colour.h"
#include "wx/dc.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"
#include <cstdint>
#include <sys/types.h>
#include <unistd.h>
#include <wx/button.h>
#include <wx/log.h>

#include <algorithm>
#include <map>

static void showSizer(wxBoxSizer* sizer, bool state = true);
static void onReturnPressed(TaskComp* taskComp);
static void onEscapePressed(TaskComp* taskComp);
static void onEscapePressed(TaskComp* taskComp);

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr, std::pair<uint32_t, TaskProjectComp*> taskProject,
                   const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      task(taskPtr) {

    taskProjects.insert_or_assign(taskProject.first, taskProject.second);
    SetName("Task");
    SetMinSize(DEFAULT_SIZE);
    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();
    SetSizerAndFit(mainSizer);
    Layout();
    Refresh();
}

void TaskComp::allocateControls() {
    mainSizer = new wxBoxSizer(wxHORIZONTAL);

    checkBox = new wxCheckBox(this, wxID_ANY, "");

    taskText = new wxStaticText(this, wxID_ANY, task->taskText);

    auto textStyle = wxTE_RICH | wxTE_LEFT | wxTE_NO_VSCROLL| wxTE_MULTILINE | wxTE_WORDWRAP;

    textCtrl = new wxTextCtrl(this, wxID_ANY, task->taskText, wxDefaultPosition, GetSize(), textStyle);

    duoDateText = new wxStaticText(this, wxID_ANY, "No Date");
}

void TaskComp::setControlsLayout() {
    textCtrl->Hide();
    auto sizerFlags = wxSizerFlags();
    mainSizer->Add(checkBox, sizerFlags.Border(wxALL, 10));
    mainSizer->Add(taskText, sizerFlags.Border(wxALL, 10));
    mainSizer->Add(textCtrl, sizerFlags.Expand().FixedMinSize());
    mainSizer->AddStretchSpacer(5);
    mainSizer->Add(duoDateText, sizerFlags.Border(wxALL, 10));
    mainSizer->AddStretchSpacer(1);
}

void TaskComp::setBindings() {
    this->Bind(wxEVT_LEFT_DCLICK, &TaskComp::onPanelDoubleLeftClick, this);
    duoDateText->Bind(wxEVT_LEFT_DCLICK, &TaskComp::onDuoDateDoubleLeftClick, this);
    Bind(wxEVT_PAINT, &TaskComp::onPaint, this);
    checkBox->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &TaskComp::onCheckBoxClick, this);
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
    }

    keyEv.Skip();
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

void TaskComp::onCheckBoxClick(wxCommandEvent& ev) {
    // Propogate "CheckBoxEvent"
    wxLogDebug("Checkbox Clicked");
    task->checked = true;
    wxCommandEvent projectChangeEvent{EVT_TASK_FINISHED};
    projectChangeEvent.SetClientData(this);
    wxPostEvent(this, std::move(projectChangeEvent));
    this->GetParent()->Layout();
    this->GetParent()->Refresh();
    ev.Skip();
}

void TaskComp::onDuoDateDoubleLeftClick(wxMouseEvent& ev) {
    wxLogDebug("Duo date double left clicked");
    wxCommandEvent projectChangeEvent{EVT_REQUEST_CAL_DIALOG};
    projectChangeEvent.SetClientData(new std::pair<TaskComp*, ChangingDate>{this, ChangingDate::DUO_DATE});
    wxPostEvent(this, std::move(projectChangeEvent));
    this->GetParent()->Layout();
    this->GetParent()->Refresh();
}
