/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainWindow.h,v $
Language:  C++
Date:      $Date: 2013/01/15 23:05:54 $
Version:   $Revision: 1.3 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) The University of Iowa. All rights reserved.
See MIMXCopyright.txt or http://www.ccad.uiowa.edu/mimx/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkKWMimxMainUserInterfacePanel - Main window containing user
// interface panel and view window.
//
// .SECTION Description
// This is the main window containing user interface panels for operations
// and the view window to view the objects. Also contains access to a pop-up
// window to change the display settings.

#ifndef __vtkKWMimxMainWindow_h
#define __vtkKWMimxMainWindow_h

// KWWidgets Headers
#include "vtkKWWindow.h"

// MIMX Headers
#include "vtkMimxKWCommonWin32Header.h"


class vtkCallbackCommand;
class vtkRenderer;
class vtkPVAxesActor;
class vtkKWRenderWidget;
class vtkKWChangeColorButton;
class vtkMimxErrorCallback;
class vtkKWFrameWithScrollbar;
class vtkKWMenu;
class vtkKWMimxViewProperties;
class vtkLinkedListWrapperTree;
class vtkKWLoadSaveDialog;
class vtkKWMimxMainUserInterfacePanel;
class vtkKWMimxDisplayPropertiesGroup;
class vtkKWIcon;
class vtkKWMimxSaveSceneGroup;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxMainWindow : public vtkKWWindow
{
public:
	static vtkKWMimxMainWindow* New();
	vtkTypeRevisionMacro(vtkKWMimxMainWindow,vtkKWWindow);
	vtkGetObjectMacro(RenderWidget, vtkKWRenderWidget);
  vtkGetObjectMacro(TraceRenderWidget, vtkKWRenderWidget);
	vtkGetObjectMacro(ErrorCallback, vtkMimxErrorCallback);
	vtkGetObjectMacro(ViewProperties, vtkKWMimxViewProperties);
	vtkGetObjectMacro(PVAxesActor, vtkPVAxesActor);
	vtkGetObjectMacro(
		MainUserInterfacePanel, vtkKWMimxMainUserInterfacePanel);
	vtkKWRenderWidget *RenderWidget;
	vtkKWRenderWidget *TraceRenderWidget;
	vtkRenderer *AxesRenderer;
	vtkPVAxesActor *PVAxesActor;
	vtkCallbackCommand *CallbackCommand;
	
	void DisplayPropertyCallback();
	void SetApplicationFontFamily( const char *font );
	void SetApplicationFontSize( const char *size );
	void AutoSaveCallback( );
	void EnableAutoSave( );
	void DisableAutoSave( );
	
	
	vtkKWIcon* GetApplyButtonIcon();
	vtkKWIcon* GetCancelButtonIcon();
	
  // Description:
  // Callbacks for loading and saving the scene. Internal, do not use.
  virtual void LoadSceneCallback();
  virtual void SaveSceneCallback();
  
	// Description:
  // Get the Application Settings Interface as well as the Application
  // Settings User Interface Manager.
  virtual vtkKWApplicationSettingsInterface *GetApplicationSettingsInterface();
	
protected:
	vtkKWMimxMainWindow();
	~vtkKWMimxMainWindow();
	virtual void CreateWidget();
	vtkKWChangeColorButton *ChangeColorButton;
	vtkMimxErrorCallback *ErrorCallback;
	vtkKWMenu *ViewMenu;
	vtkKWFrameWithScrollbar *MainNoteBookFrameScrollbar;
	vtkKWMimxViewProperties *ViewProperties;
	vtkLinkedListWrapperTree *DoUndoTree;
	vtkKWLoadSaveDialog *LoadSaveDialog;
	vtkKWMimxMainUserInterfacePanel *MainUserInterfacePanel;
	vtkKWMimxDisplayPropertiesGroup *DisplayPropertyDialog;
  vtkKWMimxSaveSceneGroup *SaveSceneGroup;
	
private:
	vtkKWMimxMainWindow(const vtkKWMimxMainWindow&);   // Not implemented.
	void operator=(const vtkKWMimxMainWindow&);  // Not implemented.
	
	vtkKWIcon *applyIcon;
	vtkKWIcon *cancelIcon; 
	
	char autoSaveEventId[256];                 
};

void updateAxis(vtkObject* caller, unsigned long , void* arg, void* );

#endif
