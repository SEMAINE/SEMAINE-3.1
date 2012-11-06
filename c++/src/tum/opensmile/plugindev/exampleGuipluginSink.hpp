/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


/*  openSMILE component:

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CEXAMPLEGUIPLUGINSINK_HPP
#define __CEXAMPLEGUIPLUGINSINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>
#include <wx/wx.h>

#define COMPONENT_DESCRIPTION_CEXAMPLEGUIPLUGINSINK "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CEXAMPLEGUIPLUGINSINK "cExampleGuipluginSink"

#undef class


#include <wx/sizer.h>
 
class wxImagePanel : public wxPanel
    {
        wxBitmap image;
        
        
    public:
        bool showImg;

        wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format);
        
        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        
        void render(wxDC& dc);
        
        DECLARE_EVENT_TABLE()
    };
 
 
 
 

// the ID we'll use to identify our event
const int VAD_UPDATE_ID = 100000;

class MyFrame: public wxFrame
{
  //wxBitmap bmp;
  wxImagePanel * drawPane;   
  wxCheckBox *m_cb;

public:
    
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

/*    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);*/

    // this is called when the event from the thread is received
    void onVadUpdate(wxCommandEvent& evt)
    {
      // get the number sent along the event and use it to update the GUI
      if (evt.GetInt()==1) {
        //m_cb->SetValue( true );
        drawPane->showImg = true;
        drawPane->paintNow();
      } else {
        //m_cb->SetValue( false );
        drawPane->showImg = false;
        drawPane->paintNow();
      }
    }


    

    DECLARE_EVENT_TABLE()
};


class MyApp: public wxApp
{
  virtual bool OnInit();
public:
  MyFrame * mframe;
  
};


class cExampleGuipluginSink : public cDataSink {
  private:
    const char *filename;
    FILE * fHandle;
    int lag;
    //wxCommandEvent &myevent; //( wxEVT_COMMAND_TEXT_UPDATED, VAD_UPDATE_ID );
    int old;

    smileThread guiThr;
    //gcroot<openSmilePluginGUI::TestForm^> fo;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    MyApp* pApp;
    cExampleGuipluginSink(const char *_name);

    virtual ~cExampleGuipluginSink();
};




#endif // __CEXAMPLEGUIPLUGINSINK_HPP
