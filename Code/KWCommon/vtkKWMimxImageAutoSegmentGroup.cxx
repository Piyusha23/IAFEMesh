/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageAutoSegmentGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/02/10 01:30:39 $
Version:   $Revision: 1.9 $

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

#include "vtkKWMimxImageAutoSegmentGroup.h"

// ITK Headers
#include <itkRescaleIntensityImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkConfidenceConnectedImageFilter.h>
#include <itkCastImageFilter.h>

// VTK Headers
#include <vtkObjectFactory.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkIdList.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellPicker.h>
#include <vtkMapper.h>
#include <vtkCellArray.h>
#include <vtkImagePlaneWidget.h>
#include <vtksys/SystemTools.hxx>

// KWWidget Headers
#include <vtkKWFrameWithLabel.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWRenderWidget.h>
#include <vtkKWSeparator.h>

// MIMX Headers
#include "vtkKWMimxApplication.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxImagePlaneWidget.h"
#include "vtkMimxLandmarkWidget.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "mimxCommonDefine.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxImageAutoSegmentGroup);
vtkCxxRevisionMacro(vtkKWMimxImageAutoSegmentGroup, "$Revision: 1.9 $");


//----------------------------------------------------------------------------
vtkKWMimxImageAutoSegmentGroup::vtkKWMimxImageAutoSegmentGroup()
{
	this->ListFrame = NULL;
  this->ImageListComboBox = NULL;
  this->SegmentationTypeMenu = NULL;
  this->ThresholdOptionFrame = NULL;
  this->ThresholdTypeMenu = NULL;
  this->RegionGrowingOptionFrame = NULL;
  this->RegionGrowingTypeMenu = NULL;
  this->ThresholdFrame = NULL;
  this->BinaryCheckButton = NULL;
  this->ThresholdUpperLimitEntry = NULL;
  this->ThresholdLowerLimitEntry = NULL;
  this->ThresholdInsideEntry = NULL;
  this->ThresholdOutsideEntry = NULL;
  this->ConnectedThresholdFrame = NULL;
  this->ConnectedThresholdUpperLimitEntry = NULL;
  this->ConnectedThresholdLowerLimitEntry = NULL;
  this->ConnectedThresholdSmoothCheckButton = NULL;
  this->ConnectedThresholdSmoothEntry = NULL;
  this->ConnectedThresholdIterationsEntry = NULL;
  this->ConnectedThresholdResultEntry = NULL;
  this->ConfidenceConnectedFrame = NULL;
  this->ConfidenceConnectedMultiplierEntry = NULL;
  this->ConfidenceConnectedSmoothCheckButton = NULL;
  this->ConfidenceConnectedSmoothEntry = NULL;
  this->ConfidenceConnectedIterationsEntry = NULL;
  this->ConfidenceConnectedSmoothIterationsEntry = NULL;
  this->ConfidenceConnectedResultEntry = NULL;
  this->LandmarkOptionFrame = NULL;
  this->LandmarkRadiusFrame = NULL;
  this->LandmarkRadiusEntry = NULL;
  this->LandmarkFrame = NULL;
  this->LandmarkLoadButton = NULL;
  this->LandmarkSaveButton = NULL;
  this->LandmarkClearButton = NULL;
  this->LandmarkWidget = vtkMimxLandmarkWidget::New();
  this->PlanePicker = vtkCellPicker::New();
}

