/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadMeshGroup.h,v $
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
// .NAME vtkKWMimxLoadMeshGroup - Load a Mesh from disk
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It will load a 
// mesh from disk. The mesh is a vtkUnstructuredGrid. If the
// file extension is .vtk then the old VTL reader is used. For
// all other extensions the new VTK-XML reader is used.

#ifndef __vtkKWMimxLoadMeshGroup_h
#define __vtkKWMimxLoadMeshGroup_h

#include <string>


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"



class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxLoadMeshGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxLoadMeshGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxLoadMeshGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // General public interface to load a mesh.
  // The file type is derived based on the filename.
  //  .vtk = VTK 4.2 unstructured grid
  //  All others = VTK XML format
  void LoadMesh(const char *FilePath);

  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
protected:
  vtkKWMimxLoadMeshGroup();
  ~vtkKWMimxLoadMeshGroup();
  virtual void CreateWidget();
  
  // Description:
  // Internal interface to load VTK XML format data
  void LoadVtkMesh(const char *meshFileName);
  
  // Description:
  // Internal interface to load VTK 4.2 polydata
  void LoadVtkXMLMesh(const char *meshFileName);

private:
  vtkKWMimxLoadMeshGroup(const vtkKWMimxLoadMeshGroup&); // Not implemented
  void operator=(const vtkKWMimxLoadMeshGroup&); // Not implemented
};

#endif


