/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/03/15 03:12:51 $
Version:   $Revision: 1.14 $

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

#include "vtkKWMimxImageMenuGroup.h"


//VTK Headers
#include "vtkActor.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkSTLReader.h"
#include "vtkExecutive.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkKWMessageDialog.h"
#include "vtkMimxErrorCallback.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkPolyData.h"
#include "vtkContourRepresentation.h"
#include "vtkFocalPlaneContourRepresentation.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkContourWidget.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkImageShiftScale.h"
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//KWWidget Headers
#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrameWithScrollbar.h"
#include "vtkKWIcon.h"
#include "vtkKWInternationalization.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWComboBoxWithLabel.h"

// MIMX Headers
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxImageViewProperties.h"
#include "vtkMimxActorBase.h"
#include "vtkKWMimxDeleteObjectGroup.h"
#include "vtkMimxImageActor.h"
#include "vtkKWMimxLoadImageGroup.h"
#include "vtkKWMimxLoadFeatureGroup.h"
#include "vtkKWMimxSaveImageGroup.h"
#include "vtkKWMimxImageThresholdGroup.h"
#include "vtkKWMimxImageAutoSegmentGroup.h"
#include "vtkKWMimxImageManualSegmentGroup.h"
#include "vtkKWMimxCropImageGroup.h"
#include "vtkKWMimxSaveFeatureGroup.h"
#include "vtkKWMimxCreateMaskFromFeatureGroup.h"
#include "vtkKWMimxCreateFeatureFromMaskGroup.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxImageMenuGroup);
vtkCxxRevisionMacro(vtkKWMimxImageMenuGroup, "$Revision: 1.14 $");

//----------------------------------------------------------------------------
vtkKWMimxImageMenuGroup::vtkKWMimxImageMenuGroup()
{
	this->ObjectMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton = NULL;
	this->TypeMenuButton = NULL;
	this->ImageList = vtkLinkedListWrapper::New();
  this->FileBrowserDialog = NULL;
  this->DeleteObjectGroup = NULL;
  this->SaveImageGroup = NULL;
  this->ThresholdImageGroup = NULL;
  this->AutomatedSegmentImageGroup = NULL;
  this->ManualSegmentImageGroup = NULL;
  this->CropImageGroup = NULL;
  this->LoadImageGroup = vtkKWMimxLoadImageGroup::New();
  this->SaveFeatureGroup = NULL;
  this->FeatureToMaskGroup = NULL;
  this->MaskToFeatureGroup = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxImageMenuGroup::~vtkKWMimxImageMenuGroup()
{
	this->ObjectMenuButton->Delete();
	this->ImageList->Delete();
  if(this->OperationMenuButton)
    this->OperationMenuButton->Delete();
  if(this->TypeMenuButton)
    this->TypeMenuButton->Delete();
  if(this->DeleteObjectGroup)
    this->DeleteObjectGroup->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
  if(this->SaveImageGroup)
	  this->SaveImageGroup->Delete();
  if(this->ThresholdImageGroup)
	  this->ThresholdImageGroup->Delete();
  if(this->LoadImageGroup)
	  this->LoadImageGroup->Delete();
  if(this->AutomatedSegmentImageGroup)
	  this->AutomatedSegmentImageGroup->Delete();
  if(this->ManualSegmentImageGroup)
	  this->ManualSegmentImageGroup->Delete();
  if(this->CropImageGroup)
	  this->CropImageGroup->Delete();
  if(this->SaveFeatureGroup)
	  this->SaveFeatureGroup->Delete();
  if(this->FeatureToMaskGroup)
	  this->FeatureToMaskGroup->Delete();
  if(this->MaskToFeatureGroup)
	  this->MaskToFeatureGroup->Delete();
 }

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}
	this->Superclass::CreateWidget();
	// add menu button with options for various Object
	// for surface
  if(!this->MainFrame)
    this->MainFrame = vtkKWFrame::New();

  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Image Operations");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->MainFrame->GetWidgetName());