//----------------------------------------------------------------------------
vtkKWMimxImageAutoSegmentGroup::~vtkKWMimxImageAutoSegmentGroup()
{
	if (this->ListFrame)
    this->ListFrame->Delete();
  if (this->ImageListComboBox)
    this->ImageListComboBox->Delete();
  if (this->SegmentationTypeMenu)
    this->SegmentationTypeMenu->Delete();
  if (this->ThresholdTypeMenu)
    this->ThresholdTypeMenu->Delete();
  if (this->RegionGrowingTypeMenu)
    this->RegionGrowingTypeMenu->Delete();
  if (this->ThresholdFrame)
    this->ThresholdFrame->Delete();
  if (this->BinaryCheckButton)
    this->BinaryCheckButton->Delete();
  if (this->ThresholdUpperLimitEntry)
    this->ThresholdUpperLimitEntry->Delete();
  if (this->ThresholdLowerLimitEntry)
    this->ThresholdLowerLimitEntry->Delete();
  if (this->ThresholdInsideEntry)
    this->ThresholdInsideEntry->Delete();
  if (this->ThresholdOutsideEntry)
    this->ThresholdOutsideEntry->Delete();
  if (this->ConnectedThresholdFrame)
    this->ConnectedThresholdFrame->Delete();
  if (this->ConnectedThresholdUpperLimitEntry)
    this->ConnectedThresholdUpperLimitEntry->Delete();
  if (this->ConnectedThresholdLowerLimitEntry)
    this->ConnectedThresholdLowerLimitEntry->Delete();
  if (this->ConnectedThresholdSmoothCheckButton)
    this->ConnectedThresholdSmoothCheckButton->Delete();
  if (this->ConnectedThresholdSmoothEntry)
    this->ConnectedThresholdSmoothEntry->Delete();
  if (this->ConnectedThresholdIterationsEntry)
    this->ConnectedThresholdIterationsEntry->Delete();
  if (this->ConnectedThresholdResultEntry)
    this->ConnectedThresholdResultEntry->Delete();
  if (this->ConfidenceConnectedFrame)
    this->ConfidenceConnectedFrame->Delete();
  if (this->ConfidenceConnectedMultiplierEntry)
    this->ConfidenceConnectedMultiplierEntry->Delete();
  if (this->ConfidenceConnectedSmoothCheckButton)
    this->ConfidenceConnectedSmoothCheckButton->Delete();
  if (this->ConfidenceConnectedSmoothEntry)
    this->ConfidenceConnectedSmoothEntry->Delete();
  if (this->ConfidenceConnectedIterationsEntry)
    this->ConfidenceConnectedIterationsEntry->Delete();
  if (this->ConfidenceConnectedResultEntry)
    this->ConfidenceConnectedResultEntry->Delete();
  if (this->ConfidenceConnectedSmoothIterationsEntry)
    this->ConfidenceConnectedSmoothIterationsEntry->Delete();
  if (this->LandmarkRadiusFrame)
    this->LandmarkRadiusFrame->Delete();
  if (this->LandmarkRadiusEntry)
    this->LandmarkRadiusEntry->Delete();
  if (this->LandmarkFrame)
    this->LandmarkFrame->Delete();
  if (this->LandmarkLoadButton)
    this->LandmarkLoadButton->Delete();
  if (this->LandmarkSaveButton)
    this->LandmarkSaveButton->Delete();
  if (this->LandmarkClearButton)
    this->LandmarkClearButton->Delete();
  if (this->LandmarkWidget)
    this->LandmarkWidget->Delete();
  if (this->PlanePicker)
    this->PlanePicker->Delete();

}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::CreateWidget()
{
  if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}

  this->Superclass::CreateWidget();

  //Select the image from the list
  if ( !this->ListFrame )
    this->ListFrame = vtkKWFrameWithLabel::New();
  this->ListFrame->SetParent( this->MainFrame );
  this->ListFrame->Create();
  this->ListFrame->SetLabelText("Image to Segment");
  this->ListFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ListFrame->GetWidgetName());
  
  if (!this->ImageListComboBox)	
    this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  this->ImageListComboBox->SetParent(this->ListFrame->GetFrame());
  this->ImageListComboBox->Create();
  this->ImageListComboBox->SetLabelText("Image : ");
  this->ImageListComboBox->SetLabelWidth(15);
  this->ImageListComboBox->GetWidget()->ReadOnlyOn();
  this->ImageListComboBox->GetWidget()->SetBalloonHelpString(
	  "Select the image to segment");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ImageListComboBox->GetWidgetName());

  //Segmentation Method Menu
  if (!this->SegmentationTypeMenu)	
	  this->SegmentationTypeMenu = vtkKWMenuButton::New();
  this->SegmentationTypeMenu->SetParent(this->MainFrame);
  this->SegmentationTypeMenu->Create();
  this->SegmentationTypeMenu->SetBorderWidth(2);
  this->SegmentationTypeMenu->SetReliefToGroove();
  this->SegmentationTypeMenu->SetPadX(2);
  this->SegmentationTypeMenu->SetPadY(2);
  this->SegmentationTypeMenu->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6", 
	  this->SegmentationTypeMenu->GetWidgetName());
  this->SegmentationTypeMenu->GetMenu()->AddRadioButton(
	  "Threshold",this, "ThresholdCallback");
  this->SegmentationTypeMenu->GetMenu()->AddRadioButton(
	  "Region Growing",this, "RegionGrowingCallback");
  this->SegmentationTypeMenu->SetValue( "Threshold" );
  
  
  // Frame To Hold Threshold Options
  if (!this->ThresholdOptionFrame)
	  this->ThresholdOptionFrame = vtkKWFrame::New();
  this->ThresholdOptionFrame->SetParent( this->MainFrame);
  this->ThresholdOptionFrame->Create();
  this->GetApplication()->Script(
      "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
      this->ThresholdOptionFrame->GetWidgetName(),this->SegmentationTypeMenu->GetWidgetName());
  
  // Select Threshold type
  if (!this->ThresholdTypeMenu)	
	  this->ThresholdTypeMenu = vtkKWMenuButton::New();
  this->ThresholdTypeMenu->SetParent(this->ThresholdOptionFrame);
  this->ThresholdTypeMenu->Create();
  this->ThresholdTypeMenu->SetBorderWidth(2);
  this->ThresholdTypeMenu->SetReliefToGroove();
  this->ThresholdTypeMenu->SetPadX(2);
  this->ThresholdTypeMenu->SetPadY(2);
  this->ThresholdTypeMenu->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6", 
	  this->ThresholdTypeMenu->GetWidgetName());
  this->ThresholdTypeMenu->GetMenu()->AddRadioButton(
	  "Threshold Outside",this, "ThresholdOutsideCallback");
  this->ThresholdTypeMenu->GetMenu()->AddRadioButton(
	  "Threshold Above",this, "ThresholdAboveCallback");
  this->ThresholdTypeMenu->GetMenu()->AddRadioButton(
	  "Threshold Below",this, "ThresholdBelowCallback");
  this->ThresholdTypeMenu->SetValue( "Threshold Outside" );

  
  // Threshold Frame - Holds Parameters for Operation
  if(!this->ThresholdFrame)
	  this->ThresholdFrame = vtkKWFrame::New();
  this->ThresholdFrame->SetParent( this->ThresholdOptionFrame );
  this->ThresholdFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
    this->ThresholdFrame->GetWidgetName(),this->ThresholdTypeMenu->GetWidgetName());

  // Binary Result 
  if (!this->BinaryCheckButton)	
	  this->BinaryCheckButton = vtkKWCheckButtonWithLabel::New();
  this->BinaryCheckButton->SetParent(this->ThresholdFrame);
  this->BinaryCheckButton->Create();
  this->BinaryCheckButton->SetLabelText("Binary Output : ");
  this->BinaryCheckButton->SetLabelWidth( 30 );
  //this->BinaryCheckButton->SetBalloonHelpString("Result will be a binary image if selected and gray scale otherwise.");
  this->BinaryCheckButton->GetWidget()->SetCommand(this, "BinaryResultCallback");
  this->GetApplication()->Script( "pack %s -side top -anchor nw -expand y -padx 2 -pady 2",
    this->BinaryCheckButton->GetWidgetName());

  // Threshold - Upper limit
  if (!this->ThresholdUpperLimitEntry)
	  this->ThresholdUpperLimitEntry = vtkKWEntryWithLabel::New();
  this->ThresholdUpperLimitEntry->SetParent(this->ThresholdFrame);
  this->ThresholdUpperLimitEntry->Create();
  this->ThresholdUpperLimitEntry->SetLabelWidth(30);
  this->ThresholdUpperLimitEntry->SetLabelText("Upper Threshold : ");
  this->ThresholdUpperLimitEntry->GetWidget()->SetRestrictValueToInteger();
  this->ThresholdUpperLimitEntry->GetWidget()->SetValue("1000");
  //this->ThresholdUpperLimitEntry->SetBalloonHelpString("Upper threshold value. Values above this will be replaced with the specified Outside value");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->ThresholdUpperLimitEntry->GetWidgetName());

  // Threshold - Lower limit
  if (!this->ThresholdLowerLimitEntry)
	  this->ThresholdLowerLimitEntry = vtkKWEntryWithLabel::New();
  this->ThresholdLowerLimitEntry->SetParent(this->ThresholdFrame);
  this->ThresholdLowerLimitEntry->Create();
  this->ThresholdLowerLimitEntry->SetLabelWidth(30);
  this->ThresholdLowerLimitEntry->SetLabelText("Lower Threshold : ");
  this->ThresholdLowerLimitEntry->GetWidget()->SetRestrictValueToInteger();
  this->ThresholdLowerLimitEntry->GetWidget()->SetValue("0");
  //this->ThresholdUpperLimitEntry->SetBalloonHelpString("Lower threshold value. Values less than this will be replaced with the specified Outside value");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->ThresholdLowerLimitEntry->GetWidgetName());
  
  if (!this->ThresholdOutsideEntry)
	  this->ThresholdOutsideEntry = vtkKWEntryWithLabel::New();
  this->ThresholdOutsideEntry->SetParent(this->ThresholdFrame);
  this->ThresholdOutsideEntry->Create();
  this->ThresholdOutsideEntry->SetLabelWidth(30);
  this->ThresholdOutsideEntry->SetLabelText("Outside Value : ");
  this->ThresholdOutsideEntry->GetWidget()->SetRestrictValueToInteger();
  this->ThresholdOutsideEntry->GetWidget()->SetValue("0");
  //this->ThresholdOutsideEntry->SetBalloonHelpString("Values to be used for voxels outside of the specified user threshold");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
    this->ThresholdOutsideEntry->GetWidgetName());
  
  if (!this->ThresholdInsideEntry)
	  this->ThresholdInsideEntry = vtkKWEntryWithLabel::New();
  this->ThresholdInsideEntry->SetParent(this->ThresholdFrame);
  this->ThresholdInsideEntry->Create();
  this->ThresholdInsideEntry->SetLabelWidth(30);
  this->ThresholdInsideEntry->SetLabelText("Inside Value : ");
  this->ThresholdInsideEntry->GetWidget()->SetRestrictValueToInteger();
  this->ThresholdInsideEntry->GetWidget()->SetValue("1");
  //this->ThresholdInsideEntry->SetBalloonHelpString("Values to be used for voxels within the specified user threshold");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
    this->ThresholdInsideEntry->GetWidgetName());
  
  // Frame To Hold Threshold Options
  if (!this->RegionGrowingOptionFrame)
	  this->RegionGrowingOptionFrame = vtkKWFrame::New();
  this->RegionGrowingOptionFrame->SetParent( this->MainFrame);
  this->RegionGrowingOptionFrame->Create();
  this->GetApplication()->Script(
     "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
     this->RegionGrowingOptionFrame->GetWidgetName(),this->SegmentationTypeMenu->GetWidgetName());
  
  // Select Region Growing type
  if (!this->RegionGrowingTypeMenu)	
	  this->RegionGrowingTypeMenu = vtkKWMenuButton::New();
  this->RegionGrowingTypeMenu->SetParent(this->RegionGrowingOptionFrame);
  this->RegionGrowingTypeMenu->Create();
  this->RegionGrowingTypeMenu->SetBorderWidth(2);
  this->RegionGrowingTypeMenu->SetReliefToGroove();
  this->RegionGrowingTypeMenu->SetPadX(2);
  this->RegionGrowingTypeMenu->SetPadY(2);
  this->RegionGrowingTypeMenu->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6", 
	  this->RegionGrowingTypeMenu->GetWidgetName());
  this->RegionGrowingTypeMenu->GetMenu()->AddRadioButton(
	  "Connected Threshold",this, "ConnectedThresholdCallback");
  this->RegionGrowingTypeMenu->GetMenu()->AddRadioButton(
	  "Confidence Connected",this, "ConfidenceConnectedCallback");
  this->RegionGrowingTypeMenu->SetValue( "Connected Threshold" );
  
  ///////////////////// Connected Threshold Frame /////////////////////
  if(!this->ConnectedThresholdFrame)
	  this->ConnectedThresholdFrame = vtkKWFrame::New();
  this->ConnectedThresholdFrame->SetParent( this->RegionGrowingOptionFrame);
  this->ConnectedThresholdFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s", 
	  this->ConnectedThresholdFrame->GetWidgetName(),this->RegionGrowingTypeMenu->GetWidgetName());

  // Connected Threshold - Upper limit
  if (!this->ConnectedThresholdUpperLimitEntry)
	  this->ConnectedThresholdUpperLimitEntry = vtkKWEntryWithLabel::New();
  this->ConnectedThresholdUpperLimitEntry->SetParent(this->ConnectedThresholdFrame);
  this->ConnectedThresholdUpperLimitEntry->Create();
  this->ConnectedThresholdUpperLimitEntry->SetLabelWidth(30);
  this->ConnectedThresholdUpperLimitEntry->SetLabelText("Upper Limit : ");
  this->ConnectedThresholdUpperLimitEntry->GetWidget()->SetValue("1000");
  this->ConnectedThresholdUpperLimitEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->ConnectedThresholdUpperLimitEntry->GetWidgetName());

  // Connected Threshold - Lower limit
  if (!this->ConnectedThresholdLowerLimitEntry)
	  this->ConnectedThresholdLowerLimitEntry = vtkKWEntryWithLabel::New();
  this->ConnectedThresholdLowerLimitEntry->SetParent(this->ConnectedThresholdFrame);
  this->ConnectedThresholdLowerLimitEntry->Create();
  this->ConnectedThresholdLowerLimitEntry->SetLabelWidth(30);
  this->ConnectedThresholdLowerLimitEntry->SetLabelText("Lower Limit : ");
  this->ConnectedThresholdLowerLimitEntry->GetWidget()->SetValue("0");
  this->ConnectedThresholdLowerLimitEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->ConnectedThresholdLowerLimitEntry->GetWidgetName());
  
  // Connected Threshold - Result Label
  if (!this->ConnectedThresholdResultEntry)
	  this->ConnectedThresholdResultEntry = vtkKWEntryWithLabel::New();
  this->ConnectedThresholdResultEntry->SetParent(this->ConnectedThresholdFrame);
  this->ConnectedThresholdResultEntry->Create();
  this->ConnectedThresholdResultEntry->SetLabelWidth(30);
  this->ConnectedThresholdResultEntry->SetLabelText("Result Label: ");
  this->ConnectedThresholdResultEntry->GetWidget()->SetValue("1");
  this->ConnectedThresholdResultEntry->GetWidget()->SetRestrictValueToInteger();
  this->ConnectedThresholdResultEntry->SetBalloonHelpString("The label of the resulting segmentation");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 this->ConnectedThresholdResultEntry->GetWidgetName());
  
  vtkKWSeparator *separator = vtkKWSeparator::New();
  separator->SetParent(this->ConnectedThresholdFrame);
  separator->Create();
  separator->SetOrientationToHorizontal();
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 separator->GetWidgetName());
  // Smoothing Check Button 
  if (!this->ConnectedThresholdSmoothCheckButton)	
	  this->ConnectedThresholdSmoothCheckButton = vtkKWCheckButtonWithLabel::New();
  this->ConnectedThresholdSmoothCheckButton->SetParent(this->ConnectedThresholdFrame);
  this->ConnectedThresholdSmoothCheckButton->Create();
  this->ConnectedThresholdSmoothCheckButton->SetLabelText("Smooth : ");
  this->ConnectedThresholdSmoothCheckButton->SetLabelWidth( 30 );
  this->ConnectedThresholdSmoothCheckButton->SetBalloonHelpString("Smooth before running connected threshold segmentation.");
  this->ConnectedThresholdSmoothCheckButton->GetWidget()->SetCommand(this, "ConnectedThresholdSmoothCallback");
  this->GetApplication()->Script( "pack %s -side top -anchor nw -expand y -padx 2 -pady 2",
                                 this->ConnectedThresholdSmoothCheckButton->GetWidgetName());
  
  // Connected Threshold - Smoothing Entry
  if (!this->ConnectedThresholdSmoothEntry)
	  this->ConnectedThresholdSmoothEntry = vtkKWEntryWithLabel::New();
  this->ConnectedThresholdSmoothEntry->SetParent(this->ConnectedThresholdFrame);
  this->ConnectedThresholdSmoothEntry->Create();
  this->ConnectedThresholdSmoothEntry->SetLabelWidth(30);
  this->ConnectedThresholdSmoothEntry->SetLabelText("Smoothing Time Step: ");
  this->ConnectedThresholdSmoothEntry->GetWidget()->SetValue("0.125");
  this->ConnectedThresholdSmoothEntry->GetWidget()->SetRestrictValueToDouble();
  this->ConnectedThresholdSmoothEntry->SetBalloonHelpString("Smoothing time step for curvature flow smoothing filter");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ConnectedThresholdSmoothEntry->GetWidgetName());
  this->ConnectedThresholdSmoothEntry->SetEnabled(0);
  
  // Connected Threshold - Smoothing Iterations Entry
  if (!this->ConnectedThresholdIterationsEntry)
	  this->ConnectedThresholdIterationsEntry = vtkKWEntryWithLabel::New();
  this->ConnectedThresholdIterationsEntry->SetParent(this->ConnectedThresholdFrame);
  this->ConnectedThresholdIterationsEntry->Create();
  this->ConnectedThresholdIterationsEntry->SetLabelWidth(30);
  this->ConnectedThresholdIterationsEntry->SetLabelText("Smoothing Iterations: ");
  this->ConnectedThresholdIterationsEntry->GetWidget()->SetValue("5");
  this->ConnectedThresholdIterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->ConnectedThresholdIterationsEntry->SetBalloonHelpString("Number of smoothing iterations for curvature flow smoothing filter");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 this->ConnectedThresholdIterationsEntry->GetWidgetName());
  this->ConnectedThresholdIterationsEntry->SetEnabled(0);
  
  
  ///////////////////// Confidence Connected Frame /////////////////////
  if(!this->ConfidenceConnectedFrame)
	  this->ConfidenceConnectedFrame = vtkKWFrame::New();
  this->ConfidenceConnectedFrame->SetParent( this->RegionGrowingOptionFrame);
  this->ConfidenceConnectedFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s", 
	  this->ConfidenceConnectedFrame->GetWidgetName(),this->RegionGrowingTypeMenu->GetWidgetName());

  //Confidence Connected Multiplier
  if (!this->ConfidenceConnectedMultiplierEntry)
	  this->ConfidenceConnectedMultiplierEntry = vtkKWEntryWithLabel::New();
  this->ConfidenceConnectedMultiplierEntry->SetParent(this->ConfidenceConnectedFrame);
  this->ConfidenceConnectedMultiplierEntry->Create();
  this->ConfidenceConnectedMultiplierEntry->SetLabelWidth(30);
  this->ConfidenceConnectedMultiplierEntry->SetLabelText("Multiplier : ");
  this->ConfidenceConnectedMultiplierEntry->GetWidget()->SetValue("1.5");
  this->ConfidenceConnectedMultiplierEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->ConfidenceConnectedMultiplierEntry->GetWidgetName());
  
  // Confidence Connected - Smoothing Iterations Entry
  if (!this->ConfidenceConnectedIterationsEntry)
	  this->ConfidenceConnectedIterationsEntry = vtkKWEntryWithLabel::New();
  this->ConfidenceConnectedIterationsEntry->SetParent(this->ConfidenceConnectedFrame);
  this->ConfidenceConnectedIterationsEntry->Create();
  this->ConfidenceConnectedIterationsEntry->SetLabelWidth(30);
  this->ConfidenceConnectedIterationsEntry->SetLabelText("Segmentation Iterations: ");
  this->ConfidenceConnectedIterationsEntry->GetWidget()->SetValue("5");
  this->ConfidenceConnectedIterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->ConfidenceConnectedIterationsEntry->SetBalloonHelpString("Number of iterations for segmentation");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 this->ConfidenceConnectedIterationsEntry->GetWidgetName());
  
  // Confidence Connected - Result Label
  if (!this->ConfidenceConnectedResultEntry)
	  this->ConfidenceConnectedResultEntry = vtkKWEntryWithLabel::New();
  this->ConfidenceConnectedResultEntry->SetParent(this->ConfidenceConnectedFrame);
  this->ConfidenceConnectedResultEntry->Create();
  this->ConfidenceConnectedResultEntry->SetLabelWidth(30);
  this->ConfidenceConnectedResultEntry->SetLabelText("Result Label: ");
  this->ConfidenceConnectedResultEntry->GetWidget()->SetValue("1");
  this->ConfidenceConnectedResultEntry->GetWidget()->SetRestrictValueToInteger();
  this->ConfidenceConnectedResultEntry->SetBalloonHelpString("The label of the resulting segmentation");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 this->ConfidenceConnectedResultEntry->GetWidgetName());
  
  separator = vtkKWSeparator::New();
  separator->SetParent(this->ConfidenceConnectedFrame);
  separator->Create();
  separator->SetOrientationToHorizontal();
  //separator->SetThickness();
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 separator->GetWidgetName());
  
  // Smoothing Check Button 
  if (!this->ConfidenceConnectedSmoothCheckButton)	
	  this->ConfidenceConnectedSmoothCheckButton = vtkKWCheckButtonWithLabel::New();
  this->ConfidenceConnectedSmoothCheckButton->SetParent(this->ConfidenceConnectedFrame);
  this->ConfidenceConnectedSmoothCheckButton->Create();
  this->ConfidenceConnectedSmoothCheckButton->SetLabelText("Smooth : ");
  this->ConfidenceConnectedSmoothCheckButton->SetLabelWidth( 30 );
  this->ConfidenceConnectedSmoothCheckButton->SetBalloonHelpString("Smooth before running confidence connected segmentation.");
  this->ConfidenceConnectedSmoothCheckButton->GetWidget()->SetCommand(this, "ConfidenceConnectedSmoothCallback");
  this->GetApplication()->Script( "pack %s -side top -anchor nw -expand y -padx 2 -pady 2",
                                 this->ConfidenceConnectedSmoothCheckButton->GetWidgetName());
  
  // Connected Threshold - Smoothing Entry
  if (!this->ConfidenceConnectedSmoothEntry)
	  this->ConfidenceConnectedSmoothEntry = vtkKWEntryWithLabel::New();
  this->ConfidenceConnectedSmoothEntry->SetParent(this->ConfidenceConnectedFrame);
  this->ConfidenceConnectedSmoothEntry->Create();
  this->ConfidenceConnectedSmoothEntry->SetLabelWidth(30);
  this->ConfidenceConnectedSmoothEntry->SetLabelText("Smoothing Time Step: ");
  this->ConfidenceConnectedSmoothEntry->GetWidget()->SetValue("0.125");
  this->ConfidenceConnectedSmoothEntry->GetWidget()->SetRestrictValueToDouble();
  this->ConfidenceConnectedSmoothEntry->SetBalloonHelpString("Smoothing time step for curvature flow smoothing filter");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 this->ConfidenceConnectedSmoothEntry->GetWidgetName());
  this->ConfidenceConnectedSmoothEntry->SetEnabled(0);
  
  // Connected Threshold - Smoothing Iterations Entry
  if (!this->ConfidenceConnectedSmoothIterationsEntry)
	  this->ConfidenceConnectedSmoothIterationsEntry = vtkKWEntryWithLabel::New();
  this->ConfidenceConnectedSmoothIterationsEntry->SetParent(this->ConfidenceConnectedFrame);
  this->ConfidenceConnectedSmoothIterationsEntry->Create();
  this->ConfidenceConnectedSmoothIterationsEntry->SetLabelWidth(30);
  this->ConfidenceConnectedSmoothIterationsEntry->SetLabelText("Smoothing Iterations: ");
  this->ConfidenceConnectedSmoothIterationsEntry->GetWidget()->SetValue("5");
  this->ConfidenceConnectedSmoothIterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->ConfidenceConnectedIterationsEntry->SetBalloonHelpString("Number of smoothing iterations for curvature flow smoothing filter");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
                                 this->ConfidenceConnectedSmoothIterationsEntry->GetWidgetName());
  this->ConfidenceConnectedSmoothIterationsEntry->SetEnabled(0);
  
  
  
  ///////////////////// Landmark Option Frame /////////////////////
  if (!this->LandmarkOptionFrame)
	  this->LandmarkOptionFrame = vtkKWFrame::New();
  this->LandmarkOptionFrame->SetParent( this->MainFrame);
  this->LandmarkOptionFrame->Create();
  this->GetApplication()->Script(
      "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
      this->RegionGrowingOptionFrame->GetWidgetName(),this->RegionGrowingOptionFrame->GetWidgetName());
  
  // Landmark Radius Frame
  if ( !this->LandmarkRadiusFrame )
    this->LandmarkRadiusFrame = vtkKWFrame::New();
  this->LandmarkRadiusFrame->SetParent( this->LandmarkOptionFrame );
  this->LandmarkRadiusFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->LandmarkRadiusFrame->GetWidgetName());

  // Landmark radius
  if (!this->LandmarkRadiusEntry )
	  this->LandmarkRadiusEntry = vtkKWEntryWithLabel::New();
  this->LandmarkRadiusEntry->SetParent(this->LandmarkRadiusFrame );
  this->LandmarkRadiusEntry->Create();
  this->LandmarkRadiusEntry->SetLabelWidth(30);
  this->LandmarkRadiusEntry->SetLabelText("Landmark Radius : ");
  this->LandmarkRadiusEntry->GetWidget()->SetCommand(this,"RadiusCallback");
  this->LandmarkRadiusEntry->GetWidget()->SetValue("1.5");
  this->LandmarkRadiusEntry->GetWidget()->SetRestrictValueToDouble();
  std::string helpString = "Landmarks can be picked in the viewer window by holding the <ctrl> ";
  helpString += "button and selecting with the left mouse button. <ctrl> and the right mouse button ";
  helpString += "will delete a landmark point. <ctrl> and the middle mouse button can be used ";
  helpString += "to move the landmarks.";
  this->LandmarkRadiusEntry->SetBalloonHelpString(helpString.c_str());
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
	  this->LandmarkRadiusEntry->GetWidgetName());

  
  // Region Growing - Landmark Operations
  if(!this->LandmarkFrame)
	  this->LandmarkFrame = vtkKWFrame::New();
  this->LandmarkFrame->SetParent( this->LandmarkOptionFrame );
  this->LandmarkFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
    this->LandmarkFrame->GetWidgetName());

  // Load Landmark Button
  if(!this->LandmarkLoadButton)
	  this->LandmarkLoadButton = vtkKWPushButton::New();
  this->LandmarkLoadButton->SetParent(this->LandmarkFrame);
  this->LandmarkLoadButton->Create();
  this->LandmarkLoadButton->SetText("Load Landmarks");
  this->LandmarkLoadButton->SetCommand(this, "LoadLandmarksCallback");
  this->LandmarkLoadButton->Script(
    "pack %s -side left -anchor nw -expand n -padx 5 -pady 6", 
    this->LandmarkLoadButton->GetWidgetName() );

  // Save Landmark Button
 if(!this->LandmarkSaveButton)
	  this->LandmarkSaveButton = vtkKWPushButton::New();
  this->LandmarkSaveButton->SetParent(this->LandmarkFrame);
  this->LandmarkSaveButton->Create();
  this->LandmarkSaveButton->SetText("Save Landmarks");
  this->LandmarkSaveButton->SetCommand(this, "SaveLandmarksCallback");
  this->LandmarkSaveButton->Script(
    "pack %s -side right -anchor ne -expand n -padx 5 -pady 6", 
    this->LandmarkSaveButton->GetWidgetName() );

  // Clear Landmark Button
  if(!this->LandmarkClearButton)
	  this->LandmarkClearButton = vtkKWPushButton::New();
  this->LandmarkClearButton->SetParent(this->LandmarkFrame);
  this->LandmarkClearButton->Create();
  this->LandmarkClearButton->SetText("Clear Landmarks");
  this->LandmarkClearButton->SetCommand(this, "ClearLandmarksCallback");
  this->LandmarkClearButton->Script(
    "pack %s -side top -anchor n -expand n -padx 5 -pady 6", 
    this->LandmarkClearButton->GetWidgetName() );

  
  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "ThresholdAboveApplyCallback");
  this->CancelButton->SetCommand(this, "CancelCallback");
  
  // Initialize the User Interface
  this->UpdateObjectLists();
  this->BinaryResultCallback(0);
  this->ThresholdCallback();

}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::Update()
{
  this->UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::OutputCallback()
{
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::ThresholdAboveCallback()
{
  
  this->GetApplication()->Script("pack forget %s", this->ThresholdUpperLimitEntry->GetWidgetName());
  this->ThresholdLowerLimitEntry->SetLabelText("Threshold : ");
  this->ThresholdLowerLimitEntry->SetBalloonHelpString("Threshold value. Values above this value will be replaced with the specified Outside value");
	this->ApplyButton->SetCommand(this, "ThresholdAboveApplyCallback");
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::ThresholdBelowCallback()
{
  this->GetApplication()->Script("pack forget %s", this->ThresholdUpperLimitEntry->GetWidgetName());
  this->ThresholdLowerLimitEntry->SetLabelText("Threshold : ");
  this->ThresholdLowerLimitEntry->SetBalloonHelpString("Threshold value. Values belows this will be replaced with the specified Outside value");
	this->ApplyButton->SetCommand(this, "ThresholdBelowApplyCallback");
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::ThresholdOutsideCallback()
{
  this->ThresholdLowerLimitEntry->SetLabelText("Lower Threshold : ");
  this->ThresholdLowerLimitEntry->SetBalloonHelpString("Lower threshold value. Values less than this will be replaced with the specified Outside value");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
    this->ThresholdUpperLimitEntry->GetWidgetName(), this->BinaryCheckButton->GetWidgetName());
  
  this->ApplyButton->SetCommand(this, "ThresholdOutsideApplyCallback");
  
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::ThresholdCallback()
{
  if ( this->LandmarkWidget->GetVisibility() )
	{
		this->LandmarkWidget->SetVisibility(0);
		this->LandmarkWidget->SetEnabled(0);
	}
  
  this->GetApplication()->Script("pack forget %s", this->ThresholdOptionFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->RegionGrowingOptionFrame->GetWidgetName());
  this->GetApplication()->Script("pack forget %s", this->LandmarkOptionFrame->GetWidgetName());
  
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s", 
	  this->ThresholdOptionFrame->GetWidgetName(), this->SegmentationTypeMenu->GetWidgetName());

	this->ThresholdTypeMenu->SetValue( "Threshold Outside" );
  this->ThresholdOutsideCallback();
  
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::RegionGrowingCallback()
{
  this->GetApplication()->Script("pack forget %s", this->ThresholdOptionFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->RegionGrowingOptionFrame->GetWidgetName());
  this->GetApplication()->Script("pack forget %s", this->LandmarkOptionFrame->GetWidgetName());
  
  this->GetApplication()->Script(
     "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
     this->RegionGrowingOptionFrame->GetWidgetName(), this->SegmentationTypeMenu->GetWidgetName());
  
  this->GetApplication()->Script(
     "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
     this->LandmarkOptionFrame->GetWidgetName(), this->RegionGrowingOptionFrame->GetWidgetName());
  
  this->RegionGrowingTypeMenu->SetValue( "Connected Threshold" );
  this->ConnectedThresholdCallback();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::ConnectedThresholdCallback()
{
	
  this->GetApplication()->Script("pack forget %s", this->ConfidenceConnectedFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s", 
		this->ConnectedThresholdFrame->GetWidgetName(),this->RegionGrowingTypeMenu->GetWidgetName());
  

	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	const char *name = combobox->GetValue();	
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
	
	//Ensure that an image is available.
	if (strcmp(this->ImageListComboBox->GetWidget()->GetValue(),"") != 0)
	{
		//Ensure that correct axis is applied for picking
		imageActor->SetInteraction(0);
		this->AxisCallback();
		this->RadiusCallback("0");
		this->LandmarkWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		this->LandmarkWidget->SetEnabled(1);
		this->LandmarkWidget->SetVisibility(1);
		imageActor->SetInteraction(1);
	}

	this->ApplyButton->SetCommand(this, "ConnectedThresholdApplyCallback");
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::ConfidenceConnectedCallback()
{
	this->GetApplication()->Script("pack forget %s", this->ConnectedThresholdFrame->GetWidgetName());
	this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
    this->ConfidenceConnectedFrame->GetWidgetName(),this->RegionGrowingTypeMenu->GetWidgetName());

	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	const char *name = combobox->GetValue();	
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
	
	//Ensure that an image is available.
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),"")!=0)
	{
		//Ensure that correct axis is applied for picking
		imageActor->SetInteraction(0);
		this->AxisCallback();
		this->RadiusCallback("0");
		this->LandmarkWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
		this->LandmarkWidget->SetEnabled(1);
		this->LandmarkWidget->SetVisibility(1);
		imageActor->SetInteraction(1);
	}

	this->ApplyButton->SetCommand(this, "ConfidenceConnectedApplyCallback");
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::AxisCallback()
{
	//Ensure that an image is selected
  
	if (strcmp(this->ImageListComboBox->GetWidget()->GetValue(),"") !=0 )
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();	
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));

		vtkMimxImagePlaneWidget *imageActorWidgetX = imageActor->GetXaxisImageActorWidget();
		vtkMimxImagePlaneWidget *imageActorWidgetY = imageActor->GetYaxisImageActorWidget();
		vtkMimxImagePlaneWidget *imageActorWidgetZ = imageActor->GetZaxisImageActorWidget();

		imageActorWidgetX->GetTexturePlaneActor()->PickableOn();
		imageActorWidgetY->GetTexturePlaneActor()->PickableOn();
		imageActorWidgetZ->GetTexturePlaneActor()->PickableOn();

		this->LandmarkWidget->SetActor(imageActorWidgetX->GetTexturePlaneActor());
		this->LandmarkWidget->SetActor2(imageActorWidgetY->GetTexturePlaneActor());
		this->LandmarkWidget->SetActor3(imageActorWidgetZ->GetTexturePlaneActor());

		this->PlanePicker->AddPickList(imageActorWidgetX->GetTexturePlaneActor());
		this->PlanePicker->AddPickList(imageActorWidgetY->GetTexturePlaneActor());
		this->PlanePicker->AddPickList(imageActorWidgetZ->GetTexturePlaneActor());
		this->PlanePicker->PickFromListOn();
		this->PlanePicker->SetTolerance(0.0005);
		this->LandmarkWidget->SetPicker(this->PlanePicker);
	}
  
}

