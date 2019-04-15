/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadBBGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/16 02:17:17 $
Version:   $Revision: 1.4 $

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
// .NAME vtkKWMimxLoadBBGroup - Load a Building Block from disk
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It will load a 
// Building Block from disk. If the file extension is .vtk the class
// will use the old VTK reader and all other files are loaded
// using the new VTK-XML format.

#ifndef __vtkKWMimxLoadBBGroup_h
#define __vtkKWMimxLoadBBGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxLoadBBGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxLoadBBGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxLoadBBGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  // Description:
  // General public interface to load a Building Block.
  // The file type is derived based on the filename.
  //  .vtk = VTK 4.2 Poly Data format
  //  All others = VTK XML format
  void LoadBlock(const char *FilePath);
  
  // Description:
  // Determines if all cells in the file are hexahedron. Returns 1 if
  // all cells are hex and 0 otherwise.
  int CheckCellTypesInUgrid(vtkUnstructuredGrid *Input);

protected:
  vtkKWMimxLoadBBGroup();
  ~vtkKWMimxLoadBBGroup();
  virtual void CreateWidget();
  
  // Description:
  // Internal interface to load VTK XML format data
  void LoadVtkXMLBuildingBlock(const char *meshFileName);
  
  // Description:
  // Internal interface to load VTK 4.2 unstructured grid
  void LoadVtkBuildingBlock(const char *meshFileName);
  
private:
  vtkKWMimxLoadBBGroup(const vtkKWMimxLoadBBGroup&); // Not implemented
  void operator=(const vtkKWMimxLoadBBGroup&); // Not implemented
};

#endif


