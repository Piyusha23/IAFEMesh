/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageAutoSegmentGroup.h,v $
Language:  C++
Date:      $Date: 2013/01/30 21:47:11 $
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
// .NAME vtkKWMimxImageAutoSegmentGroup
//
// .SECTION Description
// Tools to help with automated segmentation. This includes
// threshold and region growing techniques. We should move
// the Landmarks a proper object that we can control all
// properties of the object.

#ifndef __vtkKWMimxImageAutoSegmentGroup_h
#define __vtkKWMimxImageAutoSegmentGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkImageActor;
class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMenuButton;
class vtkKWMenuButtonWithLabel;
class vtkMimxLandmarkWidget;
class vtkKWRadioButton;
class vtkKWCheckButtonWithLabel;
class vtkCellPicker;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxImageAutoSegmentGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxImageAutoSegmentGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxImageAutoSegmentGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void ThresholdBelowCallback();
  void ThresholdAboveCallback();
  void ThresholdOutsideCallback();
  
  void ThresholdCallback();
  
  void CancelCallback();
  
  void RegionGrowingCallback();
  void ConnectedThresholdCallback();
  void ConfidenceConnectedCallback();
  void OutputCallback();
  void AxisCallback();
  void UpdateObjectLists();
  int ThresholdAboveApplyCallback();
  int ThresholdBelowApplyCallback();
  int ThresholdOutsideApplyCallback();
  int ConnectedThresholdApplyCallback();
  int ConfidenceConnectedApplyCallback();
  int SaveLandmarksCallback();
  int LoadLandmarksCallback();
  int ClearLandmarksCallback();
  void RadiusCallback(char *value);
  int BinaryResultCallback(int state);
  int ConnectedThresholdSmoothCallback(int state);
  int ConfidenceConnectedSmoothCallback(int state);

  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);
  
protected:
  vtkKWMimxImageAutoSegmentGroup();
  ~vtkKWMimxImageAutoSegmentGroup();
  virtual void CreateWidget();
  
  // List for Image Objects
  vtkKWFrameWithLabel *ListFrame;
  vtkKWComboBoxWithLabel *ImageListComboBox;
  
  // Used to Specify the Types of Segmentation
  vtkKWMenuButton *SegmentationTypeMenu;
  vtkKWMenuButton *ThresholdTypeMenu;
  vtkKWMenuButton *RegionGrowingTypeMenu;
  
  // Threshold Options
  vtkKWFrame *ThresholdOptionFrame;
  vtkKWFrame *ThresholdFrame;
  vtkKWCheckButtonWithLabel *BinaryCheckButton;
  vtkKWEntryWithLabel *ThresholdUpperLimitEntry;
  vtkKWEntryWithLabel *ThresholdLowerLimitEntry;
  vtkKWEntryWithLabel *ThresholdInsideEntry;
  vtkKWEntryWithLabel *ThresholdOutsideEntry;
  
  // Connected Threshold Options
  vtkKWFrame *RegionGrowingOptionFrame;
  vtkKWFrame *ConnectedThresholdFrame;
  vtkKWEntryWithLabel *ConnectedThresholdUpperLimitEntry;
  vtkKWEntryWithLabel *ConnectedThresholdLowerLimitEntry;
  vtkKWCheckButtonWithLabel *ConnectedThresholdSmoothCheckButton;
  vtkKWEntryWithLabel *ConnectedThresholdSmoothEntry;
  vtkKWEntryWithLabel *ConnectedThresholdIterationsEntry;
  vtkKWEntryWithLabel *ConnectedThresholdResultEntry;
  
  // Confidence Connected Threshold Options
  vtkKWFrame *ConfidenceConnectedFrame;
  vtkKWEntryWithLabel *ConfidenceConnectedMultiplierEntry;
  vtkKWEntryWithLabel *ConfidenceConnectedIterationsEntry;
  vtkKWCheckButtonWithLabel *ConfidenceConnectedSmoothCheckButton;
  vtkKWEntryWithLabel *ConfidenceConnectedSmoothEntry;
  vtkKWEntryWithLabel *ConfidenceConnectedSmoothIterationsEntry;
  vtkKWEntryWithLabel *ConfidenceConnectedResultEntry;
  
  // Landmark Controls - Used for Region Growing Tools
  vtkKWFrame *LandmarkOptionFrame;
  vtkKWFrame *LandmarkRadiusFrame;
  vtkKWEntryWithLabel *LandmarkRadiusEntry;
  vtkKWFrame *LandmarkFrame;
  vtkKWPushButton *LandmarkLoadButton;
  vtkKWPushButton *LandmarkSaveButton;
  vtkKWPushButton *LandmarkClearButton;
  
  
  // Used for User Interaction
  vtkMimxLandmarkWidget *LandmarkWidget;
  vtkCellPicker *PlanePicker;

private:
  vtkKWMimxImageAutoSegmentGroup(const vtkKWMimxImageAutoSegmentGroup&); // Not implemented
  void operator=(const vtkKWMimxImageAutoSegmentGroup&); // Not implemented
};

#endif

