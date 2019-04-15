/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageThresholdGroup.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
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
// .NAME vtkKWMimxImageThresholdGroup - Threshold an image operations
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase and performs thresholding
// operations on an image.

#ifndef __vtkKWMimxImageThresholdGroup_h
#define __vtkKWMimxImageThresholdGroup_h


// MIMX Headers
#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkKWComboBoxWithLabel;
class vtkKWFrameWithLabel;
class vtkKWFrame;
class vtkKWRadioButton;
class vtkKWCheckButton;
class vtkKWRadioButtonSet;
class vtkKWEntryWithLabel;


class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxImageThresholdGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxImageThresholdGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxImageThresholdGroup,vtkKWMimxGroupBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  int  ThresholdImageApplyCallback( );
  void ThresholdImageCancelCallback();
  void UpdateObjectLists();
  
  
protected:
	vtkKWMimxImageThresholdGroup();
	~vtkKWMimxImageThresholdGroup();
	virtual void CreateWidget();
  
  /* These are specific for the Interface */
  vtkKWFrameWithLabel *ComponentFrame;
  vtkKWComboBoxWithLabel *ObjectListComboBox;
  vtkKWFrame *EntryFrame;
  vtkKWEntryWithLabel *LowerThresholdEntry;
  vtkKWEntryWithLabel *UpperThresholdEntry;
  
  
private:
  vtkKWMimxImageThresholdGroup(const vtkKWMimxImageThresholdGroup&); // Not implemented
  void operator=(const vtkKWMimxImageThresholdGroup&); // Not implemented
};

#endif

