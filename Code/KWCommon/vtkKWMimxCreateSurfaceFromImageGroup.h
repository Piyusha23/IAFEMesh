/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateSurfaceFromImageGroup.h,v $
Language:  C++
Date:      $Date: 2013/02/10 01:30:39 $
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
// .NAME vtkKWMimxCreateSurfaceFromImageGroup - Create a surface from an image
// .SECTION Description
// The class will create a triangulated surface based on an image or
// segmentation

#ifndef __vtkKWMimxCreateSurfaceFromImageGroup_h
#define __vtkKWMimxCreateSurfaceFromImageGroup_h

//MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"
#include "mimxCommonDefine.h"

class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMenuButton;
class vtkKWSpinBoxWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkPolyData;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxCreateSurfaceFromImageGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxCreateSurfaceFromImageGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxCreateSurfaceFromImageGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  //Callbacks for pressing buttons on the GUI
  void CreateSurfaceFromImageCancelCallback();
  int CreateSurfaceFromImageApplyCallback();
  int LabelImageCallback(int state);
  
  //BTX
  //Create the Surface from a binary image
  vtkPolyData* CreateBinarySurface(ImageType::Pointer image, ImagePixelType surfaceValue, float rms, int iterations);
  
  //Create the Surface from a gray scale image
  vtkPolyData* CreateContinuosSurface(ImageType::Pointer image, float surfaceValue);
  //ETX
  
protected:
  vtkKWMimxCreateSurfaceFromImageGroup();
  ~vtkKWMimxCreateSurfaceFromImageGroup();
  virtual void CreateWidget();

  vtkKWComboBoxWithLabel    *ObjectListComboBox;
  vtkKWFrameWithLabel       *ComponentFrame;
  vtkKWFrame                *ParameterFrame;
  vtkKWCheckButtonWithLabel *LabelImageButton;
  vtkKWSpinBoxWithLabel     *SurfaceValueEntry;
  vtkKWSpinBoxWithLabel     *RMSvalueEntry;
  vtkKWSpinBoxWithLabel     *ITKiterationsEntry;
  vtkKWSpinBoxWithLabel     *VTKiterationsEntry;
  vtkKWSpinBoxWithLabel     *DecimationValueEntry;
  
  
private:
  vtkKWMimxCreateSurfaceFromImageGroup(const vtkKWMimxCreateSurfaceFromImageGroup&);
  void operator=(const vtkKWMimxCreateSurfaceFromImageGroup&);
  
};

#endif


