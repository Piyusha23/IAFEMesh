/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadFeatureGroup.h,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:58 $
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
// .NAME vtkKWMimxLoadFeatureGroup - Load a Features from disk
// .SECTION Description
// The class is derived from vtkKWMimxMenuOptionGroup. It will load a 
// features (typically lines) from disk

#ifndef __vtkKWMimxLoadFeatureGroup_h
#define __vtkKWMimxLoadFeatureGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWFileBrowserDialog;
class vtkKWLoadSaveDialog;
class vtkEntryChangedEventCallback;
class vtkKWFrameWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxLoadFeatureGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxLoadFeatureGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxLoadFeatureGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
protected:
  vtkKWMimxLoadFeatureGroup();
  ~vtkKWMimxLoadFeatureGroup();
  virtual void CreateWidget();
  
  void LoadVtkXMLPolyData(const char *fileName);
  void LoadVtkPolyData(const char *fileName);
  
private:
  vtkKWMimxLoadFeatureGroup(const vtkKWMimxLoadFeatureGroup&); // Not implemented
  void operator=(const vtkKWMimxLoadFeatureGroup&); // Not implemented
};

#endif


