/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveMeshGroup.h,v $
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
// .NAME vtkKWMimxSaveMeshGroup - GUI for saving FE mesh
// .SECTION Description
// Class contains the list of FE mesh objects to be saved in vtk or VTK
// XML file format. The meshes will be saved as unstructured grids.

#ifndef __vtkKWMimxSaveMeshGroup_h
#define __vtkKWMimxSaveMeshGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkUnstructuredGrid;
class vtkKWCheckButtonWithLabel;
class vtkKWComboBoxWithLabel;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSaveMeshGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSaveMeshGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveMeshGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  // Description:
  // Call backs for the push bottons
  void SaveMeshCancelCallback();
  int SaveMeshApplyCallback();
  
  // Description:
  // Updates the lists used for selection
  void UpdateObjectLists();
  
protected:
  vtkKWMimxSaveMeshGroup();
  ~vtkKWMimxSaveMeshGroup();
  virtual void CreateWidget();
  void WriteXMLUnstructuredGrid(const char *, vtkUnstructuredGrid *, bool binary);
  void WriteUnstructuredGrid(const char *, vtkUnstructuredGrid *, bool binary);
  
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWCheckButtonWithLabel *BinaryCheckButton;
  
private:
  vtkKWMimxSaveMeshGroup(const vtkKWMimxSaveMeshGroup&); // Not implemented
	void operator=(const vtkKWMimxSaveMeshGroup&); // Not implemented
};

#endif


