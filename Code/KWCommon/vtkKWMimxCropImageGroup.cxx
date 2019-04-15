/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCropImageGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/02/17 22:20:18 $
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
//For surface generation program
#include "vtkKWMimxCropImageGroup.h"

// ITK Headers
#include <itkExtractImageFilter.h>

// VTK Headers
#include <vtkBoxWidget.h>
#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkMath.h>

// KWWidget Headers
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWExtent.h>
#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWRenderWidget.h>

// MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxCropImageGroup);
vtkCxxRevisionMacro(vtkKWMimxCropImageGroup, "$Revision: 1.3 $");


//----------------------------------------------------------------------------
class vtkBoxWidgetCropCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCropCallback *New()
  { return new vtkBoxWidgetCropCallback; }
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
vtkKWMimxCropImageGroup::vtkKWMimxCropImageGroup()
{
  this->ObjectListComboBox = NULL;
  this->ComponentFrame = NULL;
  this->ParameterFrame = NULL;
  this->CropExtents = NULL;
  this->BoxWidget = NULL;
  this->CurrentImageActor = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxCropImageGroup::~vtkKWMimxCropImageGroup()
{
  if(this->ObjectListComboBox)
	  this->ObjectListComboBox->Delete();
  if(this->ComponentFrame)
     this->ComponentFrame->Delete();
  if(this->ParameterFrame)
	  this->ParameterFrame->Delete();
  if(this->CropExtents)
	  this->CropExtents->Delete();
  if(this->BoxWidget)
	  this->BoxWidget->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();


  if ( !this->ComponentFrame )
	  this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent( this->MainFrame );
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Image");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ComponentFrame->GetWidgetName());
  this->ComponentFrame->CollapseFrame();

  if (!this->ObjectListComboBox)	
      this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();
  this->ObjectListComboBox->SetParent(this->ComponentFrame->GetFrame());
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetLabelText("Image : ");
  this->ObjectListComboBox->SetLabelWidth(15);
  this->ObjectListComboBox->GetWidget()->SetCommand(this, "EnableCropImageCallback");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetBalloonHelpString(
	  "Image to be cropped");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

  //Frame for User Parameters
  if(!this->ParameterFrame)
	  this->ParameterFrame = vtkKWFrameWithLabel::New();
  this->ParameterFrame->SetParent( this->MainFrame );
  this->ParameterFrame->Create();
  this->ParameterFrame->SetLabelText("Region Of Interest");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
                                 this->ParameterFrame->GetWidgetName());
  
  //Slider Bars - Define region to crop
  if(!this->CropExtents)
	  this->CropExtents = vtkKWExtent::New();
  this->CropExtents->SetParent(this->ParameterFrame->GetFrame());
  this->CropExtents->Create();
  this->CropExtents->SetBorderWidth(2);
  this->CropExtents->SetReliefToGroove();
  this->CropExtents->SetExtentRange(0,150,0,150,0,150);
  this->CropExtents->SetLabelPositionToLeft();
  this->CropExtents->SetEntry1PositionToLeft();
  this->CropExtents->SetEntry2PositionToRight();
  this->CropExtents->SetSliderSize(4);
  this->CropExtents->SetThickness(15);
  this->CropExtents->SetInternalThickness(0.7);
  this->CropExtents->SetRequestedLength(150);
  this->CropExtents->GetXRange()->SetAdjustResolution(1);
  this->CropExtents->SetEndCommand(this, "CropSliderCallback");
  this->CropExtents->SetBalloonHelpString(
      "ROI to be cropped from the image");
  this->GetApplication()->Script(
              "pack %s -side top -anchor nw -expand n -padx 2 -pady 6",
              this->CropExtents->GetWidgetName());
  
  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "CropImageApplyCallback");
  this->CancelButton->SetCommand(this, "CropImageCancelCallback");

}

//----------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::Update()
{
	this->UpdateEnableState();
}

