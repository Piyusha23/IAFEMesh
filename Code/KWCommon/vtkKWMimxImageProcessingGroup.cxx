/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageProcessingGroup.cxx,v $
Language:  C++
Date:      $Date: 2012/12/13 03:37:11 $
Version:   $Revision: 1.1 $

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

#include "vtkKWMimxImageProcessingGroup.h"

#include <itkFlipImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkLineConstIterator.h>
#include <itkLineIterator.h>
#include <itkAddImageFilter.h>
#include <itkSubtractImageFilter.h>

#include <vtkObjectFactory.h>
#include <vtkBoxWidget.h>
#include <vtkPolyData.h>
#include <vtkMath.h>

#include <vtkKWCheckButton.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWExtent.h>

#include "vtkKWMimxApplication.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxImageActor.h"

#include <ctime>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxImageProcessingGroup);
vtkCxxRevisionMacro(vtkKWMimxImageProcessingGroup, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
class vtkBWCropCallback : public vtkCommand
{
public:
  static vtkBWCropCallback *New()
  { return new vtkBWCropCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
	  vtkBoxWidget *boxwidget = reinterpret_cast<vtkBoxWidget*>(caller);
	  block = vtkPolyData::New();
	  boxwidget->GetPolyData(block);

	  double bounds[6] = {0,0,0,0,0,0};
    block->GetBounds(bounds);
	  extentWidget->SetExtent(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);

    block->Delete();
	  boxwidget->SetHandleSize(0.005);
  }

  vtkPolyData *block;
  vtkKWExtent *extentWidget;
};
//----------------------------------------------------------------------------
vtkKWMimxImageProcessingGroup::vtkKWMimxImageProcessingGroup()
{
	this->TypeMenuButton = NULL;
	this->TypeMenuButton1 = NULL;
	this->TypeMenuButton2 = NULL;
	this->UpperLimitEntry = NULL;
	this->LowerLimitEntry = NULL;
	this->gaussianVarianceEntry = NULL;
	this->XFlipCheckButton = NULL;
	this->YFlipCheckButton = NULL;
	this->ZFlipCheckButton = NULL;
	this->IsotropicDimEntry = NULL;
	this->XFactorEntry = NULL;
	this->YFactorEntry = NULL;
	this->ZFactorEntry = NULL;
	this->ThresholdFrame = NULL;
	this->GaussianFrame = NULL;
	this->OutputMenuButton2=NULL;
	this->MirrorFrame = NULL;
	this->ResampleFrame = NULL;
	this->IsoResampleFrame = NULL;
	this->UserResampleFrame = NULL;
	this->ThreshSubFrame1 = NULL;
	this->ThreshSubFrame2 = NULL;
	this->ThreshOutputFrame = NULL;
	this->OutputMenuButton = NULL;
	this->ListFrame = NULL;
	this->ImageListComboBox = NULL;
	this->CropFrame = NULL;
	this->CropExtents = NULL;
	this->CropOriginXEntry = NULL;
	this->CropOriginYEntry = NULL;
	this->CropOriginZEntry = NULL;
	this->CropSizeXEntry = NULL;
	this->CropSizeYEntry = NULL;
	this->CropSizeZEntry = NULL;
	this->MathFrame = NULL;
	this->TypeMenuButton3 = NULL;
	this->MathSubFrame = NULL;
	this->ImageListComboBox1 = NULL;
	this->ImageListComboBox2 = NULL;
	this->FillFrame = NULL;
	this->FillLabelEntry = NULL;
	this->OutputMenuButton3=NULL;
	this->boxwidget=NULL;
	this->myCallback = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxImageProcessingGroup::~vtkKWMimxImageProcessingGroup()
{
	this->TypeMenuButton->Delete();
	this->TypeMenuButton1->Delete();
	this->TypeMenuButton2->Delete();
	this->UpperLimitEntry->Delete();
	this->LowerLimitEntry->Delete();
	this->gaussianVarianceEntry->Delete();
	this->OutputMenuButton2->Delete();
	this->XFlipCheckButton->Delete();
	this->YFlipCheckButton->Delete();
	this->ZFlipCheckButton->Delete();
	this->IsotropicDimEntry->Delete();
	this->XFactorEntry->Delete();
	this->YFactorEntry->Delete();
	this->ZFactorEntry->Delete();
	this->ThresholdFrame->Delete();
	this->GaussianFrame->Delete();
	this->MirrorFrame->Delete();
	this->ResampleFrame->Delete();
	this->IsoResampleFrame->Delete();
	this->UserResampleFrame->Delete();
	this->ThreshSubFrame1->Delete();
	this->ThreshSubFrame2->Delete();
	this->OutputMenuButton->Delete();
	this->ThreshOutputFrame->Delete();
	this->ListFrame->Delete();
	this->ImageListComboBox->Delete();
	this->CropFrame->Delete();
	this->CropExtents->Delete();
	this->CropOriginXEntry->Delete();
	this->CropOriginYEntry->Delete();
	this->CropOriginZEntry->Delete();
	this->CropSizeXEntry->Delete();
	this->CropSizeYEntry->Delete();
	this->CropSizeZEntry->Delete();
	this->MathFrame->Delete();
	this->TypeMenuButton3->Delete();
	this->MathSubFrame->Delete();
	this->ImageListComboBox1->Delete();
	this->ImageListComboBox2->Delete();
	this->FillLabelEntry->Delete();
	this->OutputMenuButton2->Delete();
	this->boxwidget->Delete();
	this->myCallback->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::CreateWidget()
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
  this->ListFrame->SetLabelText("Image");
  this->ListFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
    this->ListFrame->GetWidgetName());

  if (!this->ImageListComboBox)
      this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  ImageListComboBox->SetParent(this->ListFrame->GetFrame());
  ImageListComboBox->Create();
  ImageListComboBox->SetLabelText("Image : ");
  ImageListComboBox->SetLabelWidth(15);
  ImageListComboBox->GetWidget()->ReadOnlyOn();
  ImageListComboBox->GetWidget()->SetBalloonHelpString(
	  "Select the image to filter");
 // this->UpdateObjectLists();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
    ImageListComboBox->GetWidgetName());

  //Select ITK Filter
  if (!this->TypeMenuButton)
	  this->TypeMenuButton = vtkKWMenuButton::New();
  this->TypeMenuButton->SetParent(this->MainFrame);
  this->TypeMenuButton->Create();
  this->TypeMenuButton->SetBorderWidth(2);
  this->TypeMenuButton->SetReliefToGroove();
  this->TypeMenuButton->SetPadX(2);
  this->TypeMenuButton->SetPadY(2);
  this->TypeMenuButton->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6",
	  this->TypeMenuButton->GetWidgetName());
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Resample",this, "ResampleCallback");
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Crop",this, "CropCallback");
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Mirror",this, "MirrorCallback");
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Threshold",this, "ThresholdCallback");
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Gaussian Smooth",this, "SmoothCallback");
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Image Math",this, "MathCallback");
  this->TypeMenuButton->GetMenu()->AddRadioButton(
	  "Island Removal & Fill",this, "FillCallback");
  this->TypeMenuButton->SetValue( "Resample" );

  //Threshold Frame
  if(!this->ThresholdFrame)
	  this->ThresholdFrame = vtkKWFrame::New();
  this->ThresholdFrame->SetParent( this->MainFrame );
  this->ThresholdFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	  this->ThresholdFrame->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  //Select Threshold type
  if (!this->TypeMenuButton1)
	  this->TypeMenuButton1 = vtkKWMenuButton::New();
  this->TypeMenuButton1->SetParent(this->ThresholdFrame);
  this->TypeMenuButton1->Create();
  this->TypeMenuButton1->SetBorderWidth(2);
  this->TypeMenuButton1->SetReliefToGroove();
  this->TypeMenuButton1->SetPadX(2);
  this->TypeMenuButton1->SetPadY(2);
  this->TypeMenuButton1->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6",
	  this->TypeMenuButton1->GetWidgetName());
  this->TypeMenuButton1->GetMenu()->AddRadioButton(
	  "Threshold Outside",this, "ThresholdOutsideCallback");
  this->TypeMenuButton1->GetMenu()->AddRadioButton(
	  "Threshold Above",this, "ThresholdAboveCallback");
  this->TypeMenuButton1->GetMenu()->AddRadioButton(
	  "Threshold Below",this, "ThresholdBelowCallback");
  this->TypeMenuButton1->SetValue( "Threshold Outside" );

  //Output Selection Frame
  if(!this->ThreshOutputFrame)
	  this->ThreshOutputFrame = vtkKWFrame::New();
  this->ThreshOutputFrame->SetParent( this->ThresholdFrame);
  this->ThreshOutputFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
	  this->ThreshOutputFrame->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

  if (!this->OutputMenuButton)
	this->OutputMenuButton = vtkKWMenuButtonWithLabel::New();
  this->OutputMenuButton->SetParent(this->ThreshOutputFrame);
  this->OutputMenuButton->Create();
  this->OutputMenuButton->SetLabelText("Output Type: ");
  this->OutputMenuButton->SetLabelWidth( 15 );
  this->OutputMenuButton->GetWidget()->SetBorderWidth(2);
  this->OutputMenuButton->GetWidget()->SetReliefToGroove();
  this->OutputMenuButton->GetWidget()->SetPadX(2);
  this->OutputMenuButton->GetWidget()->SetPadY(2);
  this->OutputMenuButton->GetWidget()->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 2 -fill x",
	this->OutputMenuButton->GetWidgetName());
  this->OutputMenuButton->GetWidget()->GetMenu()->AddRadioButton("Gray Scale",this, "OutputCallback");
  this->OutputMenuButton->GetWidget()->GetMenu()->AddRadioButton("Binary",this, "OutputCallback");
  this->OutputMenuButton->GetWidget()->SetValue( "Gray Scale" );

  //Threshold SubFrame
  if(!this->ThreshSubFrame1)
	  this->ThreshSubFrame1 = vtkKWFrame::New();
  this->ThreshSubFrame1->SetParent( this->ThresholdFrame);
  this->ThreshSubFrame1->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
	  this->ThreshSubFrame1->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

  //Threshold upper limit
  if (!this->UpperLimitEntry)
	  this->UpperLimitEntry = vtkKWEntryWithLabel::New();
  this->UpperLimitEntry->SetParent(this->ThreshSubFrame1);
  this->UpperLimitEntry->Create();
  this->UpperLimitEntry->SetLabelWidth(15);
  this->UpperLimitEntry->SetLabelText("Upper Limit : ");
  this->UpperLimitEntry->GetWidget()->SetValue("1000");
  this->UpperLimitEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
	  this->UpperLimitEntry->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

  //Threshold SubFrame2
  if(!this->ThreshSubFrame2)
	  this->ThreshSubFrame2 = vtkKWFrame::New();
  this->ThreshSubFrame2->SetParent( this->ThresholdFrame);
  this->ThreshSubFrame2->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
	  this->ThreshSubFrame2->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

  //Threshold lower limit
  if (!this->LowerLimitEntry)
	  this->LowerLimitEntry = vtkKWEntryWithLabel::New();
  this->LowerLimitEntry->SetParent(this->ThreshSubFrame2);
  this->LowerLimitEntry->Create();
  this->LowerLimitEntry->SetLabelWidth(15);
  this->LowerLimitEntry->SetLabelText("Lower Limit : ");
  this->LowerLimitEntry->GetWidget()->SetValue("0");
  this->LowerLimitEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
	  this->LowerLimitEntry->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

  //Gaussian Frame
  if(!this->GaussianFrame)
	  this->GaussianFrame = vtkKWFrame::New();
  this->GaussianFrame->SetParent( this->MainFrame );
  this->GaussianFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
	  this->GaussianFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

  if (!this->OutputMenuButton2)
	this->OutputMenuButton2 = vtkKWMenuButtonWithLabel::New();
  this->OutputMenuButton2->SetParent(this->GaussianFrame);
  this->OutputMenuButton2->Create();
  this->OutputMenuButton2->SetLabelText("Output Type: ");
  this->OutputMenuButton2->SetLabelWidth( 15 );
  this->OutputMenuButton2->GetWidget()->SetBorderWidth(2);
  this->OutputMenuButton2->GetWidget()->SetReliefToGroove();
  this->OutputMenuButton2->GetWidget()->SetPadX(2);
  this->OutputMenuButton2->GetWidget()->SetPadY(2);
  this->OutputMenuButton2->GetWidget()->SetWidth(25);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 2 -fill x",
	this->OutputMenuButton2->GetWidgetName());
  this->OutputMenuButton2->GetWidget()->GetMenu()->AddRadioButton("16-bit",this, "OutputCallback");
  this->OutputMenuButton2->GetWidget()->GetMenu()->AddRadioButton("8-bit",this, "OutputCallback");
  this->OutputMenuButton2->GetWidget()->SetValue( "16-bit" );

  if (!this->gaussianVarianceEntry)
	  this->gaussianVarianceEntry = vtkKWEntryWithLabel::New();
  this->gaussianVarianceEntry->SetParent(this->GaussianFrame);
  this->gaussianVarianceEntry->Create();
  this->gaussianVarianceEntry->SetLabelWidth(15);
  this->gaussianVarianceEntry->SetLabelText("Variance : ");
  this->gaussianVarianceEntry->GetWidget()->SetValue("2");
  this->gaussianVarianceEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
	  this->gaussianVarianceEntry->GetWidgetName());

  //Mirror Frame
  if(!this->MirrorFrame)
	  this->MirrorFrame = vtkKWFrame::New();
  this->MirrorFrame->SetParent( this->MainFrame );
  this->MirrorFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x -after %s",
	  this->MirrorFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

  if(!this->XFlipCheckButton)
	  this->XFlipCheckButton = vtkKWCheckButtonWithLabel::New();
  this->XFlipCheckButton->SetParent(this->MirrorFrame);
  this->XFlipCheckButton->Create();
  this->XFlipCheckButton->SetLabelText("Flip X Axis ");
