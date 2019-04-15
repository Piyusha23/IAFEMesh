/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageProcessingGroup.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
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
// .NAME vtkKWMimxImageProcessingMenuGroup
//
// .SECTION Description


#ifndef __vtkKWMimxImageProccessingGroup_h
#define __vtkKWMimxImageProcessingGroup_h


// MIMX Headers
#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxKWCommonWin32Header.h"


class vtkImageActor;
class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWFrameWithLabel;
class vtkKWMenuButton;
class vtkKWCheckButtonWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWExtent;
class vtkBoxWidget;
class vtkBWCropCallback;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxImageProcessingGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxImageProcessingGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxImageProcessingGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  void ThresholdBelowCallback();
  void ThresholdAboveCallback();
  void ThresholdOutsideCallback();
  void IsoResampleCallback();
  void ResampleCallback();
  void UserDefinedCallback();
  void CancelCallback();
  void ThresholdCallback();
  void MirrorCallback();
  void SmoothCallback();
  void CropCallback();
  void CropSliderCallback(double, double, double, double, double, double);
  void MathCallback();
  void AddCallback();
  void SubtractCallback();
  void FillCallback();
  void OutputCallback();
  void UpdateObjectLists();
  int SmoothApplyCallback();
  int ThresholdAboveApplyCallback();
  int ThresholdBelowApplyCallback();
  int ThresholdOutsideApplyCallback();
  int IsoResampleApplyCallback();
  int UserResampleApplyCallback();
  int MirrorApplyCallback();
  int CropApplyCallback();
  int FillApplyCallback();
  int AddApplyCallback();
  int SubtractApplyCallback();
  vtkSetObjectMacro(FEMeshList, vtkLinkedListWrapper);

protected:
  vtkKWMimxImageProcessingGroup();
  ~vtkKWMimxImageProcessingGroup();
  virtual void CreateWidget();
  vtkKWMenuButton *TypeMenuButton;
  vtkKWMenuButton *TypeMenuButton1;
  vtkKWMenuButton *TypeMenuButton2;
  vtkKWMenuButtonWithLabel *OutputMenuButton;
  vtkKWComboBoxWithLabel *ImageListComboBox;
  vtkKWEntryWithLabel *UpperLimitEntry;
  vtkKWEntryWithLabel *LowerLimitEntry;
  vtkKWEntryWithLabel *gaussianVarianceEntry;
  vtkKWMenuButtonWithLabel *OutputMenuButton2;
  vtkKWCheckButtonWithLabel *XFlipCheckButton;
  vtkKWCheckButtonWithLabel *YFlipCheckButton;
  vtkKWCheckButtonWithLabel *ZFlipCheckButton;
  vtkKWEntryWithLabel *IsotropicDimEntry;
  vtkKWEntryWithLabel *XFactorEntry;
  vtkKWEntryWithLabel *YFactorEntry;
  vtkKWEntryWithLabel *ZFactorEntry;
  vtkKWFrame *ThresholdFrame;
  vtkKWFrame *GaussianFrame;
  vtkKWFrame *MirrorFrame;
  vtkKWFrame *IsoResampleFrame;
  vtkKWFrame *UserResampleFrame;
  vtkKWFrame *ResampleFrame;
  vtkKWFrame *ThreshSubFrame1;
  vtkKWFrame *ThreshSubFrame2;
  vtkKWFrame *ThreshOutputFrame;
  vtkKWFrame *CropFrame;
  vtkKWExtent *CropExtents;
  vtkKWEntryWithLabel *CropOriginXEntry;
  vtkKWEntryWithLabel *CropOriginYEntry;
  vtkKWEntryWithLabel *CropOriginZEntry;
  vtkKWEntryWithLabel *CropSizeXEntry;
  vtkKWEntryWithLabel *CropSizeYEntry;
  vtkKWEntryWithLabel *CropSizeZEntry;
  vtkKWFrameWithLabel *MathFrame; 
  vtkKWFrame *MathSubFrame; 
  vtkKWMenuButton *TypeMenuButton3;
  vtkKWComboBoxWithLabel *ImageListComboBox1;
  vtkKWComboBoxWithLabel *ImageListComboBox2;
  vtkKWFrame *FillFrame;
  vtkKWEntryWithLabel *FillLabelEntry;
  vtkKWMenuButtonWithLabel *OutputMenuButton3;
  vtkKWFrameWithLabel *ListFrame;
  vtkBoxWidget *boxwidget;
  vtkBWCropCallback *myCallback;
  
private:
  vtkKWMimxImageProcessingGroup(const vtkKWMimxImageProcessingGroup&); // Not implemented
  void operator=(const vtkKWMimxImageProcessingGroup&); // Not implemented
};

#endif

