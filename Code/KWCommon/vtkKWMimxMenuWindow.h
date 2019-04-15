/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMenuWindow.h,v $
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

#ifndef __vtkKWMimxMenuWindow_h
#define __vtkKWMimxMenuWindow_h


// MIMX Headers
#include "vtkKWWindowBase.h"
#include "vtkKWMimxViewWindow.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWFrameWithScrollbar;
class vtkKWMenu;
class vtkKWMimxMainNotebook;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxMenuWindow : public vtkKWWindowBase
{
public:
	static vtkKWMimxMenuWindow* New();
	vtkTypeRevisionMacro(vtkKWMimxMenuWindow,vtkKWWindowBase);
	vtkSetObjectMacro(MimxViewWindow, vtkKWMimxViewWindow);
	vtkGetObjectMacro(MimxViewWindow, vtkKWMimxViewWindow);
	
protected:
	vtkKWMimxMenuWindow();
	~vtkKWMimxMenuWindow();
	virtual void CreateWidget();
	vtkKWMenu *ViewMenu;
	vtkKWFrameWithScrollbar *MainNoteBookFrameScrollbar;
	vtkKWMimxMainNotebook *MainNotebook;
	vtkKWMimxViewWindow *MimxViewWindow;
	
private:
	vtkKWMimxMenuWindow(const vtkKWMimxMenuWindow&);   // Not implemented.
	void operator=(const vtkKWMimxMenuWindow&);  // Not implemented.
};

#endif