//---------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::RadiusCallback(char *value)
{
	double radius = this->LandmarkRadiusEntry->GetWidget()->GetValueAsDouble();
  
  if ( radius != this->LandmarkWidget->GetSphereRadius() )
	{
		//Ensure new sphere radius for future additions
		this->LandmarkWidget->SetSphereRadius( radius );
	
		//Resize sphere radius for already placed spheres
		if(LandmarkWidget->ActorCollection->GetNumberOfItems() > 0)
		{
			vtkActorCollection *copyActors = vtkActorCollection::New();
			
			this->LandmarkWidget->ActorCollection->InitTraversal();
			//Loop through all spheres
			for (int i = 0; i < this->LandmarkWidget->ActorCollection->GetNumberOfItems(); i++)
			{
				copyActors->AddItem(this->LandmarkWidget->ActorCollection->GetNextActor());
			}

			LandmarkWidget->RemoveAllLandmarks();
			copyActors->InitTraversal();
			
			//Place Resized Spheres
			for (int i = 0; i < copyActors->GetNumberOfItems(); i++)
			{
				this->LandmarkWidget->AddLandmark(copyActors->GetNextActor()->GetCenter());
			}
		}
	}
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::BinaryResultCallback(int state)
{
  if (state)
    {
    this->GetApplication()->Script(
      "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
      this->ThresholdInsideEntry->GetWidgetName(),this->ThresholdOutsideEntry->GetWidgetName());
    }
  else
    {
    this->GetApplication()->Script("pack forget %s", this->ThresholdInsideEntry->GetWidgetName());
    }
  
  return 1;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ConnectedThresholdSmoothCallback(int state)
{
  if (state)
  {
    this->ConnectedThresholdSmoothEntry->SetEnabled(1);
    this->ConnectedThresholdIterationsEntry->SetEnabled(1);
  }
  else
  {
    this->ConnectedThresholdSmoothEntry->SetEnabled(0);
    this->ConnectedThresholdIterationsEntry->SetEnabled(0);
  }
  
  return 1;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ConfidenceConnectedSmoothCallback(int state)
{
  if (state)
  {
    this->ConfidenceConnectedSmoothEntry->SetEnabled(1);
    this->ConfidenceConnectedSmoothIterationsEntry->SetEnabled(1);
  }
  else
  {
    this->ConfidenceConnectedSmoothEntry->SetEnabled(0);
    this->ConfidenceConnectedSmoothIterationsEntry->SetEnabled(0);
  }
  
  return 1;
}


//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::CancelCallback()
{
  if ( this->LandmarkWidget->GetVisibility() )
	{
		this->LandmarkWidget->SetVisibility(0);
		this->LandmarkWidget->SetEnabled(0);
	}
  
  this->UnpackGroup();
	//this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName() );
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  
  // Reset the State - Fixes an issue with the Landmark Widget
  this->ThresholdCallback();
  this->SegmentationTypeMenu->SetValue( "Threshold" );
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::UpdateObjectLists()
{
	this->UpdateImageComboBox(this->ImageListComboBox->GetWidget());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageAutoSegmentGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "ImageListComboBox: " << this->ImageListComboBox << endl;
  os << indent << "SegmentationTypeMenu: " << this->SegmentationTypeMenu << endl;
  
  os << indent << "ThresholdOptionFrame: " << this->ThresholdOptionFrame << endl;
  os << indent << "ThresholdTypeMenu: " << this->ThresholdTypeMenu << endl;
  
  os << indent << "ThresholdUpperLimitEntry: " << this->ThresholdUpperLimitEntry << endl;
  os << indent << "ThresholdLowerLimitEntry: " << this->ThresholdLowerLimitEntry << endl;
  os << indent << "BinaryCheckButton: " << this->BinaryCheckButton << endl;
  
  os << indent << "RegionGrowingTypeMenu: " << this->RegionGrowingTypeMenu << endl;
  os << indent << "RegionGrowingOptionFrame: " << this->RegionGrowingOptionFrame << endl;
  
  os << indent << "ConnectedThresholdFrame: " << this->ConnectedThresholdFrame << endl;
  os << indent << "ConnectedThresholdUpperLimitEntry: " << this->ConnectedThresholdUpperLimitEntry << endl;
  os << indent << "ConnectedThresholdLowerLimitEntry: " << this->ConnectedThresholdLowerLimitEntry << endl;
  os << indent << "ConnectedThresholdSmoothEntry: " << this->ConnectedThresholdSmoothEntry << endl;
  os << indent << "ConnectedThresholdIterationsEntry: " << this->ConnectedThresholdIterationsEntry << endl;
  os << indent << "ConnectedThresholdResultEntry: " << this->ConnectedThresholdResultEntry << endl;
  
  os << indent << "ConfidenceConnectedFrame: " << this->ConfidenceConnectedFrame << endl;
  os << indent << "ConfidenceConnectedMultiplierEntry: " << this->ConfidenceConnectedMultiplierEntry << endl;
  os << indent << "ConfidenceConnectedSmoothCheckButton: " << this->ConfidenceConnectedSmoothCheckButton << endl;
  os << indent << "ConfidenceConnectedSmoothEntry: " << this->ConfidenceConnectedSmoothEntry << endl;
  os << indent << "ConfidenceConnectedSmoothIterationsEntry: " << this->ConfidenceConnectedSmoothIterationsEntry << endl;
  os << indent << "ConfidenceConnectedResultEntry: " << this->ConfidenceConnectedResultEntry << endl;
  
  os << indent << "LandmarkOptionFrame: " << this->LandmarkOptionFrame << endl;
  os << indent << "LandmarkRadiusFrame: " << this->LandmarkRadiusFrame << endl;
  os << indent << "LandmarkRadiusEntry: " << this->LandmarkRadiusEntry << endl;
  os << indent << "LandmarkFrame: " << this->LandmarkFrame << endl;
  os << indent << "LandmarkLoadButton: " << this->LandmarkLoadButton << endl;
  os << indent << "LandmarkSaveButton: " << this->LandmarkSaveButton << endl;
  os << indent << "LandmarkClearButton: " << this->LandmarkClearButton << endl;
  
  os << indent << "PlanePicker: " << this->PlanePicker << endl;
  os << indent << "LandmarkWidget: " << this->LandmarkWidget << endl;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ThresholdAboveApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
  std::string name ( combobox->GetValue() );

	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name.c_str())));

	// Get the User Specified Parameters
	const ImagePixelType aboveThreshold = static_cast<ImagePixelType> (this->ThresholdLowerLimitEntry->GetWidget()->GetValueAsInt());
  const ImagePixelType outsideValue = static_cast<ImagePixelType> (this->ThresholdOutsideEntry->GetWidget()->GetValueAsInt());
  const ImagePixelType insideValue = static_cast<ImagePixelType> (this->ThresholdInsideEntry->GetWidget()->GetValueAsInt());
  const int binary = this->BinaryCheckButton->GetWidget()->GetSelectedState();
  
  ImageType::Pointer resultImage;
	if (binary)
    {
    resultImage = imageActor->GetITKImage();
    typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
		FilterType::Pointer filter = FilterType::New();
		filter->SetInput(imageActor->GetITKImage());
		filter->SetOutsideValue(outsideValue);
		filter->SetInsideValue(insideValue);
		filter->SetUpperThreshold(aboveThreshold);
		try
      {
      filter->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      callback->ErrorMessage("Binary threshold failed! Check memory size.");
      return 0;
      }
    resultImage = filter->GetOutput();
    }
  else
    {
    typedef itk::ThresholdImageFilter<ImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(imageActor->GetITKImage());
    filter->ThresholdAbove(aboveThreshold);
    filter->SetOutsideValue(outsideValue);
    try
      {
      filter->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      callback->ErrorMessage("Image threshold failed! Check memory size.");
      return 0;
      }
    resultImage = filter->GetOutput();
    }
  
	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name.c_str()))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	this->AddImageToDisplay(resultImage, "Threshold-", foundationname, 1, NULL);
	this->GetMimxMainWindow()->SetStatusText("Thresholded Image");
	
	this->UpdateObjectLists();
	
	return 1;

}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ThresholdBelowApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
  std::string name = combobox->GetValue();

	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name.c_str())));

	// Get the User Specified Parameters
	const ImagePixelType belowThreshold = this->ThresholdLowerLimitEntry->GetWidget()->GetValueAsInt();
  const ImagePixelType outsideValue = this->ThresholdOutsideEntry->GetWidget()->GetValueAsInt();
  const ImagePixelType insideValue = this->ThresholdInsideEntry->GetWidget()->GetValueAsInt();
  const int binary = this->BinaryCheckButton->GetWidget()->GetSelectedState();
  
  ImageType::Pointer resultImage;
	if (binary)
    {
    typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
		FilterType::Pointer filter = FilterType::New();
		filter->SetInput(imageActor->GetITKImage());
		filter->SetOutsideValue(outsideValue);
		filter->SetInsideValue(insideValue);
		filter->SetLowerThreshold(belowThreshold);
		try
      {
      filter->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      callback->ErrorMessage("Binary threshold failed! Check memory size.");
      return 0;
      }
    resultImage = filter->GetOutput();
    }
  else
    {
    typedef itk::ThresholdImageFilter<ImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(imageActor->GetITKImage());
    filter->ThresholdBelow(belowThreshold);
    filter->SetOutsideValue(outsideValue);
    try
      {
      filter->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      callback->ErrorMessage("Image threshold failed! Check memory size.");
      return 0;
      }
    resultImage = filter->GetOutput();
    }
  
	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name.c_str()))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(resultImage, "Threshold-", foundationname, 1, NULL);
	
	this->GetMimxMainWindow()->SetStatusText("Threshold Image");
	this->UpdateObjectLists();
	
	return 1;
 
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ThresholdOutsideApplyCallback()
{

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
  std::string name = combobox->GetValue();
  
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name.c_str())));
  
  
	// Get the User Specified Parameters
	const ImagePixelType belowThreshold = static_cast<ImagePixelType> (this->ThresholdLowerLimitEntry->GetWidget()->GetValueAsInt());
  const ImagePixelType aboveThreshold = static_cast<ImagePixelType> (this->ThresholdUpperLimitEntry->GetWidget()->GetValueAsInt());
  const ImagePixelType outsideValue = static_cast<ImagePixelType> (this->ThresholdOutsideEntry->GetWidget()->GetValueAsInt());
  const ImagePixelType insideValue = static_cast<ImagePixelType> (this->ThresholdInsideEntry->GetWidget()->GetValueAsInt());
  const int binary = this->BinaryCheckButton->GetWidget()->GetSelectedState();
  
  ImageType::Pointer resultImage;
	if (binary)
    {
    typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
		FilterType::Pointer filter = FilterType::New();
		filter->SetInput( imageActor->GetITKImage() );
		filter->SetOutsideValue(outsideValue);
		filter->SetInsideValue(insideValue);
		filter->SetLowerThreshold(belowThreshold);
    filter->SetUpperThreshold(aboveThreshold);
    try
      {
      filter->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      callback->ErrorMessage("Binary threshold failed! Check memory size.");
      return 0;
      }
    resultImage = filter->GetOutput();
    }
  else
    {
    typedef itk::ThresholdImageFilter<ImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(imageActor->GetITKImage());
    filter->ThresholdOutside(belowThreshold,aboveThreshold);
    filter->SetOutsideValue(outsideValue);
    try
      {
      filter->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      callback->ErrorMessage("Image threshold failed! Check memory size.");
      return 0;
      }
    resultImage = filter->GetOutput();
    }
  
	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name.c_str()))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(resultImage, "Threshold-", foundationname, 1, NULL);
	
	this->GetMimxMainWindow()->SetStatusText("Threshold Image");
	this->UpdateObjectLists();
	
	return 1;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ConnectedThresholdApplyCallback()
{

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	const char *name = combobox->GetValue();

	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
	 
	//Define threshold limits
	const ImagePixelType lowerThreshold = this->ConnectedThresholdLowerLimitEntry->GetWidget()->GetValueAsInt();
	const ImagePixelType aboveThreshold = this->ConnectedThresholdUpperLimitEntry->GetWidget()->GetValueAsInt();
  const int smooth = this->ConnectedThresholdSmoothCheckButton->GetWidget()->GetSelectedState();
  const double timeStep = this->ConnectedThresholdSmoothEntry->GetWidget()->GetValueAsDouble();
  const int iterations = this->ConnectedThresholdIterationsEntry->GetWidget()->GetValueAsInt();
  const ImagePixelType labelValue = this->ConnectedThresholdResultEntry->GetWidget()->GetValueAsInt();
  
  ImageType::Pointer segmentImage;
  
  if ( smooth )
  {
	  // Cast image to float
    typedef itk::CastImageFilter<ImageType, FloatImageType> floatCastImageFilterType;
	  floatCastImageFilterType::Pointer floatImageCastFilter = floatCastImageFilterType::New();
	  floatImageCastFilter->SetInput(imageActor->GetITKImage());
	  floatImageCastFilter->Update();

    // Smooth the image
	  typedef itk::CurvatureFlowImageFilter<FloatImageType,FloatImageType> CurvatureFlowImageFilterType;
	  CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();
	  smoothing->SetInput(floatImageCastFilter->GetOutput());
	  smoothing->SetNumberOfIterations( iterations );
	  smoothing->SetTimeStep( timeStep );
  
    // Cast back to short
	  typedef itk::CastImageFilter<FloatImageType, ImageType> shortCastImageFilterType;
	  shortCastImageFilterType::Pointer shortImageCastFilter = shortCastImageFilterType::New();
	  shortImageCastFilter->SetInput(smoothing->GetOutput());
    
    segmentImage = shortImageCastFilter->GetOutput();
  }
  else 
  {
    segmentImage = imageActor->GetITKImage();
  }


	typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType> ConnectedFilterType;
	ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
	connectedThreshold->SetInput( segmentImage );
	connectedThreshold->SetLower(lowerThreshold);
	connectedThreshold->SetUpper(aboveThreshold);
	connectedThreshold->SetReplaceValue( labelValue );

	//Implement seed selection functionality here!!
	LandmarkWidget->ActorCollection->InitTraversal();
	double RASpoint[3] = {0,0,0};
	double ijkpoint[3] = {0,0,0};

	ImageType::IndexType  index;
  int numLandmarks = this->LandmarkWidget->ActorCollection->GetNumberOfItems();
	for( int i=0; i < numLandmarks; i++)
	{
		this->LandmarkWidget->GetActorCollection()->GetNextActor()->GetMapper()->GetCenter(RASpoint);
		imageActor->RasToIjkTransformPoint(RASpoint, ijkpoint);
		index [0] = ijkpoint[0]; 
		index [1] = ijkpoint[1];
		index [2] = ijkpoint[2];
		connectedThreshold->SetSeed(index);
	}

	try
  {
    connectedThreshold->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    callback->ErrorMessage("Connected threshold segmentation failed! Check memory size.");
    return 0;
  }

	imageActor->DisplayActor(0);
  
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(connectedThreshold->GetOutput(), "ConnectedThresh-", foundationname, 1, NULL);
	
	this->GetMimxMainWindow()->SetStatusText("Connected Threshold Image");
	this->UpdateObjectLists();
	LandmarkWidget->SetVisibility(1);

	//imageActor->SetInteraction(1);
	
	return 1;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ConfidenceConnectedApplyCallback()
{

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	const char *name = combobox->GetValue();

	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
	
	//Get Multiplier
	const ImagePixelType multiplier = this->ConfidenceConnectedMultiplierEntry->GetWidget()->GetValueAsDouble();
  const int smooth = this->ConfidenceConnectedSmoothCheckButton->GetWidget()->GetSelectedState();
  const double timeStep = this->ConfidenceConnectedSmoothEntry->GetWidget()->GetValueAsDouble();
  const int smoothIterations = this->ConfidenceConnectedSmoothIterationsEntry->GetWidget()->GetValueAsInt();
  const int segmentIterations = this->ConfidenceConnectedIterationsEntry->GetWidget()->GetValueAsInt();
  const ImagePixelType labelValue = this->ConfidenceConnectedResultEntry->GetWidget()->GetValueAsInt();
  
  ImageType::Pointer segmentImage;
  
  if ( smooth )
  {
    // Cast image to float
	  typedef itk::CastImageFilter<ImageType, FloatImageType> floatCastImageFilterType;
	  floatCastImageFilterType::Pointer floatImageFilter = floatCastImageFilterType::New();
	  floatImageFilter->SetInput(imageActor->GetITKImage());

    // Smooth image
	  typedef itk::CurvatureFlowImageFilter<FloatImageType,FloatImageType> CurvatureFlowImageFilterType;
	  CurvatureFlowImageFilterType::Pointer smoothingImageFilter = CurvatureFlowImageFilterType::New();
	  smoothingImageFilter->SetInput(floatImageFilter->GetOutput());
	  smoothingImageFilter->SetNumberOfIterations( smoothIterations );
	  smoothingImageFilter->SetTimeStep( timeStep );
	
    // Cast back to float
	  typedef itk::CastImageFilter<FloatImageType, ImageType> shortCastImageFilterType;
	  shortCastImageFilterType::Pointer shortImageCastFilter = shortCastImageFilterType::New();
	  shortImageCastFilter->SetInput(smoothingImageFilter->GetOutput());
	
    segmentImage = shortImageCastFilter->GetOutput();
  }
  else 
  {
    segmentImage = imageActor->GetITKImage();
  }
  
	typedef itk::ConfidenceConnectedImageFilter<ImageType,ImageType> ConnectedFilterType;
	ConnectedFilterType::Pointer confidenceConnectedFilter = ConnectedFilterType::New();
	confidenceConnectedFilter->SetInput( segmentImage );
	confidenceConnectedFilter->SetMultiplier(multiplier);
	confidenceConnectedFilter->SetNumberOfIterations( segmentIterations );
	confidenceConnectedFilter->SetReplaceValue(labelValue);
	
	//Implement seed selection functionality here!!
	LandmarkWidget->ActorCollection->InitTraversal();
	double RASpoint[3] = {0,0,0};
	double ijkpoint[3] = {0,0,0};

	ImageType::IndexType  index;
  int numLandmarks = LandmarkWidget->ActorCollection->GetNumberOfItems();
	for( int i=0; i < numLandmarks; i++)
	{
		this->LandmarkWidget->GetActorCollection()->GetNextActor()->GetMapper()->GetCenter(RASpoint);
		imageActor->RasToIjkTransformPoint(RASpoint, ijkpoint);
		index [0] = ijkpoint[0]; 
		index [1] = ijkpoint[1];
		index [2] = ijkpoint[2];
		confidenceConnectedFilter->SetSeed(index);
	}

	confidenceConnectedFilter->SetInitialNeighborhoodRadius(2);
	try
  {
    confidenceConnectedFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    callback->ErrorMessage("Confidence connected segmentation failed! Check memory size.");
    return 0;
  }
	
	
	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(confidenceConnectedFilter->GetOutput(), "ConfidenceConnected-", foundationname, 1, NULL);
	
	this->GetMimxMainWindow()->SetStatusText("Confidence Connected Image");
	this->UpdateObjectLists();
	LandmarkWidget->SetVisibility(1);
	//imageActor->SetInteraction(1);
	
	return 1;

}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::LoadLandmarksCallback()
{

	vtkKWLoadSaveDialog *fileBrowserDialog = vtkKWLoadSaveDialog::New();
  fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
  fileBrowserDialog->SetFileTypes("{{VTK files} {.vtk}} {{VTK XML files} {.vtp}} {{All files} {*.*}}");
  fileBrowserDialog->SetDefaultExtension(".vtk");
  fileBrowserDialog->SetMultipleSelection(1);
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();

	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		int numSelections = fileBrowserDialog->GetNumberOfFileNames();
		for (int i=0; i<numSelections; i++)
		{
      vtkPolyData *landmarks;
    
      vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
			callback->SetState(0);

			std::string fileName = vtksys::SystemTools::CollapseFullPath( fileBrowserDialog->GetNthFileName(i) );
			std::string extension = vtksys::SystemTools::LowerCase( vtksys::SystemTools::GetFilenameLastExtension( fileName ) );
			this->GetApplication()->SetRegistryValue(1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( fileName ).c_str());
			fileBrowserDialog->SaveLastPathToRegistry("LastPath");
    
			this->GetMimxMainWindow()->SetStatusText(fileName.c_str());
      
      if (extension == ".vtk")
        {
        vtkPolyDataReader *vtkReader = vtkPolyDataReader::New();
			  vtkReader->SetFileName( fileName.c_str() );
        vtkReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
        vtkReader->Update();
        landmarks = vtkReader->GetOutput();
        //Should Delete the Reader
        }
      else
        {
        vtkXMLPolyDataReader *vtkXMLReader = vtkXMLPolyDataReader::New();
			  vtkXMLReader->SetFileName( fileName.c_str() );
        vtkXMLReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
        vtkXMLReader->Update();
        landmarks = vtkXMLReader->GetOutput();
        //Should Delete the Reader
        }
    
			vtkPoints *verts = landmarks->GetPoints();
			for(int j = 0; j < landmarks->GetNumberOfPoints(); j++)
			{
				double point[3] = {0,0,0};
				verts->GetPoint(j,point);
				this->LandmarkWidget->AddLandmark(point);
			}
				    
			if (!callback->GetState())
			{
				this->GetMimxMainWindow()->SetStatusText("Loaded Landmarks");
			}
		}
	}

	return 1;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::SaveLandmarksCallback()
{
	vtkActorCollection *actorCollection = this->LandmarkWidget->GetActorCollection();

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if (this->LandmarkWidget->GetNumberOfLandmarks() == 0)
	{
    callback->ErrorMessage("No Landmarks Selected");;
		return 0;
	}
  
  vtkKWLoadSaveDialog *fileBrowserDialog = vtkKWLoadSaveDialog::New() ;
	fileBrowserDialog->SaveDialogOn();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
	fileBrowserDialog->RetrieveLastPathFromRegistry("LandmarkDataPath");
	fileBrowserDialog->SetTitle ("Save Landmarks");
	fileBrowserDialog->SetFileTypes ("{{VTK files} {.vtk}} {{VTK XML files} {.vtp}} {{All files} {*.*}}");
	fileBrowserDialog->SetDefaultExtension (".vtk");
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();
  
	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
  {
	  vtkPoints *centers = vtkPoints::New();
	  vtkIdList *ids = vtkIdList::New();
    vtkCellArray *cells = vtkCellArray::New();
	  actorCollection->InitTraversal();
	  double centroid[3] = {0,0,0};

	  //Put all landmarks into a list of points
    int numLandmarks = LandmarkWidget->GetNumberOfLandmarks();
	  for( vtkIdType i = 0; i<numLandmarks; i++)
	  {
		  this->LandmarkWidget->GetLandmark(actorCollection->GetNextActor(), centroid);
		  centers->InsertNextPoint(centroid);
      cells->InsertNextCell(1,&i);
		  ids->InsertId(i,i);
	  }
  
	  vtkPolyData *Vertices = vtkPolyData::New();
	  Vertices->Allocate(1000,1000);
	  Vertices->SetPoints(centers);
	  Vertices->SetVerts(cells);
	  Vertices->Update();
  
    const char *filename = fileBrowserDialog->GetFileName();
    if (filename)
    {
      std::string extension = vtksys::SystemTools::LowerCase( vtksys::SystemTools::GetFilenameLastExtension( filename ) );
      this->GetApplication()->SetRegistryValue(1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( filename ).c_str());
      fileBrowserDialog->SaveLastPathToRegistry("LastPath");
    
      if (extension == ".vtk")
        {
        vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
        writer->SetFileName( filename );
        writer->SetInput(Vertices);
        writer->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
        writer->Update();
        writer->RemoveObserver(callback);
        if (callback->GetState())
			  {
          writer->Delete();
          Vertices->Delete();
          cells->Delete();
          ids->Delete();
          centers->Delete();
          return 0;
			  }
        writer->Delete();
        }
      else
        {
        vtkXMLPolyDataWriter *writer = vtkXMLPolyDataWriter::New();
        writer->SetFileName( filename );
        writer->SetInput(Vertices);
        writer->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
        writer->Update();
        writer->RemoveObserver(callback);
        if (callback->GetState())
          {
          writer->Delete();
          Vertices->Delete();
          cells->Delete();
          ids->Delete();
          centers->Delete();
          return 0;
          }
        writer->Delete();
        }
    }
    Vertices->Delete();
    cells->Delete();
    ids->Delete();
    centers->Delete();
    this->GetMimxMainWindow()->SetStatusText("Saved Landmarks");
  }
  
  
  
  return 1;
}

//---------------------------------------------------------------------------
int vtkKWMimxImageAutoSegmentGroup::ClearLandmarksCallback()
{
	this->LandmarkWidget->RemoveAllLandmarks();
	return 0;
}

