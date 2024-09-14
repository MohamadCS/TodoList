#include "../include/TodoList/TaskComp.hpp"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/memory.h"
#include "wx/osx/checkbox.h"
#include "wx/osx/stattext.h"
#include "wx/sizer.h"
#include <wx/dcclient.h>

TaskComp::TaskComp(wxWindow* parent, wxWindowID id, const wxPoint& postion,
                   const wxSize& size)
    : wxPanel(parent, id, postion, size) {

    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
    m_checkBox = new wxCheckBox(this, wxID_ANY, "");
    m_taskText = new wxStaticText(this, wxID_ANY, "This is a new task");

    m_mainSizer->Add(m_checkBox, wxSizerFlags().Border(wxALL, 10));
    m_mainSizer->Add(m_taskText, wxSizerFlags().Border(wxALL, 10));


    SetSizerAndFit(m_mainSizer);
}


void TaskComp::setStyle(){
    SetBackgroundColour(*wxLIGHT_GREY);
    SetWindowStyle(GetWindowStyle() | wxBORDER_THEME);
}