//  this->XFlipCheckButton->GetWidget()->SetCommand(this, "FlipXCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -expand n -padx 2 -pady 2",
	  this->XFlipCheckButton->GetWidgetName());

  if(!this->YFlipCheckButton)
	  this->YFlipCheckButton = vtkKWCheckButtonWithLabel::New();
  this->YFlipCheckButton->SetParent(this->MirrorFrame);
  this->YFlipCheckButton->Create();
  this->YFlipCheckButton->SetLabelText("Flip Y Axis");
//  this->YFlipCheckButton->GetWidget()->SetCommand(this, "FlipYCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -expand n -padx 2 -pady 2",
	  this->YFlipCheckButton->GetWidgetName());

  if(!this->ZFlipCheckButton)
	  this->ZFlipCheckButton = vtkKWCheckButtonWithLabel::New();
  this->ZFlipCheckButton->SetParent(this->MirrorFrame);
  this->ZFlipCheckButton->Create();
  this->ZFlipCheckButton->SetLabelText("Flip Z Axis");
//  this->ZFlipCheckButton->GetWidget()->SetCommand(this, "FlipZCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -expand n -padx 2 -pady 2",
	  this->ZFlipCheckButton->GetWidgetName());

 //Resample Frame
  if(!this->ResampleFrame)
	  this->ResampleFrame = vtkKWFrame::New();
  this->ResampleFrame->SetParent( this->MainFrame );
  this->ResampleFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	  this->ResampleFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

  if (!this->TypeMenuButton2)
	  this->TypeMenuButton2 = vtkKWMenuButton::New();
  this->TypeMenuButton2->SetParent(this->ResampleFrame);
  this->TypeMenuButton2->Create();
  this->TypeMenuButton2->SetBorderWidth(2);
  this->TypeMenuButton2->SetReliefToGroove();
  this->TypeMenuButton2->SetPadX(2);
  this->TypeMenuButton2->SetPadY(2);
  this->TypeMenuButton2->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6",
	  this->TypeMenuButton2->GetWidgetName());
  this->TypeMenuButton2->GetMenu()->AddRadioButton(
	  "Isotropic",this, "IsoResampleCallback");
  this->TypeMenuButton2->GetMenu()->AddRadioButton(
	  "User-defined",this, "UserDefinedCallback");
  this->TypeMenuButton2->SetValue( "Isotropic" );

  //IsoResample Frame
  if(!this->IsoResampleFrame)
	  this->IsoResampleFrame = vtkKWFrame::New();
  this->IsoResampleFrame->SetParent( this->ResampleFrame );
  this->IsoResampleFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	  this->IsoResampleFrame->GetWidgetName(), this->TypeMenuButton2->GetWidgetName());

  if (!this->IsotropicDimEntry)
	  this->IsotropicDimEntry = vtkKWEntryWithLabel::New();
  this->IsotropicDimEntry->SetParent(this->IsoResampleFrame);
  this->IsotropicDimEntry->Create();
  this->IsotropicDimEntry->SetLabelWidth(30);
  this->IsotropicDimEntry->SetLabelText("Isotropic Dimension (mm) : ");
  this->IsotropicDimEntry->GetWidget()->SetValue("1");
  this->IsotropicDimEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	  this->IsotropicDimEntry->GetWidgetName(), this->TypeMenuButton2->GetWidgetName());

  //UserResample Frame
  if(!this->UserResampleFrame)
	  this->UserResampleFrame = vtkKWFrame::New();
  this->UserResampleFrame->SetParent( this->ResampleFrame );
  this->UserResampleFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	  this->UserResampleFrame->GetWidgetName(), this->TypeMenuButton2->GetWidgetName());

  if (!this->XFactorEntry)
	  this->XFactorEntry = vtkKWEntryWithLabel::New();
  this->XFactorEntry->SetParent(this->UserResampleFrame);
  this->XFactorEntry->Create();
  this->XFactorEntry->SetLabelWidth(30);
  this->XFactorEntry->SetLabelText("X Resample Factor : ");
  this->XFactorEntry->GetWidget()->SetValue("1");
  this->XFactorEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	  this->XFactorEntry->GetWidgetName(),this->TypeMenuButton2->GetWidgetName());

  if (!this->YFactorEntry)
	  this->YFactorEntry = vtkKWEntryWithLabel::New();
  this->YFactorEntry->SetParent(this->UserResampleFrame);
  this->YFactorEntry->Create();
  this->YFactorEntry->SetLabelWidth(30);
  this->YFactorEntry->SetLabelText("Y Resample Factor : ");
  this->YFactorEntry->GetWidget()->SetValue("1");
  this->YFactorEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	  this->YFactorEntry->GetWidgetName(),this->TypeMenuButton2->GetWidgetName());

  if (!this->ZFactorEntry)
	  this->ZFactorEntry = vtkKWEntryWithLabel::New();
  this->ZFactorEntry->SetParent(this->UserResampleFrame);
  this->ZFactorEntry->Create();
  this->ZFactorEntry->SetLabelWidth(30);
  this->ZFactorEntry->SetLabelText("Z Resample Factor : ");
  this->ZFactorEntry->GetWidget()->SetValue("1");
  this->ZFactorEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	  this->ZFactorEntry->GetWidgetName(),this->TypeMenuButton2->GetWidgetName());

 //Crop Frame
  if(!this->CropFrame)
	  this->CropFrame = vtkKWFrame::New();
  this->CropFrame->SetParent( this->MainFrame );
  this->CropFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	  this->CropFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

  //Slider Bars
  if(!this->CropExtents)
	  this->CropExtents = vtkKWExtent::New();
  this->CropExtents->SetParent(this->CropFrame);
  this->CropExtents->Create();
  this->CropExtents->SetBorderWidth(2);
  this->CropExtents->SetReliefToGroove();
  this->CropExtents->SetExtentRange(0,150,0,150,0,150);
  this->CropExtents->SetLabelPositionToLeft();
  this->CropExtents->SetEntry1PositionToLeft();
  this->CropExtents->SetEntry2PositionToRight();
  this->CropExtents->SetSliderSize(4);
  this->CropExtents->SetThickness(23);
  this->CropExtents->SetInternalThickness(0.7);
  this->CropExtents->SetRequestedLength(150);
  this->CropExtents->GetXRange()->SetAdjustResolution(1);
  this->CropExtents->SetEndCommand(this, "CropSliderCallback");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6",
	  this->CropExtents->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

  ////Crop Origin X
  //if (!this->CropOriginXEntry)
	 // this->CropOriginXEntry = vtkKWEntryWithLabel::New();
  //this->CropOriginXEntry->SetParent(this->CropFrame);
  //this->CropOriginXEntry->Create();
  //this->CropOriginXEntry->SetLabelWidth(30);
  //this->CropOriginXEntry->SetLabelText("X Start : ");
  //this->CropOriginXEntry->GetWidget()->SetValue("0");
  //this->CropOriginXEntry->GetWidget()->SetRestrictValueToInteger();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	 // this->CropOriginXEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  ////Crop Size X
  //if (!this->CropSizeXEntry)
	 // this->CropSizeXEntry = vtkKWEntryWithLabel::New();
  //this->CropSizeXEntry->SetParent(this->CropFrame);
  //this->CropSizeXEntry->Create();
  //this->CropSizeXEntry->SetLabelWidth(30);
  //this->CropSizeXEntry->SetLabelText("X End : ");
  //this->CropSizeXEntry->GetWidget()->SetValue("256");
  //this->CropSizeXEntry->GetWidget()->SetRestrictValueToInteger();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	 // this->CropSizeXEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  ////Crop Origin Y
  //if (!this->CropOriginYEntry)
	 // this->CropOriginYEntry = vtkKWEntryWithLabel::New();
  //this->CropOriginYEntry->SetParent(this->CropFrame);
  //this->CropOriginYEntry->Create();
  //this->CropOriginYEntry->SetLabelWidth(30);
  //this->CropOriginYEntry->SetLabelText("Y Start : ");
  //this->CropOriginYEntry->GetWidget()->SetValue("0");
  //this->CropOriginYEntry->GetWidget()->SetRestrictValueToInteger();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	 // this->CropOriginYEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  ////Crop Size Y
  //if (!this->CropSizeYEntry)
	 // this->CropSizeYEntry = vtkKWEntryWithLabel::New();
  //this->CropSizeYEntry->SetParent(this->CropFrame);
  //this->CropSizeYEntry->Create();
  //this->CropSizeYEntry->SetLabelWidth(30);
  //this->CropSizeYEntry->SetLabelText("Y End : ");
  //this->CropSizeYEntry->GetWidget()->SetValue("256");
  //this->CropSizeYEntry->GetWidget()->SetRestrictValueToInteger();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	 // this->CropSizeYEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  ////Crop Origin Z
  //if (!this->CropOriginZEntry)
	 // this->CropOriginZEntry = vtkKWEntryWithLabel::New();
  //this->CropOriginZEntry->SetParent(this->CropFrame);
  //this->CropOriginZEntry->Create();
  //this->CropOriginZEntry->SetLabelWidth(30);
  //this->CropOriginZEntry->SetLabelText("Z Start : ");
  //this->CropOriginZEntry->GetWidget()->SetValue("0");
  //this->CropOriginZEntry->GetWidget()->SetRestrictValueToInteger();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	 // this->CropOriginZEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  ////Crop Size Z
  //if (!this->CropSizeZEntry)
	 // this->CropSizeZEntry = vtkKWEntryWithLabel::New();
  //this->CropSizeZEntry->SetParent(this->CropFrame);
  //this->CropSizeZEntry->Create();
  //this->CropSizeZEntry->SetLabelWidth(30);
  //this->CropSizeZEntry->SetLabelText("Z End : ");
  //this->CropSizeZEntry->GetWidget()->SetValue("256");
  //this->CropSizeZEntry->GetWidget()->SetRestrictValueToInteger();
  //this->GetApplication()->Script(
	 // "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
	 // this->CropSizeZEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  //Math Frame
  if ( !this->MathFrame )
    this->MathFrame = vtkKWFrameWithLabel::New();
  this->MathFrame->SetParent( this->MainFrame );
  this->MathFrame->Create();
  this->MathFrame->SetLabelText("Images");
  this->MathFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
    this->MathFrame->GetWidgetName());

  if (!this->TypeMenuButton3)
	  this->TypeMenuButton3 = vtkKWMenuButton::New();
  this->TypeMenuButton3->SetParent(this->MathFrame);
  this->TypeMenuButton3->Create();
  this->TypeMenuButton3->SetBorderWidth(2);
  this->TypeMenuButton3->SetReliefToGroove();
  this->TypeMenuButton3->SetPadX(2);
  this->TypeMenuButton3->SetPadY(2);
  this->TypeMenuButton3->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 6",
	  this->TypeMenuButton3->GetWidgetName());
  this->TypeMenuButton3->GetMenu()->AddRadioButton(
	  "Add",this, "AddCallback");
  this->TypeMenuButton3->GetMenu()->AddRadioButton(
	  "Subtract",this, "SubtractCallback");
  this->TypeMenuButton3->SetValue( "Add" );

  //Image 1
  if (!this->ImageListComboBox1)
      this->ImageListComboBox1 = vtkKWComboBoxWithLabel::New();
  ImageListComboBox1->SetParent(this->MathFrame->GetFrame());
  ImageListComboBox1->Create();
  ImageListComboBox1->SetLabelText("Image 1 : ");
  ImageListComboBox1->SetLabelWidth(15);
  ImageListComboBox1->GetWidget()->ReadOnlyOn();
  ImageListComboBox1->GetWidget()->SetBalloonHelpString(
	  "Select the first image");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
    ImageListComboBox1->GetWidgetName());

  // VAM - Add Icon between the images
  
  //Image 2
  if (!this->ImageListComboBox2)
      this->ImageListComboBox2 = vtkKWComboBoxWithLabel::New();
  ImageListComboBox2->SetParent(this->MathFrame->GetFrame());
  ImageListComboBox2->Create();
  ImageListComboBox2->SetLabelText("Image 2 : ");
  ImageListComboBox2->SetLabelWidth(15);
  ImageListComboBox2->GetWidget()->ReadOnlyOn();
  ImageListComboBox2->GetWidget()->SetBalloonHelpString(
	  "Select the second image");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
    ImageListComboBox2->GetWidgetName());

  //Fill Frame
  if(!this->FillFrame)
	  this->FillFrame = vtkKWFrame::New();
  this->FillFrame->SetParent( this->MainFrame );
  this->FillFrame->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	  this->FillFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

  //Fill Label
  if (!this->FillLabelEntry)
	  this->FillLabelEntry = vtkKWEntryWithLabel::New();
  this->FillLabelEntry->SetParent(this->FillFrame);
  this->FillLabelEntry->Create();
  this->FillLabelEntry->SetLabelWidth(15);
  this->FillLabelEntry->SetLabelText("Label to Fill : ");
  this->FillLabelEntry->GetWidget()->SetValue("1");
  this->FillLabelEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
	  this->FillLabelEntry->GetWidgetName(),this->TypeMenuButton->GetWidgetName());

  //Fill Direction
  if (!this->OutputMenuButton3)
	this->OutputMenuButton3 = vtkKWMenuButtonWithLabel::New();
  this->OutputMenuButton3->SetParent(this->FillFrame);
  this->OutputMenuButton3->Create();
  this->OutputMenuButton3->SetLabelText("Fill Direction: ");
  this->OutputMenuButton3->SetLabelWidth( 15 );
  this->OutputMenuButton3->GetWidget()->SetBorderWidth(2);
  this->OutputMenuButton3->GetWidget()->SetReliefToGroove();
  this->OutputMenuButton3->GetWidget()->SetPadX(2);
  this->OutputMenuButton3->GetWidget()->SetPadY(2);
  this->OutputMenuButton3->GetWidget()->SetWidth(20);
  this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 2 -fill x",
	this->OutputMenuButton3->GetWidgetName());
  this->OutputMenuButton3->GetWidget()->GetMenu()->AddRadioButton("X",this, "OutputCallback");
  this->OutputMenuButton3->GetWidget()->GetMenu()->AddRadioButton("Y",this, "OutputCallback");
  this->OutputMenuButton3->GetWidget()->GetMenu()->AddRadioButton("Z",this, "OutputCallback");
  this->OutputMenuButton3->GetWidget()->SetValue( "X" );

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "IsoResampleApplyCallback");
  this->CancelButton->SetCommand(this, "CancelCallback");
  
  
  this->UpdateObjectLists();
  this->ResampleCallback();

}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::OutputCallback()
{
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::ThresholdAboveCallback()
{
	this->GetApplication()->Script("pack forget %s", this->ThreshOutputFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThreshSubFrame1->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThreshSubFrame2->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
		this->ThreshSubFrame1->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

	this->ApplyButton->SetCommand(this, "ThresholdAboveApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::ThresholdBelowCallback()
{
	this->GetApplication()->Script("pack forget %s", this->ThreshOutputFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThreshSubFrame1->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThreshSubFrame2->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
		this->ThreshSubFrame2->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

	this->ApplyButton->SetCommand(this, "ThresholdBelowApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::ThresholdOutsideCallback()
{
	this->GetApplication()->Script("pack forget %s", this->ThreshOutputFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThreshSubFrame1->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThreshSubFrame2->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
		this->ThreshOutputFrame->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
		this->ThreshSubFrame1->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
		this->ThreshSubFrame2->GetWidgetName(),this->TypeMenuButton1->GetWidgetName());

	this->ApplyButton->SetCommand(this, "ThresholdOutsideApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::IsoResampleCallback()
{
	this->GetApplication()->Script("pack forget %s", this->IsoResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->UserResampleFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->IsoResampleFrame->GetWidgetName(), this->TypeMenuButton2->GetWidgetName());

	this->ApplyButton->SetCommand(this, "IsoResampleApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::UserDefinedCallback()
{
	this->GetApplication()->Script("pack forget %s", this->IsoResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->UserResampleFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->UserResampleFrame->GetWidgetName(), this->TypeMenuButton2->GetWidgetName());

	this->ApplyButton->SetCommand(this, "UserResampleApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::ResampleCallback()
{
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ListFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MathFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ResampleFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->UserResampleFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ListFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	this->ApplyButton->SetCommand(this, "IsoResampleApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::SmoothCallback()
{
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ListFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MathFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->GaussianFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ListFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	this->ApplyButton->SetCommand(this, "SmoothApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::ThresholdCallback()
{
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ListFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MathFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ThresholdFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ListFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	this->ApplyButton->SetCommand(this, "ThresholdOutsideApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::MirrorCallback()
{
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ListFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MathFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->MirrorFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ListFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	this->ApplyButton->SetCommand(this, "MirrorApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::CropCallback()
{
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ListFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MathFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->CropFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ListFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return ;
	}

	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));

	boxwidget = vtkBoxWidget::New();
	boxwidget->KeyPressActivationOff();
	boxwidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	boxwidget->SetPlaceFactor(1.0);
	boxwidget->SetInput(imageActor->GetOrientedDataSet());
	boxwidget->PlaceWidget();
	boxwidget->SetEnabled(1);
	boxwidget->SetHandleSize(0.005);
	boxwidget->RotationEnabledOff();

	double bounds[6] = {0,0,0,0,0,0};
	imageActor->SetInteraction(0);
	imageActor->GetOrientedDataSet()->GetBounds(bounds);

  std::cout << "X Bounds : " << bounds[0] << " " << bounds[1] << std::endl;
  std::cout << "Y Bounds : " << bounds[2] << " " << bounds[3] << std::endl;
  std::cout << "Z Bounds : " << bounds[4] << " " << bounds[5] << std::endl;

	this->CropExtents->SetExtentRange(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
	this->CropExtents->SetExtent(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);

	vtkBWCropCallback *myCallback = vtkBWCropCallback::New();
	myCallback->extentWidget = this->CropExtents;
	boxwidget->AddObserver(vtkCommand::InteractionEvent,myCallback);

	this->ApplyButton->SetCommand(this, "CropApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::CropSliderCallback(double xlower, double xupper, double ylower, double yupper, double zlower, double zupper)
{
	double sliderbounds[6] = {0,0,0,0,0,0};
	this->CropExtents->GetExtent(sliderbounds);
	boxwidget->PlaceWidget(sliderbounds);
	boxwidget->SetHandleSize(0.005);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::MathCallback()
{
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ListFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MathFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->MathFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	this->ApplyButton->SetCommand(this, "AddApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::AddCallback()
{
	this->ApplyButton->SetCommand(this, "AddApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::SubtractCallback()
{
	this->ApplyButton->SetCommand(this, "SubtractApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::FillCallback()
{
	//vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
	this->GetApplication()->Script("pack forget %s", this->ThresholdFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->GaussianFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->MirrorFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->ResampleFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->CropFrame->GetWidgetName());
	this->GetApplication()->Script("pack forget %s", this->FillFrame->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->FillFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());
	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x -after %s",
	this->ListFrame->GetWidgetName(), this->TypeMenuButton->GetWidgetName());

	this->ApplyButton->SetCommand(this, "FillApplyCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::CancelCallback()
{
	this->UnpackGroup();
	this->MenuGroup->SetMenuButtonsEnabled(1);
	this->GetMimxMainWindow()->GetMainUserInterfacePanel()->SetEnabled(1);

	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(strcmp(this->ImageListComboBox->GetWidget()->GetValue(),""))
	{
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		imageActor->SetInteraction(1);
	}
	if(this->boxwidget!=NULL)
	{
		boxwidget->EnabledOff();
	}
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::UpdateObjectLists()
{
	this->UpdateImageComboBox(this->ImageListComboBox->GetWidget());
	this->UpdateImageComboBox(this->ImageListComboBox1->GetWidget());
	this->UpdateImageComboBox(this->ImageListComboBox2->GetWidget());
}
//----------------------------------------------------------------------------
void vtkKWMimxImageProcessingGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::SmoothApplyCallback()
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

	// Setup pixel type and input image type
	typedef itk::Image< signed short,  3 >   ImageType;
    //To save as an 16bit image
    typedef itk::Image< signed short,  3 >   OutputImageType1;
	//To save as an 8bit image
    typedef itk::Image< unsigned char,  3 >   OutputImageType2;
	//typedef  signed short  PixelType;
	const double gaussianVariance = this->gaussianVarianceEntry->GetWidget()->GetValueAsDouble();

	//Check values
	if(gaussianVariance <= 0)
	{callback->ErrorMessage("Variance must be greater than 0");
		return 0;}

	if(!strcmp(this->OutputMenuButton2->GetWidget()->GetValue(), "16-bit"))
	{
		typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;
		RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
		rescaler->SetOutputMinimum(0);
		rescaler->SetOutputMaximum(255);
		rescaler->SetInput(imageActor->GetITKImage());
		rescaler->Update();

		typedef itk::DiscreteGaussianImageFilter<ImageType,OutputImageType1> FilterType;
		FilterType::Pointer filter = FilterType::New();
		filter->SetInput( rescaler->GetOutput());
		filter->SetVariance(gaussianVariance);
		filter->Update();

		imageActor->DisplayActor(0);
		const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
		AddImageToDisplay(filter->GetOutput(), "Smooth-", foundationname, 1, NULL);

	}
	else if(!strcmp(this->OutputMenuButton2->GetWidget()->GetValue(), "8-bit"))
	{
		typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType2;
		RescaleFilterType2::Pointer rescaler2 = RescaleFilterType2::New();
		rescaler2->SetOutputMinimum(0);
		rescaler2->SetOutputMaximum(255);
		rescaler2->SetInput(imageActor->GetITKImage());
		rescaler2->Update();

		typedef itk::DiscreteGaussianImageFilter<ImageType,ImageType> FilterType2;
		FilterType2::Pointer filter2 = FilterType2::New();
		filter2->SetInput(rescaler2->GetOutput());
		filter2->SetVariance(gaussianVariance);
		filter2->Update();

		typedef itk::RescaleIntensityImageFilter<ImageType, OutputImageType2> RescaleFilterType3;
		RescaleFilterType3::Pointer rescaler3 = RescaleFilterType3::New();
		rescaler3->SetOutputMinimum(0);
		rescaler3->SetOutputMaximum(255);
		rescaler3->SetInput(filter2->GetOutput());
		rescaler3->Update();

		imageActor->DisplayActor(0);
		const char *foundationname2 = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
		this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
		AddImageToDisplay(rescaler2->GetOutput(), "Smooth-", foundationname2, 1, NULL);
	}

	this->GetMimxMainWindow()->SetStatusText("Smooth Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Smoothing Operation, Gaussian Variance: "<<this->gaussianVarianceEntry->GetWidget()->GetValueAsDouble()<< ", Output Type: "<<this->OutputMenuButton2->GetWidget()->GetValue()<<endl;
	outfile.close();

    return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::ThresholdAboveApplyCallback()
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

	// Setup pixel type and input image type
	typedef itk::Image< signed short,  3 >   ImageType;
	typedef  signed short  InputPixelType;
	const InputPixelType aboveThreshold = this->UpperLimitEntry->GetWidget()->GetValueAsInt();

	typedef itk::ThresholdImageFilter<ImageType> FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(imageActor->GetITKImage());
	filter->ThresholdAbove(aboveThreshold);
	filter->Update();

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(filter->GetOutput(), "ThreshA-", foundationname, 1, NULL);
	this->GetMimxMainWindow()->SetStatusText("Threshold Image");

	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Threshold Above Operation, Threshold Value: "<<this->UpperLimitEntry->GetWidget()->GetValueAsInt()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::ThresholdBelowApplyCallback()
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

	// Setup pixel type and input image type
	typedef itk::Image< signed short,  3 >   ImageType;
	typedef  signed short  InputPixelType;
	const InputPixelType belowThreshold = this->LowerLimitEntry->GetWidget()->GetValueAsInt();

	typedef itk::ThresholdImageFilter<ImageType> FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(imageActor->GetITKImage());
	filter->SetOutsideValue(-1000);
	filter->ThresholdBelow(belowThreshold);
	filter->Update();

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(filter->GetOutput(), "ThreshB-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Threshold Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Threshold Below Operation, Threshold Value: "<<this->LowerLimitEntry->GetWidget()->GetValueAsInt()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::ThresholdOutsideApplyCallback()
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

	typedef itk::Image< signed short,  3 >   ImageType;
    ImageType::Pointer threshimage = ImageType::New();

	if(!strcmp(this->OutputMenuButton->GetWidget()->GetValue(), "Gray Scale"))
	{
	  // Setup pixel type and input image type

	  typedef  signed short  InputPixelType;
	  const InputPixelType belowThreshold = this->LowerLimitEntry->GetWidget()->GetValueAsInt();
	  const InputPixelType aboveThreshold = this->UpperLimitEntry->GetWidget()->GetValueAsInt();

	  typedef itk::ThresholdImageFilter<ImageType> FilterType;
	  FilterType::Pointer filter = FilterType::New();
	  filter->SetInput(imageActor->GetITKImage());
	  filter->SetOutsideValue(-1000);
	  filter->ThresholdOutside(belowThreshold,aboveThreshold);
	  filter->Update();

      threshimage = filter->GetOutput();
	}
	else if(!strcmp(this->OutputMenuButton->GetWidget()->GetValue(), "Binary"))
	{
		// Setup pixel type and input image type
		typedef itk::Image< signed short,  3 >   ImageType;

		typedef  signed short  InputPixelType;
		const InputPixelType lowerThreshold = this->LowerLimitEntry->GetWidget()->GetValueAsInt();
		const InputPixelType upperThreshold = this->UpperLimitEntry->GetWidget()->GetValueAsInt();

		typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
		FilterType::Pointer filter = FilterType::New();
		filter->SetInput(imageActor->GetITKImage());
		filter->SetOutsideValue(0);
		filter->SetInsideValue(255);
		filter->SetLowerThreshold(lowerThreshold);
		filter->SetUpperThreshold(upperThreshold);
		filter->Update();

		typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;
		RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
		rescaler->SetOutputMinimum(0);
		rescaler->SetOutputMaximum(255);
		rescaler->SetInput(filter->GetOutput());
		rescaler->Update();

		threshimage = rescaler->GetOutput();
	}

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(threshimage, "ThreshO-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Threshold Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Threshold Between Operation, Lower Threshold Value: "<<this->LowerLimitEntry->GetWidget()->GetValueAsInt()<<", Upper Threshold Value: "<<this->UpperLimitEntry->GetWidget()->GetValueAsInt()<<", Output Type: "<<this->OutputMenuButton->GetWidget()->GetValue()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::IsoResampleApplyCallback()
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

	//Convert from anisotropic to isotropic image
	const ImageType::SpacingType& iso_inputSpacing = imageActor->GetITKImage()->GetSpacing();

	const double isoSpacing = this->IsotropicDimEntry->GetWidget()->GetValueAsDouble();

	//Check values
	if(isoSpacing <= 0)
	{callback->ErrorMessage("Isotropic dimension must be greater than 0");
		return 0;}

	//Resample image
	typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
	ResampleFilterType::Pointer iso_resampler = ResampleFilterType::New();

	typedef itk::IdentityTransform<double,3> TransformType;
	TransformType::Pointer isotransform = TransformType::New();
	isotransform->SetIdentity();
	iso_resampler->SetTransform(isotransform);

	typedef itk::LinearInterpolateImageFunction<ImageType,double> InterpolatorType;
	InterpolatorType::Pointer interpolator = InterpolatorType::New();
	iso_resampler->SetInterpolator(interpolator);

	ImageType::SpacingType iso_spacing;
	iso_spacing[0] = isoSpacing;
	iso_spacing[1] = isoSpacing;
	iso_spacing[2] = isoSpacing;

	iso_resampler->SetOutputSpacing(iso_spacing);
	iso_resampler->SetOutputOrigin(imageActor->GetITKImage()->GetOrigin());
	iso_resampler->SetOutputDirection(imageActor->GetITKImage()->GetDirection());

	ImageType::SizeType iso_inputSize = imageActor->GetITKImage()->GetLargestPossibleRegion().GetSize();

	typedef ImageType::SizeType::SizeValueType SizeValueType;

	const double dx = iso_inputSize[0] * iso_inputSpacing[0] / isoSpacing;
	const double dy = iso_inputSize[1] * iso_inputSpacing[1] / isoSpacing;
	const double dz = (iso_inputSize[2]-1) * iso_inputSpacing[2] / isoSpacing;

	ImageType::SizeType iso_size;

	iso_size[0]=static_cast<SizeValueType>(dx);
	iso_size[1]=static_cast<SizeValueType>(dy);
	iso_size[2]=static_cast<SizeValueType>(dz);

	iso_resampler->SetSize(iso_size);
	iso_resampler->SetInput(imageActor->GetITKImage());
	iso_resampler->Update();

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(iso_resampler->GetOutput(), "Isotropic-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Resample Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Isotropic Resample Image Operation, Isotropic Dimention: "<<this->IsotropicDimEntry->GetWidget()->GetValueAsDouble()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::UserResampleApplyCallback()
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

	//Downsample image
	const double factorX = this->XFactorEntry->GetWidget()->GetValueAsDouble();
	const double factorY = this->YFactorEntry->GetWidget()->GetValueAsDouble();
	const double factorZ = this->ZFactorEntry->GetWidget()->GetValueAsDouble();

	//Check values
	if(factorX <= 0)
	{callback->ErrorMessage("X factor must be greater than 0");
		return 0;}

	if(factorY <= 0)
	{callback->ErrorMessage("Y factor must be greater than 0");
		return 0;}

	if(factorZ <= 0)
	{callback->ErrorMessage("Z factor must be greater than 0");
		return 0;}

	//Resample image
    typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
	ResampleFilterType::Pointer resampler = ResampleFilterType::New();

	typedef itk::IdentityTransform<double,3> TransformType;
	TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
	resampler->SetTransform(transform);

	typedef itk::LinearInterpolateImageFunction<ImageType,double> InterpolatorType;
	InterpolatorType::Pointer interpolator = InterpolatorType::New();
  	resampler->SetInterpolator(interpolator);

	const ImageType::SpacingType& inputSpacing = imageActor->GetITKImage()->GetSpacing();
	ImageType::SpacingType spacing;
	spacing[0] = inputSpacing[0]*factorX;
	spacing[1] = inputSpacing[1]*factorY;
	spacing[2] = inputSpacing[2]*factorZ;

	resampler->SetOutputSpacing(spacing);
	resampler->SetOutputOrigin(imageActor->GetITKImage()->GetOrigin());
	resampler->SetOutputDirection(imageActor->GetITKImage()->GetDirection());

	ImageType::SizeType inputSize = imageActor->GetITKImage()->GetLargestPossibleRegion().GetSize();

	typedef ImageType::SizeType::SizeValueType SizeValueType;
	ImageType::SizeType size;

	size[0]=static_cast<SizeValueType>(inputSize[0] / factorX);
	size[1]=static_cast<SizeValueType>(inputSize[1] / factorY);
    size[2]=static_cast<SizeValueType>(inputSize[2] / factorZ);

    resampler->SetSize(size);
    resampler->SetInput(imageActor->GetITKImage());
    resampler->Update();

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(resampler->GetOutput(), "Resample-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Resample Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", User Defined Resample Image Operation, X Factor: "<<this->XFactorEntry->GetWidget()->GetValueAsDouble()<<", Y Factor: "<<this->YFactorEntry->GetWidget()->GetValueAsDouble()<<", Z Factor: "<<this->ZFactorEntry->GetWidget()->GetValueAsDouble()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::MirrorApplyCallback()
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

	// Setup pixel type and input image type
	typedef itk::Image< signed short,  3 >   ImageType;

	//Setup input to flip image filter
	typedef itk::FlipImageFilter<ImageType>  FilterType;
	FilterType::Pointer filter = FilterType::New();

	typedef FilterType::FlipAxesArrayType FlipAxesArrayType;
	FlipAxesArrayType flipArray;
	flipArray[0] = this->XFlipCheckButton->GetWidget()->GetSelectedState();
	flipArray[1] = this->YFlipCheckButton->GetWidget()->GetSelectedState();
	flipArray[2] = this->ZFlipCheckButton->GetWidget()->GetSelectedState();

	filter->SetFlipAxes( flipArray );
	filter->SetInput( imageActor->GetITKImage());

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(filter->GetOutput(), "Mirror-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Mirror Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Mirror Operation, X Flip: "<<this->XFlipCheckButton->GetWidget()->GetSelectedState()<<", Y Flip: "<<this->YFlipCheckButton->GetWidget()->GetSelectedState()<<", Z Flip: "<<this->ZFlipCheckButton->GetWidget()->GetSelectedState()<<endl;
	outfile.close();
	
	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::CropApplyCallback()
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

	//Get values from entry boxes
	double bounds[6] = {0,0,0,0,0,0};
	this->CropExtents->GetExtent(bounds);

	//Convert from IJK back to Lps
	double RASmin[3] = {bounds[0],bounds[2],bounds[4]};
	double RASmax[3] = {bounds[1],bounds[3],bounds[5]};
	double IJKmin[3] = {0,0,0};
	double IJKmax[3] = {0,0,0};

  //std::cout << "RAS Image Min - " << RASmin[0] << " " << RASmin[1] << " " << RASmin[2] << std::endl;
  //std::cout << "RAS Image Max - " << RASmax[0] << " " << RASmax[1] << " " << RASmax[2] << std::endl;

	imageActor->RasToIjkTransformPoint(RASmin, IJKmin);
	imageActor->RasToIjkTransformPoint(RASmax, IJKmax);

  //std::cout << "RAS IJK Image Min - " << IJKmin[0] << " " << IJKmin[1] << " " << IJKmin[2] << std::endl;
  //std::cout << "RAS IJK Image Max - " << IJKmax[0] << " " << IJKmax[1] << " " << IJKmax[2] << std::endl;

  ImageType::RegionType region = imageActor->GetITKImage()->GetLargestPossibleRegion();
  //ImageType::IndexType indexMinLoc;
  ImageType::RegionType::SizeType  size;
  ImageType::RegionType::IndexType inputStart;
  inputStart[0] = /*region.GetSize(0) - 1 -*/ vtkMath::Round(IJKmin[0]);
  inputStart[1] = /*region.GetSize(1) - 1 - */vtkMath::Round(IJKmin[1]);
  inputStart[2] = vtkMath::Round(IJKmin[2]);

  ImageType::IndexType inputEnd;
  inputEnd[0] = /*region.GetSize(0) - 1 - */vtkMath::Round(IJKmax[0]);
  inputEnd[1] = /*region.GetSize(1) - 1 - */vtkMath::Round(IJKmax[1]);
  inputEnd[2] = vtkMath::Round(IJKmax[2]);

  //std::cout << "Input Start - " << inputStart << std::endl;
  //std::cout << "Input End - " << inputEnd << std::endl;


  ImageType::RegionType extractRegion;

	for(int i =0; i<3; i++)
	{
		int tempIndex = 0;
		if (inputEnd[i] < inputStart[i])
		{
			tempIndex = inputStart[i];
			inputStart[i] = inputEnd[i];
			inputEnd[i] = tempIndex;
		}
	}

	size[0] = inputEnd[0] - inputStart[0] + 1;
	size[1] = inputEnd[1] - inputStart[1] + 1;
	size[2] = inputEnd[2] - inputStart[2] + 1;

  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator< ImageType >       IteratorType;

  ImageType::RegionType inputRegion;
  inputRegion.SetSize( size );
  inputRegion.SetIndex( inputStart );

  //Output image parameters
  ImageType::RegionType outputRegion;
  ImageType::RegionType::IndexType outputStart;
  ImageType::PointType outputOrigin;

  outputStart[0] = 0;
  outputStart[1] = 0;
  outputStart[2] = 0;

  outputRegion.SetSize( size );
  outputRegion.SetIndex( outputStart );

  ImageType::Pointer outputImage = ImageType::New();
  outputImage->SetSpacing( imageActor->GetITKImage()->GetSpacing() );
  outputImage->SetDirection( imageActor->GetITKImage()->GetDirection() );
  outputImage->SetRegions( outputRegion );
  imageActor->GetITKImage()->TransformIndexToPhysicalPoint(inputStart, outputOrigin);
  outputImage->SetOrigin( outputOrigin );
  outputImage->Allocate();

  //Create Iterators and perform copy
  ConstIteratorType inputIt(imageActor->GetITKImage(), inputRegion  );
  IteratorType      outputIt(outputImage, outputRegion );

  inputIt.GoToBegin();
  outputIt.GoToBegin();

  while( !inputIt.IsAtEnd() )
  {
    outputIt.Set(inputIt.Get());
    ++inputIt;
    ++outputIt;
  }

  //std::cout << "------------------------------------------------" << std::endl;
  //std::cout << "Output Image : " << outputImage << std::endl;



	imageActor->SetInteraction(1);
	imageActor->DisplayActor(0);
    boxwidget->EnabledOff();

	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(outputImage, "Cropped-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Cropped Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Cropping Operation, Bounds (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax): ("<<bounds[0]<<","<<bounds[1]<<","<<bounds[2]<<","<<bounds[3]<<","<<bounds[4]<<","<<bounds[5]<<")"<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::FillApplyCallback()
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

    typedef signed short PixelType;
    const int dimension=3;
    typedef itk::Image<PixelType, dimension> ImageType;

    /* Extract Label of Interest */
    typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> BinaryFilterType;
    BinaryFilterType::Pointer extractLabelFilter = BinaryFilterType::New();
    extractLabelFilter->SetInput(imageActor->GetITKImage());
    extractLabelFilter->SetLowerThreshold( this->FillLabelEntry->GetWidget()->GetValueAsInt());
    extractLabelFilter->SetUpperThreshold( this->FillLabelEntry->GetWidget()->GetValueAsInt());
    extractLabelFilter->SetInsideValue(1);
    extractLabelFilter->SetOutsideValue(0);
    extractLabelFilter->Update();

    std::cout << "Extract Label" << std::endl;

    typedef itk::ConnectedComponentImageFilter<ImageType,ImageType,ImageType> ConnectedFilterType;
    ConnectedFilterType::Pointer connectedFilter = ConnectedFilterType::New();
    connectedFilter->SetInput( extractLabelFilter->GetOutput() );
    connectedFilter->Update();

    std::cout << "Connected Component" << std::endl;

    typedef itk::RelabelComponentImageFilter<ImageType,ImageType> RelabelFilterType;
    RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
    relabelFilter->SetInput(connectedFilter->GetOutput());
    relabelFilter->Update();

    std::cout << "Relabel Image" << std::endl;

	BinaryFilterType::Pointer binaryFilter = BinaryFilterType::New();
	binaryFilter->SetInput(relabelFilter->GetOutput());
	binaryFilter->SetLowerThreshold(1);
	binaryFilter->SetUpperThreshold(1);
	binaryFilter->SetInsideValue(1);
	binaryFilter->SetOutsideValue(0);
	binaryFilter->Update();

	std::cout << "Binary Threshold" << std::endl;

	/* Scan line fill of the Binary Segmentation */
	ImageType::Pointer binaryImage = binaryFilter->GetOutput();
	ImageType::RegionType imageRegion = binaryImage->GetLargestPossibleRegion();
	ImageType::RegionType::SizeType imageSize = imageRegion.GetSize();

	ImageType::Pointer fillImage = ImageType::New();
	fillImage->SetRegions(imageRegion);
	fillImage->SetOrigin(binaryImage->GetOrigin());
	fillImage->SetDirection(binaryImage->GetDirection());
	fillImage->SetSpacing(binaryImage->GetSpacing());
	fillImage->Allocate();
	fillImage->FillBuffer( 0 );
	std::cout << fillImage << std::endl;

	int lineDir;
	int dir1;
	int dir2;

	int option = 0;

	if(!strcmp(this->OutputMenuButton3->GetWidget()->GetValue(), "X") || !strcmp(this->OutputMenuButton3->GetWidget()->GetValue(), "x"))
	{
		option = 0;
	}
	else if(!strcmp(this->OutputMenuButton3->GetWidget()->GetValue(), "Y") || !strcmp(this->OutputMenuButton3->GetWidget()->GetValue(), "y"))
	{
		option = 1;
	}
	else if(!strcmp(this->OutputMenuButton3->GetWidget()->GetValue(), "Z") || !strcmp(this->OutputMenuButton3->GetWidget()->GetValue(), "z"))
	{
		option = 2;
	}

	switch (option)
	{
	  case 1: /* X */
	    lineDir=0;
	    dir1=1;
	    dir2=2;
	    break;
	  case 2: /* Y */
		lineDir=1;
		dir1=0;
		dir2=2;
		break;
	  default: /* Z */
	    lineDir=2;
	    dir1=0;
	    dir2=1;
	    break;
	}

	for (unsigned int i=0;i<imageSize[dir1];i++)
	{
	  std::cout << "Fill Image: " << i << std::endl;
	  for (unsigned int j=0;j<imageSize[dir2];j++)
	  {
	    ImageType::IndexType index1;
	    ImageType::IndexType index2;
	    index1[lineDir] = 0;
	    index2[lineDir] = imageSize[lineDir]-1;
	    index1[dir1] = i;
	    index2[dir1] = i;
	    index1[dir2] = j;
	    index2[dir2] = j;
	    bool foundRegion = false;
	    ImageType::IndexType startIndex;
	    ImageType::IndexType endIndex;
	    PixelType previousPixel = 0;
	    itk::LineConstIterator< ImageType > it(binaryImage, index1, index2);
	    while (!it.IsAtEnd())
	    {
	      if ((it.Get() > 0) && (foundRegion == false))
		  {
		    foundRegion = true;
		    startIndex = it.GetIndex();
		  }
	      if ((it.Get() == 0) && (previousPixel == 1) && (foundRegion == true))
		  {
		    endIndex = it.GetIndex();
		  }
		   previousPixel = it.Get();
		   ++it;
		}

		if (foundRegion)
		{
		  itk::LineIterator< ImageType > fillIt(fillImage, startIndex, endIndex);
		  while (!fillIt.IsAtEnd())
		  {
		    fillIt.Set( 1 );
		    ++fillIt;
		  }
		}
	  }
	}

	imageActor->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(fillImage, "Fill-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Fill Image");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Input Image Filename: "<< this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName() << ", Image Fill and Island Removal Operation, Fill Direction: "<<this->OutputMenuButton3->GetWidget()->GetValue()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::AddApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox1->GetWidget()->GetValue(),"") || !strcmp(this->ImageListComboBox2->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox1 = this->ImageListComboBox1->GetWidget();
	vtkKWComboBox *combobox2 = this->ImageListComboBox2->GetWidget();
	const char *name1 = combobox1->GetValue();
	const char *name2 = combobox2->GetValue();

	vtkMimxImageActor *imageActor1 = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox1->GetValueIndex(name1)));
	vtkMimxImageActor *imageActor2 = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox2->GetValueIndex(name2)));

    typedef signed short PixelType;
    const int dimension=3;
    typedef itk::Image<PixelType, dimension> ImageType;

    /* Extract Label of Interest */
    typedef itk::AddImageFilter<ImageType,ImageType,ImageType> AddFilterType;
    AddFilterType::Pointer AddImage = AddFilterType::New();
    AddImage->SetInput1(imageActor1->GetITKImage());
	AddImage->SetInput2(imageActor2->GetITKImage());
    AddImage->Update();

	imageActor1->DisplayActor(0);
	imageActor2->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox1->GetValueIndex(name1))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(AddImage->GetOutput(), "Add-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Image Added");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Image Addition, Image 1: "<<this->ImageList->GetItem(combobox1->GetValueIndex(name1))->GetFileName()<<", Image 2: "<<this->ImageList->GetItem(combobox2->GetValueIndex(name2))->GetFileName()<<endl;
	outfile.close();

	return 1;
}
//---------------------------------------------------------------------------
int vtkKWMimxImageProcessingGroup::SubtractApplyCallback()
{
vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox1->GetWidget()->GetValue(),"") || !strcmp(this->ImageListComboBox2->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	//Get image from combobox
	vtkKWComboBox *combobox1 = this->ImageListComboBox1->GetWidget();
	vtkKWComboBox *combobox2 = this->ImageListComboBox2->GetWidget();
	const char *name1 = combobox1->GetValue();
	const char *name2 = combobox2->GetValue();

	vtkMimxImageActor *imageActor1 = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox1->GetValueIndex(name1)));
	vtkMimxImageActor *imageActor2 = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox2->GetValueIndex(name2)));

    typedef signed short PixelType;
    const int dimension=3;
    typedef itk::Image<PixelType, dimension> ImageType;

    /* Extract Label of Interest */
    typedef itk::SubtractImageFilter<ImageType,ImageType,ImageType> SubtractImageFilter;
    SubtractImageFilter::Pointer SubtractImage = SubtractImageFilter::New();
    SubtractImage->SetInput1(imageActor1->GetITKImage());
	SubtractImage->SetInput2(imageActor2->GetITKImage());
    SubtractImage->Update();

	imageActor1->DisplayActor(0);
	imageActor2->DisplayActor(0);
	const char *foundationname = this->ImageList->GetItem(combobox1->GetValueIndex(name1))->GetFileName();
	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
	AddImageToDisplay(SubtractImage->GetOutput(), "Subtract-", foundationname, 1, NULL);

	this->GetMimxMainWindow()->SetStatusText("Image Subtracted");
	this->UpdateObjectLists();

	//Get time information for log file
	time_t *rawtime = new time_t; 
	struct tm * timeinfo; 
	time(rawtime);
	timeinfo = localtime(rawtime);
	
	//Save history of image processing operations in text file
	ofstream outfile;
	outfile.open ("ImageProcessingLog.txt", ios::app);
	outfile << asctime(timeinfo);
	outfile << "Image Subtraction, Image 1: "<<this->ImageList->GetItem(combobox1->GetValueIndex(name1))->GetFileName()<<", Image 2: "<<this->ImageList->GetItem(combobox2->GetValueIndex(name2))->GetFileName()<<endl;
	outfile.close();

	return 1;
}
