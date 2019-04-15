/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMenuOptionGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/01 21:45:59 $
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
// .NAME vtkKWMimxMenuOptionGroup - Base class for all the GUI used
// .SECTION Description
// The class contains two push buttons for Apply and Cancel and a Frame 
// which houses all the GUI.
//
// .SECTION To Be Done
// Should also incorporate combobox widget which is used for selection of an
// object and is common in all the derived classes. Should also include
// UpdateObjectLists function common in all the derived classes.

#ifndef __vtkKWMimxMenuOptionGroup_h
#define __vtkKWMimxMenuOptionGroup_h

// VTK Headers
#include "vtkCommand.h"
#include "vtkLinkedListWrapper.h"
#include "vtkLinkedListWrapperTree.h"


// KWWidget Headers
#include "vtkKWComboBox.h"
#include "vtkKWFrame.h"

// MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxViewProperties.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkKWMimxViewProperties.h"
#include "vtkMimxKWCommonWin32Header.h"

/*
typedef itk::Image<signed short, 3>  ImageType;
typedef ImageType::Pointer           ImagePointerType;
typedef itk::Image<float, 3>         FloatImageType;
typedef FloatImageType::Pointer      FloatImagePointerType;
typedef itk::Image<unsigned char, 3> CharImageType;
typedef CharImageType::Pointer       CharImagePointerType;
*/

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxMenuOptionGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxMenuOptionGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxMenuOptionGroup,vtkKWCompositeWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();

  vtkSetObjectMacro(MenuGroup, vtkKWMimxMainMenuGroup);

  vtkGetObjectMacro(BaseFrame, vtkKWFrame);

  // Description:
  // Get/Set the use of a Frame with slider if requested.
  vtkSetMacro(UseFrameWithSlider, int);
  vtkGetMacro(UseFrameWithSlider, int);
  
  // Description:
  // Set the use of Apply/Cancel Buttons at the bottom of the menu
  void SetUseApplyCancelButtons(int useButtons);
  void SetUseApplyCancelButtonsOn();
  void SetUseApplyCancelButtonsOff();
  
  // Description:
  // Get/Set the use of Apply Buttons at the bottom of the menu
  vtkSetMacro(UseApplyButton, int);
  vtkGetMacro(UseApplyButton, int);
  
  // Description:
  // Get/Set the use of Cancel Buttons at the bottom of the menu
  vtkSetMacro(UseCancelButton, int);
  vtkGetMacro(UseCancelButton, int);
  
  // Description:
  // This is used to count the number of objects generated from a
  // particular type of operation. Initially it was assumed that each group
  // had only one object creation operation. With the changes made, the group
  // can have more than one operation.
  vtkSetMacro(Count, int);
  
  // Description:
  // Unpack the group from the interface. This is a convenience method to hide
  // the entire group of widgets
  void UnpackGroup();
  
  // Description:
  // Pack the group into the interface. This is a convenience method to pack
  // the entire group of widgets into the interface
  void PackGroup();
  
  // Description:
  // Extract the filename of the object without an extention
  const char* ExtractFileName(const char* FName);
  
protected:
  vtkKWMimxMenuOptionGroup();
  virtual ~vtkKWMimxMenuOptionGroup();
  virtual void CreateWidget();
  vtkKWPushButton *ApplyButton;
  vtkKWPushButton *CancelButton;
  vtkKWFrame *BaseFrame;
  vtkKWFrame *ApplyCancelButtonFrame;
  vtkKWFrameWithScrollbar *MainFrameWithSlider;

  vtkKWMimxMainMenuGroup *MenuGroup;

  vtkIdType Count;  // to keep track of number of objects created during runtime 
  char objectSelectionPrevious[256];
  int CancelStatus;
  int UseFrameWithSlider;
  int UseApplyButton;
  int UseCancelButton;
private:
  vtkKWMimxMenuOptionGroup(const vtkKWMimxMenuOptionGroup&); // Not implemented
  void operator=(const vtkKWMimxMenuOptionGroup&); // Not implemented
 };

#endif


