/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxBoundaryConditionsMenuGroup.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
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
// .NAME vtkKWMimxBoundaryConditionsMenuGroup - It is the base class for all Object menu options.
// .SECTION Description
// The class is derived from vtkKWMimxMainMenuGroup. It is the base class
// for all Object menu options.

#ifndef __vtkKWMimxBoundaryConditionsMenuGroup_h
#define __vtkKWMimxBoundaryConditionsMenuGroup_h


// MIMX Headers
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWMimxAssignBoundaryConditionsGroup;
class vtkKWMimxDeleteBoundaryConditionsGroup;
class vtkKWMimxExportAbaqusFEMeshGroup;
class vtkKWMimxSaveMeshGroup;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxBoundaryConditionsMenuGroup : public vtkKWMimxMainMenuGroup
{
public:
  static vtkKWMimxBoundaryConditionsMenuGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxBoundaryConditionsMenuGroup,vtkKWMimxMainMenuGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();  
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  void UpdateObjectLists();
  void AssignBoundaryConditionsCallback();
  void DeleteBoundaryConditionsCallback();
  void SaveMeshCallback();
  void ExportAbaqusFEMeshCallback();
  
protected:
	vtkKWMimxBoundaryConditionsMenuGroup();
	~vtkKWMimxBoundaryConditionsMenuGroup();
	virtual void CreateWidget();
	vtkKWMimxAssignBoundaryConditionsGroup *AssignBoundaryConditionsGroup;
	vtkKWMimxDeleteBoundaryConditionsGroup *DeleteBoundaryConditionsGroup;
	vtkKWMimxSaveMeshGroup *SaveMeshGroup;
  vtkKWMimxExportAbaqusFEMeshGroup *ExportAbaqusFEMeshGroup;
  
private:
  vtkKWMimxBoundaryConditionsMenuGroup(const vtkKWMimxBoundaryConditionsMenuGroup&); // Not implemented
  void operator=(const vtkKWMimxBoundaryConditionsMenuGroup&); // Not implemented

};

#endif
