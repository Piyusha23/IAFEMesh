/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateFeatureFromMaskGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/16 21:24:42 $
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
// .NAME vtkKWMimxCreateFeatureFromMaskGroup - Create a surface from an image
// .SECTION Description
// The class will create a contour that can be editted from a binary image or
// label map

#ifndef __vtkKWMimxCreateFeatureFromMaskGroup_h
#define __vtkKWMimxCreateFeatureFromMaskGroup_h

//MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"
#include "mimxCommonDefine.h"

class vtkKWComboBoxWithLabel;
class vtkKWFrameWithLabel;
class vtkKWSpinBoxWithLabel;
class vtkKWMenuButtonWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxCreateFeatureFromMaskGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxCreateFeatureFromMaskGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxCreateFeatureFromMaskGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  //Callbacks for pressing buttons on the GUI
  void CreateMaskFromFeatureCancelCallback();
  int  CreateMaskFromFeatureApplyCallback();
  int GetFeaturePlane(vtkPolyData *feature);
  
protected:
  vtkKWMimxCreateFeatureFromMaskGroup();
  ~vtkKWMimxCreateFeatureFromMaskGroup();
  virtual void CreateWidget();

  vtkKWComboBoxWithLabel    *ImageListComboBox;
  vtkKWFrameWithLabel       *ComponentFrame;
  vtkKWFrame                *ParameterFrame;
  vtkKWMenuButtonWithLabel  *TraceAxisMenu;
  vtkKWSpinBoxWithLabel     *LabelValueEntry;
  vtkKWSpinBoxWithLabel     *MaxPointsEntry;
  
private:
  vtkKWMimxCreateFeatureFromMaskGroup(const vtkKWMimxCreateFeatureFromMaskGroup&);
  void operator=(const vtkKWMimxCreateFeatureFromMaskGroup&);

};

#endif


