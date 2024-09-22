#include "../include/TodoList/TaskComp.hpp"
#include "../include/TodoList/Utils.hpp"

#include <algorithm>
#include <map>
#include <sys/types.h>
#include <unistd.h>

#include <utility>
#include <wx/button.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

static void showSizer(wxSizer* sizer, bool state = true);
static void onReturnPressed(TodoList::Gui::TaskComp* taskComp);
static void onEscapePressed(TodoList::Gui::TaskComp* taskComp);

namespace TodoList::Gui {

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, Core::Task* taskPtr,
                   std::pair<Core::ID, TaskProjectComp*> taskProject, const wxPoint& postion, const wxSize& size)
    : wxPanel(parent, id, postion, size),
      task(taskPtr) {

    // taskProjects.insert_or_assign(taskProject.first, taskProject.second);
    SetName("Task");
    SetMinSize(DEFAULT_SIZE);
    SetSizer(new wxBoxSizer(wxHORIZONTAL));

    allocateControls();
    setControlsLayout();
    setBindings();
    setStyle();

    setStateDefault();
    Fit();

    Utility::refresh(this);
}

void TaskComp::allocateControls() {

    checkBox = new wxCheckBox(this, wxID_ANY, "");

    taskText = new wxStaticText(this, wxID_ANY, task->taskText);

    auto textStyle = wxBORDER_NONE | wxTE_WORDWRAP;

    textCtrl = new wxTextCtrl(this, wxID_ANY, task->taskText, wxDefaultPosition, wxDefaultSize, textStyle);

    std::string dateText = "No Date";

    if (auto dateTp = task->duoDate; task->duoDate.has_value()) {
        dateText = Utility::timePointToStr(dateTp.value());
    }
    duoDateText = new wxStaticText(this, wxID_ANY, dateText);
}

void TaskComp::setControlsLayout() {
    GetSizer()->Add(checkBox, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxALIGN_LEFT, 10);
    GetSizer()->Add(taskText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxALIGN_LEFT, 10);
    GetSizer()->AddStretchSpacer();
    GetSizer()->Add(duoDateText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    GetSizer()->Add(textCtrl, 1, wxEXPAND | wxALL, 10); // TODO: Consider wxExpand
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
    setStateChangingText();
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
    setStateDefault();
    setText(textCtrl->GetValue());
}

void TaskComp::onPaint(wxPaintEvent& ev) {
    wxPaintDC dc(this);

    // Draw the background
    const auto color = wxColor(250, 250, 250);
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
    projectChangeEvent.SetClientData(new std::pair<TaskComp*, Date>{this, Date::DUO_DATE});
    wxPostEvent(this, std::move(projectChangeEvent));
    Utility::refresh(this->GetParent());
}

void TaskComp::setDate(const Core::TimePoint& timePoint, Date dateType) {
    const std::string dateText = Utility::timePointToStr(timePoint);

    switch (dateType) {
    case Date::DUO_DATE:
        duoDateText->SetLabel(dateText);
        task->duoDate = timePoint;
        break;
    case Date::DEADLINE_DATE:
        duoDateText->SetLabel(dateText);
        task->deadLine = timePoint;
        break;
    default:
        std::unreachable();
        break;
    }

    Utility::refresh(std::make_tuple(duoDateText, this));
}

void TaskComp::setStateDefault() {
    auto& children = GetSizer()->GetChildren();
    std::for_each(children.begin(), children.end(), [](auto& child) { child->Show(true); });
    textCtrl->Hide();
}

void TaskComp::setStateChangingText() {
    auto& children = GetSizer()->GetChildren();
    std::for_each(children.begin(), children.end(), [](auto& child) { child->Show(false); });
    textCtrl->Show();
    textCtrl->SetFocus();
}

void TaskComp::setText(const wxString& text){
    taskText->SetLabel(text);
    task->taskText = text;
}

} // namespace TodoList::Gui

static void showSizer(wxSizer* sizer, bool state) {

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

    taskComp->setStateDefault();
    taskComp->setText(taskComp->textCtrl->GetValue());
    TodoList::Utility::refresh(taskComp);
}


