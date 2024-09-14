#pragma once

#include "wx/dc.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include <wx/dcclient.h>
#include <wx/window.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

class TaskComp : public wxPanel {
public:
    TaskComp(wxWindow* parent, wxWindowID id,
             const wxPoint& postion = wxDefaultPosition,
             const wxSize& size = DEFAULT_SIZE);

    // void render(wxDC&);
    //
    // void paintNow();
    //
    // void onPaint(wxPaintEvent&);

private:
    wxStaticText* m_taskText;
    wxStaticText* m_duoDateText;
    wxStaticText* m_deadLineText;
    wxTextCtrl* wx_textEnterTask;
    wxCheckBox* m_checkBox;
    wxBoxSizer* m_mainSizer;

private:
    inline static const wxSize DEFAULT_SIZE = wxSize(200,100);
    void setStyle();
};
