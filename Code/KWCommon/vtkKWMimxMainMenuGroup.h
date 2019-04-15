/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMainMenuGroup.h,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:58 $
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
// .NAME vtkKWMimxMainMenuGroup - It is the base class for all operations menu options.
// .SECTION Description
// The class is derived from vtkKWCompositeWidget. It is the base class
// for all operations menu options.

#ifndef __vtkKWMimxMainMenuGroup_h
#define __vtkKWMimxMainMenuGroup_h

// VTK Headers
#include "vtkLinkedListWrapper.h"
#include "vtkLinkedListWrapperTree.h"

// MIMX Headers
#include "vtkKWCompositeWidget.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxActorBase.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWMenuButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMimxViewProperties;
class vtkKWMessageDialog;
class vtkKWRenderWidget;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxMainMenuGroup : public vtkKWCompositeWidget
{
public:
  static vtkKWMimxMainMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxMainMenuGroup,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Update();
  virtual void UpdateEnableState();
  
  vtkSetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(MimxMainWindow, vtkKWMimxMainWindow);
  vtkGetObjectMacro(SurfaceList, vtkLinkedListWrapper);
  vtkGetObjectMacro(FeatureList, vtkLinkedListWrapper);
  vtkGetObjectMacro(BBoxList, vtkLinkedListWrapper);
  vtkGetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  vtkGetObjectMacro(ImageList, vtkLinkedListWrapper);
  vtkGetObjectMacro(ImageMaskList, vtkLinkedListWrapper);
  vtkSetObjectMacro(DoUndoTree, vtkLinkedListWrapperTree);
  vtkGetObjectMacro(MainFrame, vtkKWFrame);
  virtual void SetMenuButtonsEnabled(int);
  
 protected:
	vtkKWMimxMainMenuGroup();
	virtual ~vtkKWMimxMainMenuGroup();

	virtual void CreateWidget();
	vtkKWMenuButtonWithLabel *ObjectMenuButton;
	vtkKWMenuButtonWithLabel *OperationMenuButton;
	vtkKWMenuButtonWithLabel *TypeMenuButton;
  vtkKWFrame *MainFrame;
  vtkLinkedListWrapper *SurfaceList;
  vtkLinkedListWrapper *FeatureList;
  vtkLinkedListWrapper *BBoxList;
  vtkLinkedListWrapper *FEMeshList;
  vtkLinkedListWrapper *ImageList;
  vtkLinkedListWrapper *ImageMaskList;
	const char* ExtractFileName(const char*);
	vtkKWMimxMainWindow *MimxMainWindow;
	vtkKWMimxViewProperties *ViewProperties;
	vtkLinkedListWrapperTree *DoUndoTree;

	int ErrorState;

private:
  vtkKWMimxMainMenuGroup(const vtkKWMimxMainMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxMainMenuGroup&); // Not implemented
};

#endif


