/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadSurfaceGroup.h,v $
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
// .NAME vtkKWMimxLoadSurfaceGroup - User Interface to Load a Surface from disk
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It will load a 
// surface mesh from disk

#ifndef __vtkKWMimxLoadSurfaceGroup_h
#define __vtkKWMimxLoadSurfaceGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;
class vtkEntryChangedEventCallback;
class vtkKWFrameWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxLoadSurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxLoadSurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxLoadSurfaceGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  // Description:
  // General public interface to load a surface.
  // The file type is derived based on the filename.
  //  .stl = STereoLithography file format 
  //  .vtk = VTK 4.2 Poly Data format
  //  All others = VTK XML format
  void LoadSurface(const char *FilePath);

protected:
  vtkKWMimxLoadSurfaceGroup();
  ~vtkKWMimxLoadSurfaceGroup();
  virtual void CreateWidget();
  
  // Description:
  // Internal interface to load VTK XML format data
  void LoadVtkXMLPolyData(const char* FilePath);
  
  // Description:
  // Internal interface to load VTK 4.2 polydata
  void LoadVtkPolyData(const char* FilePath);
  
  // Description:
  // Internal interface to load stereolithography files
  void ReadStlPolyDataFile(const char* FilePath);

private:
  vtkKWMimxLoadSurfaceGroup(const vtkKWMimxLoadSurfaceGroup&); // Not implemented
  void operator=(const vtkKWMimxLoadSurfaceGroup&); // Not implemented
};

#endif