//---------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
int vtkKWMimxCropImageGroup::CropImageApplyCallback()
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	if(this->CurrentImageActor == NULL)
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	double bounds[6] = {0,0,0,0,0,0};
	this->CropExtents->GetExtent(bounds);
  
	//Convert from RAS back to IJK Index coordinates
	double RASmin[3] = {bounds[0],bounds[2],bounds[4]};
	double RASmax[3] = {bounds[1],bounds[3],bounds[5]};
	double IJKmin[3] = {0,0,0};
	double IJKmax[3] = {0,0,0};
  
  //std::cout << "RAS Image Min - " << RASmin[0] << " " << RASmin[1] << " " << RASmin[2] << std::endl;
  //std::cout << "RAS Image Max - " << RASmax[0] << " " << RASmax[1] << " " << RASmax[2] << std::endl;
  
	this->CurrentImageActor->RasToIjkTransformPoint(RASmin, IJKmin);
	this->CurrentImageActor->RasToIjkTransformPoint(RASmax, IJKmax);
  
  //std::cout << "IJK Image Min - " << IJKmin[0] << " " << IJKmin[1] << " " << IJKmin[2] << std::endl;
  //std::cout << "IJK Image Max - " << IJKmax[0] << " " << IJKmax[1] << " " << IJKmax[2] << std::endl;
  
  ImageType::RegionType region = this->CurrentImageActor->GetITKImage()->GetLargestPossibleRegion();
  ImageType::RegionType::SizeType  size;
  ImageType::RegionType::IndexType inputStart;
  inputStart[0] = vtkMath::Round(IJKmin[0]);
  inputStart[1] = vtkMath::Round(IJKmin[1]);
  inputStart[2] = vtkMath::Round(IJKmin[2]);
  
  ImageType::IndexType inputEnd;
  inputEnd[0] = vtkMath::Round(IJKmax[0]);
  inputEnd[1] = vtkMath::Round(IJKmax[1]);
  inputEnd[2] = vtkMath::Round(IJKmax[2]);
  
  //std::cout << "Input Start - " << inputStart << std::endl;
  //std::cout << "Input End - " << inputEnd << std::endl;
  
	for (int i=0; i<3; i++)
    {
		int tempIndex = 0;
		if (inputEnd[i] < inputStart[i])
      {
			tempIndex = inputStart[i];
			inputStart[i] = inputEnd[i];
			inputEnd[i] = tempIndex;
      }
    if (inputStart[i] < 0)
      {
      inputStart[i] = 0;
      }
    if (inputEnd[i] > region.GetSize()[i]-1)
      {
      inputEnd[i] = region.GetSize()[i]-1;
      }
    }
  
	size[0] = inputEnd[0] - inputStart[0] + 1;
	size[1] = inputEnd[1] - inputStart[1] + 1;
	size[2] = inputEnd[2] - inputStart[2] + 1;
  
  ImageType::RegionType extractRegion;
  extractRegion.SetSize( size );
  extractRegion.SetIndex( inputStart );
  
  //std::cout << "Extraction Region: " << extractRegion << std::endl;

  typedef itk::ExtractImageFilter< ImageType, ImageType > CropImageFilterType;
  CropImageFilterType::Pointer cropImageFilter = CropImageFilterType::New();
  cropImageFilter->SetInput( this->CurrentImageActor->GetITKImage() );
  cropImageFilter->SetExtractionRegion(extractRegion);
  try
    {
    cropImageFilter->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    callback->ErrorMessage("Crop image failed!");
    return 0;
    }
  
  this->CurrentImageActor->SetInteraction(1);
  this->CurrentImageActor->DisplayActor(0);
  std::string foundationName = this->CurrentImageActor->GetFileName();
  this->CurrentImageActor = NULL;
  this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
  this->AddImageToDisplay(cropImageFilter->GetOutput(), "Crop-", foundationName.c_str(), 1 , NULL);
  this->GetMimxMainWindow()->SetStatusText("Cropped Image");
  
  this->UpdateObjectLists();
  
  if (this->BoxWidget)
    {
    this->BoxWidget->EnabledOff();
    this->BoxWidget->Delete();
    this->BoxWidget = NULL;
    }
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::CropImageCancelCallback()
{
	if (this->BoxWidget)
    {
    this->BoxWidget->EnabledOff();
    this->BoxWidget->Delete();
    this->BoxWidget = NULL;
    }
  
  if (this->CurrentImageActor)
    {
    this->CurrentImageActor->SetInteraction(1);
    this->CurrentImageActor = NULL;
    }
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}

