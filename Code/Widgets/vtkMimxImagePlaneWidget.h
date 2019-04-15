/*=========================================================================
 
 Program:   MIMX Meshing Toolkit
 Module:    $RCSfile: vtkMimxImagePlaneWidget.h,v $
 Language:  C++
 
 Date:      $Date: 2013/01/31 14:51:36 $
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
// .NAME vtkMimxImagePlaneWidget - A simple extension of vtkImagePlaneWidget
// .SECTION Description
// The class simply derives from vtkImagePlaneWidget and adds the ability
// to get the TexturePlaneActor and PlanePicker used in vtkImagePlaneWidget.
// This functionality is used to pick seed locations and provide manual 
// editing of regions-of-interest.


#ifndef __vtkMimxImagePlaneWidget_h
#define __vtkMimxImagePlaneWidget_h

#include <vtkImagePlaneWidget.h>
#include "vtkMimxWidgetsWin32Header.h"

class vtkAbstractPropPicker;
class vtkActor;
class vtkTransform;


class VTK_MIMXWIDGETS_EXPORT vtkMimxImagePlaneWidget : public vtkImagePlaneWidget
{
public:
  static vtkMimxImagePlaneWidget *New();
	vtkTypeRevisionMacro(vtkMimxImagePlaneWidget,vtkImagePlaneWidget);

	vtkActor *GetTexturePlaneActor();
	vtkAbstractPropPicker *GetPlanePicker();

  void SetIJKToLPSTransform(vtkTransform *transform);
  
  //Overloaded functions that replace functionality in vtkImagePlaneWidget
  void OnChar();
  void StartCursor();
  void OnMouseMove();
  void ManageTextDisplay();
  
protected:
	vtkMimxImagePlaneWidget();
	~vtkMimxImagePlaneWidget();
  
  vtkTransform *IJKToLPSTransform;
};

#endif
