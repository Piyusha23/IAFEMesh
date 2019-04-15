/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveSurfaceGroup.h,v $
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
// .NAME vtkKWMimxSaveSurfaceGroup - GUI for saving surfaces.
// .SECTION Description
// The class contains a list of surfaces that could be saved in either 
// STL format, vtkPolyData, or VTK XML based format.

#ifndef __vtkKWMimxSaveSurfaceGroup_h
#define __vtkKWMimxSaveSurfaceGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkPolyData;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSaveSurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSaveSurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveSurfaceGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  // Description:
  // Callbacks for push buttons
  //void SaveSurfaceDoneCallback();
  int SaveSurfaceApplyCallback();
  void SaveSurfaceCancelCallback();
  
  // Description:
  // Updates the lists used for selection
  void UpdateObjectLists();
  
protected:
  vtkKWMimxSaveSurfaceGroup();
  ~vtkKWMimxSaveSurfaceGroup();
  virtual void CreateWidget();
  
  void SaveSTLSurface(const char *fileName, vtkPolyData *polyData, bool binary);
  void SaveVTKSurface(const char *fileName, vtkPolyData *polyData, bool binary);
  void SaveVTKXMLSurface(const char *fileName, vtkPolyData *polyData, bool binary);
  
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWCheckButtonWithLabel *BinaryCheckButton;
  
private:
  vtkKWMimxSaveSurfaceGroup(const vtkKWMimxSaveSurfaceGroup&); // Not implemented
  void operator=(const vtkKWMimxSaveSurfaceGroup&); // Not implemented
};

#endif


