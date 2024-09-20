#include "../include/TodoList/TaskComp.hpp"
#include "../include/TodoList/Utils.hpp"

#include <algorithm>
#include <cstdint>
#include <map>
#include <sys/types.h>
#include <unistd.h>

#include <wx/button.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

static void showSizer(wxBoxSizer* sizer, bool state = true);
static void onReturnPressed(TodoList::Gui::TaskComp* taskComp);
static void onEscapePressed(TodoList::Gui::TaskComp* taskComp);

namespace TodoList::Gui {

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, Core::Task* taskPtr,
                   std::pair<uint32_t, TaskProjectComp*> taskProject, const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      task(taskPtr) {

    taskProjects.insert_or_assign(taskProject.first, taskProject.second);
    SetName("Task");
    SetMinSize(DEFAULT_SIZE);
    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();
    SetSizer(mainSizer);

    Utility::refresh(this);
}

void TaskComp::allocateControls() {
    mainSizer = new wxBoxSizer(wxHORIZONTAL);

    checkBox = new wxCheckBox(this, wxID_ANY, "");

    taskText = new wxStaticText(this, wxID_ANY, task->taskText);

    auto textStyle = wxBORDER_NONE | wxTE_WORDWRAP;

    textCtrl = new wxTextCtrl(this, wxID_ANY, task->taskText, wxDefaultPosition, wxDefaultSize, textStyle);

    duoDateText = new wxStaticText(this, wxID_ANY, "No Date");
}

void TaskComp::setControlsLayout() {
    textCtrl->Hide();
    mainSizer->Add(checkBox, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxALIGN_LEFT, 10);
    mainSizer->Add(taskText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxALIGN_LEFT, 10);
    mainSizer->AddStretchSpacer();
    mainSizer->Add(duoDateText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    mainSizer->Add(textCtrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 10);
}

void TaskComp::setBindings() {
    Bind(wxEVT_LEFT_DCLICK, &TaskComp::onPanelDoubleLeftClick, this);

    duoDateText->Bind(wxEVT_LEFT_DCLICK, &TaskComp::onDuoDateDoubleLeftClick, this);

    Bind(wxEVT_PAINT, &TaskComp::onPaint, this);

    checkBox->Bind(wxEVT_CHECKBOX, &TaskComp::onCheckBoxClick, this);

    textCtrl->Bind(
        wxEVT_KEY_DOWN, [this](wxKeyEvent& ev) { std::invoke(&TaskComp::onKeyPressedTextCtrl, this, std::ref(ev)); },
        textCtrl->GetId());
}

void TaskComp::setStyle() {
    wxFont font = textCtrl->GetFont();
    font.SetPointSize(13);
    textCtrl->SetFont(font);
    SetBackgroundColour(wxTransparentColor);
    textCtrl->SetBackgroundColour(wxColor(250, 250, 250));
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

void TaskComp::cancelTextInsertion() {
    showSizer(mainSizer, true);
    textCtrl->Hide();
    taskText->SetLabel(textCtrl->GetValue());
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
    Utility::refresh(this->GetParent());
}

void TaskComp::onDuoDateDoubleLeftClick(wxMouseEvent& ev) {
    wxLogDebug("Duo date double left clicked");
    wxCommandEvent projectChangeEvent{EVT_REQUEST_CAL_DIALOG};
    projectChangeEvent.SetClientData(new std::pair<TaskComp*, ChangingDate>{this, ChangingDate::DUO_DATE});
    wxPostEvent(this, std::move(projectChangeEvent));
    Utility::refresh(this->GetParent());
}

} // namespace TodoList::Gui

static void showSizer(wxBoxSizer* sizer, bool state) {

    if (sizer == nullptr) {
        wxLogDebug("Got Null");
        exit(0);
    }

    auto& children = sizer->GetChildren();
    std::for_each(children.begin(), children.end(), [state](auto& child) { child->Show(state); });
}

static void onReturnPressed(TodoList::Gui::TaskComp* taskComp) {

    if (taskComp == nullptr) {
        wxLogDebug("Got Null");
        exit(0);
    }

    showSizer(taskComp->mainSizer, true);
    taskComp->textCtrl->Hide();
    taskComp->taskText->SetLabel(taskComp->textCtrl->GetValue());
    TodoList::Utility::refresh(taskComp);
}