/*
	this->ObjectMenuButton->SetParent(this->MainFrame->GetFrame());
	this->ObjectMenuButton->Create();
	this->ObjectMenuButton->SetBorderWidth(0);
	this->ObjectMenuButton->SetReliefToGroove();
	this->ObjectMenuButton->SetLabelText("Object :");
	this->ObjectMenuButton->SetPadX(2);
	this->ObjectMenuButton->SetPadY(2);
	this->ObjectMenuButton->SetWidth(30);
	this->ObjectMenuButton->SetLabelWidth(10);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->ObjectMenuButton->GetWidgetName());
	this->ObjectMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Image",this, "ImageMenuCallback");*/
	//this->ObjectMenuButton->GetWidget()->GetMenu()->AddRadioButton(
	//	"Image-Mask",this, "ImageMaskMenuCallback");

	// declare operations menu
	if(!this->OperationMenuButton)	
		this->OperationMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton->SetParent(this->MainFrame);
	this->OperationMenuButton->Create();
	this->OperationMenuButton->SetBorderWidth(0);
	this->OperationMenuButton->SetReliefToGroove();
	this->OperationMenuButton->GetWidget()->SetWidth(40);
	this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 15", 
		this->OperationMenuButton->GetWidgetName());
	this->OperationMenuButton->SetEnabled(1);
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Load",this, "LoadImageCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Load Feature",this, "LoadFeatureCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Threshold",this, "ThresholdImageCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Automated Segmentation",this, "AutomatedImageSegmentationCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Manual Segmentation",this, "ManualImageSegmentationCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Convert ROI to Mask",this, "FeatureToMaskCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Convert Mask to ROI",this, "MaskToFeatureCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Save Feature",this, "SaveFeatureCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Crop",this, "CropImageCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Delete",this, "DeleteImageCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Save",this, "SaveImageCallback");

	// declare operations menu
	if(!this->TypeMenuButton)	
		this->TypeMenuButton = vtkKWMenuButtonWithLabel::New();
	this->TypeMenuButton->SetParent(this->MainFrame);
	this->TypeMenuButton->Create();
