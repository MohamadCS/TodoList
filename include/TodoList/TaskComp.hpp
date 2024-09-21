#pragma once

#include "AppCore.hpp"
#include "Events.hpp"
#include <cstdint>
#include <map>

#include "wx/event.h"
#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace TodoList::Gui {

struct TaskProjectComp;

struct TaskComp : public wxPanel {

    Core::Task* task;
    wxStaticText* taskText;
    wxStaticText* duoDateText;
    wxStaticText* deadLineText;
    wxTextCtrl* textCtrl;
    wxCheckBox* checkBox;
    // wxBoxSizer* mainSizer;
    std::map<uint32_t, TaskProjectComp*> taskProjects;
    inline static const wxSize DEFAULT_SIZE = wxSize(200, 40);

    enum class ChangingDate { DUO_DATE, DEADLINE_DATE };

    void setStyle();
    void setBindings();
    void allocateControls();
    void setControlsLayout();
    void cancelTextInsertion();

    TaskComp(wxWindow* parent, wxWindowID id, Core::Task* taskPtr, std::pair<uint32_t, TaskProjectComp*> taskProject,
             const wxPoint& postion = wxDefaultPosition, const wxSize& size = DEFAULT_SIZE);

    // Event Handling
    void onPaint(wxPaintEvent& event);
    void onKeyPressedTextCtrl(wxKeyEvent&);
    void onPanelDoubleLeftClick(wxMouseEvent&);
    void onCheckBoxClick(wxCommandEvent&);
    void onDuoDateDoubleLeftClick(wxMouseEvent&);

};
} // namespace TodoList::Gui
