#pragma once

#include "AppCore.hpp"
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

    enum class Date;
    Core::Task* task;

    wxStaticText* taskText;
    wxStaticText* duoDateText;
    wxStaticText* deadLineText;

    wxTextCtrl* textCtrl;
    wxCheckBox* checkBox;
    inline static const wxSize DEFAULT_SIZE = wxSize(200, 40);

    TaskComp(wxWindow* parent, wxWindowID id, Core::Task* taskPtr, std::pair<Core::ID, TaskProjectComp*> taskProject,
             const wxPoint& postion = wxDefaultPosition, const wxSize& size = DEFAULT_SIZE);

    void setText(const wxString&);
    void setDate(const Core::TimePoint&, Date);
    void setStateChangingText();
    void setStateDefault();

    void cancelTextInsertion();

    enum class Date {
        DUO_DATE,
        DEADLINE_DATE
    };
    // Event Handling
private:
    void onPaint(wxPaintEvent& event);
    void onKeyPressedTextCtrl(wxKeyEvent&);
    void onPanelDoubleLeftClick(wxMouseEvent&);
    void onCheckBoxClick(wxCommandEvent&);
    void onDuoDateDoubleLeftClick(wxMouseEvent&);
    void setBindings();
};
} // namespace TodoList::Gui
