/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveFeatureGroup.h,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:59 $
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
// .NAME vtkKWMimxSaveFeatureGroup - GUI for saving surfaces.
// .SECTION Description
// The class contains a list of features that could be saved in either 
// vtkPolyData, or VTK XML based format.

#ifndef __vtkKWMimxSaveFeatureGroup_h
#define __vtkKWMimxSaveFeatureGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWComboBoxWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkPolyData;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxSaveFeatureGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSaveFeatureGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSaveFeatureGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  // Description:
  // Callbacks for push buttons
  //void SaveSurfaceDoneCallback();
  int SaveFeatureApplyCallback();
  void SaveFeatureCancelCallback();
  
  // Description:
  // Updates the lists used for selection
  void UpdateObjectLists();
  
protected:
  vtkKWMimxSaveFeatureGroup();
  ~vtkKWMimxSaveFeatureGroup();
  virtual void CreateWidget();
  
  void SaveVTKFeature(const char *fileName, vtkPolyData *polyData, bool binary);
  void SaveVTKXMLFeature(const char *fileName, vtkPolyData *polyData, bool binary);
  
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWCheckButtonWithLabel *BinaryCheckButton;
  
private:
  vtkKWMimxSaveFeatureGroup(const vtkKWMimxSaveFeatureGroup&); // Not implemented
  void operator=(const vtkKWMimxSaveFeatureGroup&); // Not implemented
};

#endif


