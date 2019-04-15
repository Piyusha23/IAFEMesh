/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateMaskFromFeatureGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/14 02:54:31 $
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
// .NAME vtkKWMimxCreateMaskFromFeatureGroup - Create a surface from an image
// .SECTION Description
// The class will create a binary image from a contour defined by the user.

#ifndef __vtkKWMimxCreateMaskFromFeatureGroup_h
#define __vtkKWMimxCreateMaskFromFeatureGroup_h

//MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"
#include "mimxCommonDefine.h"

class vtkKWComboBoxWithLabel;
class vtkKWFrameWithLabel;
class vtkKWSpinBoxWithLabel;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxCreateMaskFromFeatureGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxCreateMaskFromFeatureGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxCreateMaskFromFeatureGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  //Callbacks for pressing buttons on the GUI
  void CreateMaskFromFeatureCancelCallback();
  int  CreateMaskFromFeatureApplyCallback();
  int GetFeaturePlane(vtkPolyData *feature);
  void UpdateImageList();
  void UpdateFeatureList();
  
protected:
  vtkKWMimxCreateMaskFromFeatureGroup();
  ~vtkKWMimxCreateMaskFromFeatureGroup();
  virtual void CreateWidget();

  vtkKWComboBoxWithLabel    *ImageListComboBox;
  vtkKWComboBoxWithLabel    *FeatureListComboBox;
  vtkKWFrameWithLabel       *ComponentFrame;
  vtkKWFrame                *ParameterFrame;
  vtkKWSpinBoxWithLabel     *FillValueEntry;
  
  
private:
  vtkKWMimxCreateMaskFromFeatureGroup(const vtkKWMimxCreateMaskFromFeatureGroup&);
  void operator=(const vtkKWMimxCreateMaskFromFeatureGroup&);

};

#endif


