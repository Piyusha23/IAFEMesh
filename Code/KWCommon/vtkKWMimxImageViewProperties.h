/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageViewProperties.h,v $
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
// .NAME vtkKWMimxImageViewProperties - a tabbed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxImageViewProperties_h
#define __vtkKWMimxImageViewProperties_h


// MIMX Headers
#include "vtkKWCompositeWidget.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWFrameWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkLinkedListWrapper;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxImageViewProperties : public vtkKWCompositeWidget
{
public:
  static vtkKWMimxImageViewProperties* New();
  vtkTypeRevisionMacro(vtkKWMimxImageViewProperties,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkSetObjectMacro(ObjectList, vtkLinkedListWrapper);
  vtkGetObjectMacro(MainFrame, vtkKWFrameWithLabel);
  void VisibilityCallback(int );
  void AddObjectList();
  void DeleteObjectList(int );
  
protected:
	vtkKWMimxImageViewProperties();
	~vtkKWMimxImageViewProperties();
	virtual void CreateWidget();
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;
  vtkKWMimxMainWindow *MimxMainWindow;
  vtkLinkedListWrapper *ObjectList;
  vtkKWFrameWithLabel *MainFrame;
  
private:
  vtkKWMimxImageViewProperties(const vtkKWMimxImageViewProperties&); // Not implemented
  void operator=(const vtkKWMimxImageViewProperties&); // Not implemented
};

#endif