//------------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
    {
		ObjectListComboBox->GetWidget()->AddValue(
                                              this->ImageList->GetItem(i)->GetFileName());
    
		int viewedItem = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(i))->GetActorVisibility();
		if ( (defaultItem == -1) && ( viewedItem ) )
      {
		  defaultItem = i;
      }
    }
	
	if ((this->ImageList->GetNumberOfItems() > 0) && (defaultItem == -1))
    defaultItem = this->ImageList->GetNumberOfItems()-1;
  
	if (defaultItem != -1)
    {
    ObjectListComboBox->GetWidget()->SetValue(
                                              this->ImageList->GetItem(defaultItem)->GetFileName());
    }
}

//----------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::EnableCropImageCallback(char *value)
{
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  std::string name = combobox->GetValue();
  if (name == "")
    {
    this->CropExtents->SetEnabled(0);
    return;
    }
  
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name.c_str())));
  if (imageActor == NULL)
    {
    this->CropExtents->SetEnabled(0);
    return;
    }
  
  if (this->BoxWidget)
    {
    this->BoxWidget->EnabledOff();
    this->BoxWidget->Delete();
    this->BoxWidget = NULL;
    }
  
  if (this->CurrentImageActor)
    {
    this->CurrentImageActor->SetInteraction(1);
    }
  
  for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
    {
		vtkMimxImageActor *tmpImageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(i));
    tmpImageActor->DisplayActor(0);
    }
  imageActor->DisplayActor(1);
  this->UpdateObjectLists();
  
  this->CurrentImageActor = imageActor;
  
	this->BoxWidget = vtkBoxWidget::New();
	this->BoxWidget->KeyPressActivationOff();
	this->BoxWidget->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
	this->BoxWidget->SetPlaceFactor(1.0);
	this->BoxWidget->SetInput(imageActor->GetOrientedDataSet());
	this->BoxWidget->PlaceWidget();
	this->BoxWidget->SetEnabled(1);
	this->BoxWidget->SetHandleSize(0.005);
	this->BoxWidget->RotationEnabledOff();
  
	double bounds[6] = {0,0,0,0,0,0};
	this->CurrentImageActor->SetInteraction(0);
	this->CurrentImageActor->GetOrientedDataSet()->GetBounds(bounds);
  
  //std::cout << "X Bounds : " << bounds[0] << " " << bounds[1] << std::endl;
  //std::cout << "Y Bounds : " << bounds[2] << " " << bounds[3] << std::endl;
  //std::cout << "Z Bounds : " << bounds[4] << " " << bounds[5] << std::endl;
  
  this->CropExtents->SetEnabled(1);
	this->CropExtents->SetExtentRange(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
	this->CropExtents->SetExtent(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
  
	vtkBoxWidgetCropCallback *myCallback = vtkBoxWidgetCropCallback::New();
	myCallback->extentWidget = this->CropExtents;
	this->BoxWidget->AddObserver(vtkCommand::InteractionEvent,myCallback);
}

//----------------------------------------------------------------------------
void vtkKWMimxCropImageGroup::CropSliderCallback(double xlower, double xupper, double ylower, double yupper, double zlower, double zupper)
{
	double sliderbounds[6] = {0,0,0,0,0,0};
	this->CropExtents->GetExtent(sliderbounds);
  
  if (this->BoxWidget)
    {
    this->BoxWidget->PlaceWidget(sliderbounds);
    this->BoxWidget->SetHandleSize(0.005);
    }
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}
