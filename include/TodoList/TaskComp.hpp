#pragma once
 

#include "AppCore.hpp"

#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class TaskComp : public wxPanel {
public:
    TaskComp(wxWindow* parent, wxWindowID id, Task* taskPtr,
             const wxPoint& postion = wxDefaultPosition,
             const wxSize& size = DEFAULT_SIZE);

private:
    Task* m_task;
    wxStaticText* m_taskText;
    wxStaticText* m_duoDateText;
    wxStaticText* m_deadLineText;
    wxTextCtrl* m_textCtrl;
    wxCheckBox* m_checkBox;
    wxBoxSizer* m_mainSizer;

private:
    inline static const wxSize DEFAULT_SIZE = wxSize(200, 200);
    void setStyle();
    void setBindings();
    void allocateControls();
    void setControlsLayout();
    void onKeyPressedTextCtrl(wxKeyEvent&);
    void onPanelDoubleLeftClick(wxMouseEvent&);
    void onReturnPressed();
};
