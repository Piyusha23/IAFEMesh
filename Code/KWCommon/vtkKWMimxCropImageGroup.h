/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCropImageGroup.h,v $
Language:  C++
Date:      $Date: 2013/02/17 22:20:18 $
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
// .NAME vtkKWMimxCropImageGroup - Crop the image to a ROI
// .SECTION Description
// The class will crop an image to a rectangular region of interest
// specified by the user.

#ifndef __vtkKWMimxCropImageGroup_h
#define __vtkKWMimxCropImageGroup_h

//MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"
#include "mimxCommonDefine.h"

class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWExtent;
class vtkBoxWidget;
class vtkMimxImageActor;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxCropImageGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxCropImageGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxCropImageGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual void Update();
  virtual void UpdateEnableState();
  void UpdateObjectLists();
  
  //Callbacks for pressing Apply/Cancel buttons on the GUI
  void CropImageCancelCallback();
  int CropImageApplyCallback();
  
  //Callback to handle changing of the image
  void EnableCropImageCallback(char *value);
  
  //Callback to handle slider changes to the image extent
  void CropSliderCallback(double xl, double uu, double yl, double yu, double zl, double zu);
  
protected:
  vtkKWMimxCropImageGroup();
  ~vtkKWMimxCropImageGroup();
  virtual void CreateWidget();

  vtkKWComboBoxWithLabel    *ObjectListComboBox;
  vtkKWFrameWithLabel       *ComponentFrame;
  vtkKWFrameWithLabel       *ParameterFrame;
  vtkKWExtent               *CropExtents;
  
  vtkBoxWidget              *BoxWidget;
  vtkMimxImageActor         *CurrentImageActor;
  
private:
  vtkKWMimxCropImageGroup(const vtkKWMimxCropImageGroup&);
  void operator=(const vtkKWMimxCropImageGroup&);
  
};

#endif