/*
	this->TypeMenuButton->SetBorderWidth(0);
	this->TypeMenuButton->SetReliefToGroove();
	this->TypeMenuButton->SetLabelText("Type :");
	this->TypeMenuButton->SetPadX(2);
	this->TypeMenuButton->SetPadY(2);
	this->TypeMenuButton->SetWidth(30);
	this->TypeMenuButton->SetLabelWidth(10);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand y -padx 2 -pady 5 -fill x", 
		this->TypeMenuButton->GetWidgetName());
	this->TypeMenuButton->SetEnabled(0);
*/
	//this->MessageDialog->SetApplication(this->GetApplication());
	
	this->OperationMenuButton->GetWidget()->SetValue( "Load" );	
	
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::Update()
{
	this->UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::TraceImageCallback()
{
	this->ContourWidget = vtkContourWidget::New();
  vtkRenderWindowInteractor *iren = 	this->GetMimxMainWindow()->GetTraceRenderWidget()->GetRenderWindowInteractor();
  vtkRenderer *ren = this->GetMimxMainWindow()->GetTraceRenderWidget()->GetRenderer();

  this->ImageTraceActor = vtkImageActor::New();
  vtkImageData *image = vtkMimxImageActor::SafeDownCast(
		this->ImageList->GetItem(this->ImageList->GetNumberOfItems()-1))->GetDataSet();

  vtkImageShiftScale *imageScale = vtkImageShiftScale::New();
  imageScale->SetInput( image );
  imageScale->SetOutputScalarTypeToUnsignedChar();
  imageScale->SetScale(1);
  imageScale->Update();
  ImageTraceActor->SetInput( imageScale->GetOutput() );
  ImageTraceActor->SetZSlice(20);
  ren->AddActor( ImageTraceActor );

  this->ContourRepresentation = (vtkFocalPlaneContourRepresentation *)
    vtkFocalPlaneContourRepresentation::New();

  //vtkImageActorPointPlacer* placer = vtkImageActorPointPlacer::New();
  //placer->SetImageActor( this->ImageTraceActor );

  //this->ContourRepresentation->SetPointPlacer(placer);
  this->ContourWidget->SetInteractor( iren );
  //this->ContourWidget->SetRepresentation(this->ContourRepresentation);
  this->ContourWidget->SetKeyPressActivation(0);

  this->ContourWidget->On();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::LoadImageCallback()
{
	this->LoadImageGroup->SetParent( this->GetParent() );
	this->LoadImageGroup->SetImageList(this->ImageList);
	this->LoadImageGroup->SetMenuGroup(this);
	this->LoadImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
  this->LoadImageGroup->Create();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::LoadFeatureCallback()
{
	vtkKWMimxLoadFeatureGroup *loadFeatureGroup = vtkKWMimxLoadFeatureGroup::New();
  loadFeatureGroup->SetParent( this->GetParent() );
  loadFeatureGroup->SetFeatureList(this->FeatureList);
  loadFeatureGroup->SetMenuGroup(this);
  loadFeatureGroup->SetMimxMainWindow(this->GetMimxMainWindow());
  loadFeatureGroup->Create();
  loadFeatureGroup->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::SaveImageCallback()
{
	if (!this->SaveImageGroup)
    {
		this->SaveImageGroup = vtkKWMimxSaveImageGroup::New();
		this->SaveImageGroup->SetParent( this->GetParent() );
		this->SaveImageGroup->SetImageList(this->ImageList);
		this->SaveImageGroup->SetMenuGroup(this);
		this->SaveImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveImageGroup->Create();
    }
	else
    {
		this->SaveImageGroup->UpdateObjectLists();
    }
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
  
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->SaveImageGroup->GetBaseFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::SaveFeatureCallback()
{
	if (!this->SaveFeatureGroup)
    {
		this->SaveFeatureGroup = vtkKWMimxSaveFeatureGroup::New();
		this->SaveFeatureGroup->SetParent( this->GetParent() );
		this->SaveFeatureGroup->SetFeatureList(this->FeatureList);
		this->SaveFeatureGroup->SetMenuGroup(this);
		this->SaveFeatureGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveFeatureGroup->Create();
    }
	else
    {
		this->SaveFeatureGroup->UpdateObjectLists();
    }
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
  
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->SaveFeatureGroup->GetBaseFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::DeleteImageCallback()
{
	if(!this->DeleteObjectGroup)
	{
		this->DeleteObjectGroup = vtkKWMimxDeleteObjectGroup::New();
		this->DeleteObjectGroup->SetParent( this->GetParent() );
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(NULL);
		this->DeleteObjectGroup->SetImageList(this->ImageList);
		this->DeleteObjectGroup->SetMenuGroup(this);
		this->DeleteObjectGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->DeleteObjectGroup->Create();
		this->DeleteObjectGroup->UpdateObjectLists();
		this->TypeMenuButton->GetWidget()->SetValue("");
		this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
		this->TypeMenuButton->SetEnabled(0);
	}
	else
	{
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(NULL);
		this->DeleteObjectGroup->SetImageList(this->ImageList);
		this->DeleteObjectGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->DeleteObjectGroup->GetObjectListComboBox()->SetLabelText("Image: ");
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->DeleteObjectGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::ThresholdImageCallback()
{
	if (!this->ThresholdImageGroup)
  {
		this->ThresholdImageGroup = vtkKWMimxImageThresholdGroup::New();
		this->ThresholdImageGroup->SetParent( this->GetParent() );
		this->ThresholdImageGroup->SetImageList(this->ImageList);
		this->ThresholdImageGroup->SetMenuGroup(this);
		this->ThresholdImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->ThresholdImageGroup->Create();
  }
	else
  {
		this->ThresholdImageGroup->UpdateObjectLists();
  }
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
  
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->ThresholdImageGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::AutomatedImageSegmentationCallback()
{
	if (!this->AutomatedSegmentImageGroup)
    {
    this->AutomatedSegmentImageGroup = vtkKWMimxImageAutoSegmentGroup::New();
		this->AutomatedSegmentImageGroup->SetParent( this->GetParent() );
		this->AutomatedSegmentImageGroup->SetImageList(this->ImageList);
		this->AutomatedSegmentImageGroup->SetMenuGroup(this);
		this->AutomatedSegmentImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
    this->AutomatedSegmentImageGroup->SetUseFrameWithSlider( 0 );
		this->AutomatedSegmentImageGroup->Create();
    }
	else
    {
		this->AutomatedSegmentImageGroup->UpdateObjectLists();
    }
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->AutomatedSegmentImageGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::ManualImageSegmentationCallback()
{
	if (!this->ManualSegmentImageGroup)
    {
    this->ManualSegmentImageGroup = vtkKWMimxImageManualSegmentGroup::New();
		this->ManualSegmentImageGroup->SetParent( this->GetParent() );
		this->ManualSegmentImageGroup->SetImageList(this->ImageList);
    this->ManualSegmentImageGroup->SetFeatureList(this->FeatureList);
		this->ManualSegmentImageGroup->SetMenuGroup(this);
		this->ManualSegmentImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
    this->ManualSegmentImageGroup->SetUseFrameWithSlider( 0 );
		this->ManualSegmentImageGroup->Create();
    }
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->ManualSegmentImageGroup->GetMainFrame()->GetWidgetName());
  this->ManualSegmentImageGroup->EnableTracingInteractor();
  this->ManualSegmentImageGroup->UpdateObjectLists();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::FeatureToMaskCallback()
{
	if (!this->FeatureToMaskGroup)
    {
    this->FeatureToMaskGroup = vtkKWMimxCreateMaskFromFeatureGroup::New();
		this->FeatureToMaskGroup->SetParent( this->GetParent() );
		this->FeatureToMaskGroup->SetImageList(this->ImageList);
    this->FeatureToMaskGroup->SetFeatureList(this->FeatureList);
		this->FeatureToMaskGroup->SetMenuGroup(this);
		this->FeatureToMaskGroup->SetMimxMainWindow(this->GetMimxMainWindow());
    this->FeatureToMaskGroup->SetUseFrameWithSlider( 0 );
		this->FeatureToMaskGroup->Create();
    }
  else
    {
		this->FeatureToMaskGroup->UpdateObjectLists();
    }
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->FeatureToMaskGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::MaskToFeatureCallback()
{
	if (!this->MaskToFeatureGroup)
    {
    this->MaskToFeatureGroup = vtkKWMimxCreateFeatureFromMaskGroup::New();
		this->MaskToFeatureGroup->SetParent( this->GetParent() );
		this->MaskToFeatureGroup->SetImageList(this->ImageList);
    this->MaskToFeatureGroup->SetFeatureList(this->FeatureList);
		this->MaskToFeatureGroup->SetMenuGroup(this);
		this->MaskToFeatureGroup->SetMimxMainWindow(this->GetMimxMainWindow());
    this->MaskToFeatureGroup->SetUseFrameWithSlider( 0 );
		this->MaskToFeatureGroup->Create();
    }
  else
    {
		this->MaskToFeatureGroup->UpdateObjectLists();
    }
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->MaskToFeatureGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::CropImageCallback()
{
	if (!this->CropImageGroup)
    {
    this->CropImageGroup = vtkKWMimxCropImageGroup::New();
		this->CropImageGroup->SetParent( this->GetParent() );
		this->CropImageGroup->SetImageList(this->ImageList);
		this->CropImageGroup->SetMenuGroup(this);
		this->CropImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
    this->CropImageGroup->SetUseFrameWithSlider( 0 );
		this->CropImageGroup->Create();
    }
	else
    {
		this->CropImageGroup->UpdateObjectLists();
    }
  this->CropImageGroup->EnableCropImageCallback("");
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->CropImageGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxImageMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------
