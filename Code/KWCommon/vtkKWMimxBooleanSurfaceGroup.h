/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxBooleanSurfaceGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/22 21:07:38 $
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
// .NAME vtkKWMimxBooleanSurfaceGroup - a tabbed notebook of UI pages
// .SECTION Description
// Inturn Uses the vtkImplicitPolydata class. Takes in 2 surfaces and 
// computes the boolean of the two surfaces. 4 options are available
//   1. Subtraction (1-2) 
//   2. Subtraction (2-1) 
//   3. Intersection 
//   4. Union
// 

#ifndef __vtkKWMimxBooleanSurfaceGroup_h
#define __vtkKWMimxBooleanSurfaceGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkSphereSource.h"

#include "vtkMimxKWCommonWin32Header.h"


class vtkActor;
class vtkAppendPolyData;
class vtkClipPolyData;
class vtkPolyDataMapper;
class vtkKWMenuButtonWithLabel;
class vtkKWComboBoxWithLabel;
//class vtkImplicitPolyData;
class vtkBooleanOperationPolyDataFilter;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxBooleanSurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxBooleanSurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxBooleanSurfaceGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  // Description:
  // Updating all the entries in the combobox entries
	void UpdateObjectLists();
	
	// Description:
  // Callbacks for user interface
  void SaveVTKSurfaceDoneCallback();
  void SaveVTKSurfaceCancelCallback();
  int SaveVTKSurfaceApplyCallback();
  void UnionSurfaceCallback();
  void IntersectionSurfaceCallback();
  void AdditionSurfaceCallback ();
  void SubtractionSurfaceCallback ();

protected:
	vtkKWMimxBooleanSurfaceGroup();
	~vtkKWMimxBooleanSurfaceGroup();
		
	vtkPolyDataMapper *SelectedSurfaceMapper;	
	vtkActor *SelectedSurfaceActor;
	vtkKWMenuButtonWithLabel *Menubutton;
	
	//vtkImplicitPolyData *Surface1;
	//vtkImplicitPolyData *Surface2;

	vtkSphereSource *Surface1;
	vtkSphereSource *Surface2;

	//vtkClipPolyData *Clipper1;
	//vtkClipPolyData *Clipper2;

	vtkBooleanOperationPolyDataFilter *Clipper1;

	vtkAppendPolyData *AppendSurfaces;

	vtkPolyDataMapper *SelectedSurfaceMapper1;
	vtkActor *SelectedSurfaceActor1; 

	vtkPolyDataMapper *SelectedSurfaceMapper2;
	vtkActor *SelectedSurfaceActor2; 

	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWComboBoxWithLabel *ObjectListComboBox1;
	
	virtual void CreateWidget();

private:
  vtkKWMimxBooleanSurfaceGroup(const vtkKWMimxBooleanSurfaceGroup&); // Not implemented
  void operator=(const vtkKWMimxBooleanSurfaceGroup&); // Not implemented
};

#endif

