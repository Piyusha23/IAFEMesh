/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveVTKFEMeshGroup.h,v $
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
// .NAME vtkKWMimxSaveVTKFEMeshGroup - a taFEMeshed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxSaveVTKFEMeshGroup_h
#define __vtkKWMimxSaveVTKFEMeshGroup_h



// MIMX Headers
#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWFrameWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSaveVTKFEMeshGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxSaveVTKFEMeshGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveVTKFEMeshGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  void SaveVTKFEMeshCancelCallback();
  int SaveVTKFEMeshApplyCallback();
  void UpdateObjectLists();
  
protected:
	vtkKWMimxSaveVTKFEMeshGroup();
	~vtkKWMimxSaveVTKFEMeshGroup();
	virtual void CreateWidget();
	
	vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWLoadSaveDialog *FileBrowserDialog;
private:
  vtkKWMimxSaveVTKFEMeshGroup(const vtkKWMimxSaveVTKFEMeshGroup&); // Not implemented
	void operator=(const vtkKWMimxSaveVTKFEMeshGroup&); // Not implemented
};

#endif

