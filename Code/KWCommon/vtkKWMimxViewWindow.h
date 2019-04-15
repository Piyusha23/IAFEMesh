/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxViewWindow.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
Version:   $Revision: 1.2 $

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

#ifndef __vtkKWMimxViewWindow_h
#define __vtkKWMimxViewWindow_h

// KWWidgets Headers
#include "vtkKWWindowBase.h"

// MIMX Headers
#include "vtkMimxKWCommonWin32Header.h"

class vtkCallbackCommand;
class vtkRenderer;
class vtkPVAxesActor;
class vtkKWRenderWidget;
class vtkKWChangeColorButton;
class vtkMimxErrorCallback;

//class vtkMimxUnstructuredGridWidget;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxViewWindow : public vtkKWWindowBase
{
public:
  static vtkKWMimxViewWindow* New();
  vtkTypeRevisionMacro(vtkKWMimxViewWindow,vtkKWWindowBase);
  
  vtkGetObjectMacro(RenderWidget, vtkKWRenderWidget);
  vtkGetObjectMacro(ErrorCallback, vtkMimxErrorCallback);
  vtkKWRenderWidget *RenderWidget;
  vtkRenderer *AxesRenderer;
  vtkPVAxesActor *PVAxesActor;
  vtkCallbackCommand *CallbackCommand;
  void ViewWindowProperties();

protected:
  vtkKWMimxViewWindow();
  ~vtkKWMimxViewWindow();
   virtual void CreateWidget();
   vtkKWChangeColorButton *ChangeColorButton;
   vtkMimxErrorCallback *ErrorCallback;
   
private:
  vtkKWMimxViewWindow(const vtkKWMimxViewWindow&);   // Not implemented.
  void operator=(const vtkKWMimxViewWindow&);  // Not implemented.
};
void updateAxis(vtkObject* caller, unsigned long , void* arg, void* );

#endif
