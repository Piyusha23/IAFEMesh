/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSmoothSurfaceGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/16 22:16:55 $
Version:   $Revision: 1.1 $

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
// VtkSmoothPolyDataFilter filter as well as VtkWindowedSincPolyDataFiler are
// available. vtkSmoothPolyDataFilter adjusts point positions using Laplacian smoothing
// where as VtkWindowedSincPolyDataFiler adjust point positions using a windowed sinc 
// function interpolation kernel.

#ifndef __vtkKWMimxSmoothSurfaceGroup_h
#define __vtkKWMimxSmoothSurfaceGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkActor;
class vtkPolyDataMapper;
class vtkSmoothPolyDataFilter;
class vtkWindowedSincPolyDataFilter;

class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWMenuButtonWithLabel;


class  VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSmoothSurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSmoothSurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSmoothSurfaceGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void SaveVTKSurfaceDoneCallback();
  void SaveVTKSurfaceCancelCallback();
  int SaveVTKSurfaceApplyCallback();
  void UpdateObjectLists();
  void WSSmoothSurfaceCallback();
  void LSmoothSurfaceCallback();
  void Smoothvaluechangedcallback(const char *);

protected:
	vtkKWMimxSmoothSurfaceGroup();
	~vtkKWMimxSmoothSurfaceGroup();
	
	vtkPolyDataMapper *SelectedSurfaceMapper;
	vtkActor *SelectedSurfaceActor;
	vtkKWMenuButtonWithLabel *Menubutton;	
	vtkSmoothPolyDataFilter *LSmooth;
	vtkWindowedSincPolyDataFilter *Smooth ;
	
	vtkKWEntryWithLabel *Entrybutton;
	

	virtual void CreateWidget();
	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWLoadSaveDialog *FileBrowserDialog;

private:
  vtkKWMimxSmoothSurfaceGroup(const vtkKWMimxSmoothSurfaceGroup&); // Not implemented
  void operator=(const vtkKWMimxSmoothSurfaceGroup&); // Not implemented
};

#endif

