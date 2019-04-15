/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateVoxelMesh.h,v $
Language:  C++
Date:      $Date: 2013/04/04 02:44:10 $
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
// .NAME vtkKWMimxCreateVoxelMesh - User interface to create a voxel Mesh
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. 

#ifndef __vtkKWMimxCreateVoxelMesh_h
#define __vtkKWMimxCreateVoxelMesh_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWEntry;
class vtkKWCheckButtonWithLabel;
class vtkKWFrame;
class vtkPolyData;
class vtkKWMimxNodeElementNumbersGroup;
class vtkMimxApplyNodeElementNumbers;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxCreateVoxelMesh : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxCreateVoxelMesh* New();
  vtkTypeRevisionMacro(vtkKWMimxCreateVoxelMesh,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void VoxelMeshCancelCallback();
  int VoxelMeshApplyCallback();
  void LabelMapCallback( int );
  void UpdateObjectLists();
  void AddMeshToDisplay(
  vtkUnstructuredGrid *mesh, const char *namePrefix, const char *FoundationName, int index);
  void InitializeMeshFieldData(vtkUnstructuredGrid *mesh, 
                const char *nodeSetName, const char *elementSetName, int nodeNumber, int elementNumber);

protected:
  vtkKWMimxCreateVoxelMesh();
  ~vtkKWMimxCreateVoxelMesh();
  virtual void CreateWidget();

  vtkKWComboBoxWithLabel *ImageListComboBox;
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWCheckButtonWithLabel *LabelMapButton;
  vtkKWEntryWithLabel *LabelMapValueEntry;
  void RemovePreviousSelectionDisplay();
  vtkKWEntryWithLabel *ResampleFactorEntry;
  vtkKWMimxNodeElementNumbersGroup *NodeElementNumbersGroup;

private:
  vtkKWMimxCreateVoxelMesh(const vtkKWMimxCreateVoxelMesh&);
  void operator=(const vtkKWMimxCreateVoxelMesh&);
};

#endif


