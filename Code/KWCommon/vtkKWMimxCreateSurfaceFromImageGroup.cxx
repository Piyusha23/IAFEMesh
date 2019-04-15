/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateSurfaceFromImageGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/02/10 03:03:26 $
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
//For surface generation program
#include "vtkKWMimxCreateSurfaceFromImageGroup.h"

// ITK Headers
#include <itkImage.h>
#include <itkAntiAliasBinaryImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>

// VTK Headers
#include <vtkMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkClipPolyData.h>
#include <vtkFieldData.h>
#include <vtkImageChangeInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkIdList.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>

// KWWidget Headers
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWPushButton.h>
#include <vtkKWSpinBoxWithLabel.h>
#include <vtkKWSpinBox.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>

// MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxCreateSurfaceFromImageGroup);
vtkCxxRevisionMacro(vtkKWMimxCreateSurfaceFromImageGroup, "$Revision: 1.4 $");

//----------------------------------------------------------------------------
vtkKWMimxCreateSurfaceFromImageGroup::vtkKWMimxCreateSurfaceFromImageGroup()
{
  this->LabelImageButton = NULL;
  this->RMSvalueEntry = NULL;
  this->ITKiterationsEntry = NULL;
  this->VTKiterationsEntry = NULL;
  this->DecimationValueEntry = NULL;
  this->ComponentFrame = NULL;
  this->ObjectListComboBox = NULL;
  this->ParameterFrame = NULL;
  this->SurfaceValueEntry = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxCreateSurfaceFromImageGroup::~vtkKWMimxCreateSurfaceFromImageGroup()
{
  if(this->LabelImageButton)
	  this->LabelImageButton->Delete();
  if(this->RMSvalueEntry)
     this->RMSvalueEntry->Delete();
  if(this->ITKiterationsEntry)
	  this->ITKiterationsEntry->Delete();
  if(this->VTKiterationsEntry)
	  this->VTKiterationsEntry->Delete();
  if(this->DecimationValueEntry)
	  this->DecimationValueEntry->Delete();
  if (this->ComponentFrame)
	  this->ComponentFrame->Delete();
  if(this->ObjectListComboBox)
	  this->ObjectListComboBox->Delete();
  if(this->ParameterFrame)
	  this->ParameterFrame->Delete();
  if(this->SurfaceValueEntry)
	  this->SurfaceValueEntry->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateSurfaceFromImageGroup::CreateWidget()
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
  ObjectListComboBox->SetParent(this->ComponentFrame->GetFrame());
  ObjectListComboBox->Create();
  ObjectListComboBox->SetLabelText("Image : ");
  ObjectListComboBox->SetLabelWidth(15);
  ObjectListComboBox->GetWidget()->ReadOnlyOn();
  ObjectListComboBox->GetWidget()->SetBalloonHelpString(
	  "Image to be used for surface generation");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

  //Frame for User Parameters
  if(!this->ParameterFrame)
	  this->ParameterFrame = vtkKWFrame::New();
  this->ParameterFrame->SetParent( this->MainFrame );
  this->ParameterFrame->Create();
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
                                 this->ParameterFrame->GetWidgetName());
  
  //Label Image Button
  if (!this->LabelImageButton)
	  this->LabelImageButton = vtkKWCheckButtonWithLabel::New();
  this->LabelImageButton->SetParent(this->ParameterFrame);
  this->LabelImageButton->Create();
  this->LabelImageButton->SetLabelText("Label Image : ");
  this->LabelImageButton->SetLabelWidth(30);
  this->LabelImageButton->SetBalloonHelpString("Select this option if the image is binary or the image represents multiple discerete labels");
  this->LabelImageButton->GetWidget()->SetCommand(this, "LabelImageCallback");
  this->GetApplication()->Script(
     "pack %s -side top -anchor nw -expand y -padx 2 -pady 6",
     this->LabelImageButton->GetWidgetName());
  
  //Iso Surface Value
  if (!this->SurfaceValueEntry)
	  this->SurfaceValueEntry = vtkKWSpinBoxWithLabel::New();
  this->SurfaceValueEntry->SetParent(this->ParameterFrame);
  this->SurfaceValueEntry->Create();
  this->SurfaceValueEntry->SetLabelWidth(30);
  this->SurfaceValueEntry->SetLabelText("Iso-Surface Value : ");
  this->SurfaceValueEntry->GetWidget()->SetValueFormat("%5.3f");
  this->SurfaceValueEntry->GetWidget()->SetValue(1.0);
  this->SurfaceValueEntry->GetWidget()->SetRestrictValueToDouble();
  this->SurfaceValueEntry->GetWidget()->SetRange(-1000, 4095);
  this->SurfaceValueEntry->GetWidget()->SetIncrement(1);
  this->SurfaceValueEntry->GetWidget()->SetBalloonHelpString(
                        "Image value to be used for surface generation");
  this->GetApplication()->Script(
                                 "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
                                 this->SurfaceValueEntry->GetWidgetName());
  
  //ITK Smoothing RMS Error Value
    if (!this->RMSvalueEntry)
	  this->RMSvalueEntry = vtkKWSpinBoxWithLabel::New();
  this->RMSvalueEntry->SetParent(this->ParameterFrame);
  this->RMSvalueEntry->Create();
  this->RMSvalueEntry->SetLabelWidth(30);
  this->RMSvalueEntry->SetLabelText("Anti-Aliasing RMS : ");
  this->RMSvalueEntry->GetWidget()->SetValueFormat("%5.3f");
  this->RMSvalueEntry->GetWidget()->SetValue(0.001);
  this->RMSvalueEntry->GetWidget()->SetRestrictValueToDouble();
  this->RMSvalueEntry->GetWidget()->SetRange(0.0, 1.0);
  this->RMSvalueEntry->GetWidget()->SetIncrement(0.001);
  this->RMSvalueEntry->GetWidget()->SetBalloonHelpString(
        "Anti-Aliasing RMS error value to be used for the image before surface generation");
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->RMSvalueEntry->GetWidgetName());
  this->RMSvalueEntry->SetEnabled(0);
  
  //ITK Smoothing Iterations
  if (!this->ITKiterationsEntry)
	  this->ITKiterationsEntry = vtkKWSpinBoxWithLabel::New();
  this->ITKiterationsEntry->SetParent(this->ParameterFrame);
  this->ITKiterationsEntry->Create();
  this->ITKiterationsEntry->SetLabelWidth(30);
  this->ITKiterationsEntry->GetWidget()->SetValue(20);
  this->ITKiterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->ITKiterationsEntry->GetWidget()->SetRange(0.0, 10000.0);
  this->ITKiterationsEntry->GetWidget()->SetIncrement(10);
  this->ITKiterationsEntry->SetLabelText("Anti-aliasing Iterations : ");
  this->ITKiterationsEntry->GetWidget()->SetBalloonHelpString(
        "Number of Anti-Aliasing iterations for the image before surface generation");
  this->ITKiterationsEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->ITKiterationsEntry->GetWidgetName());
  this->ITKiterationsEntry->SetEnabled(0);
  
  //Decimation Reduction Decimal
  if (!this->DecimationValueEntry)
	  this->DecimationValueEntry = vtkKWSpinBoxWithLabel::New();
  this->DecimationValueEntry->SetParent(this->ParameterFrame);
  this->DecimationValueEntry->Create();
  this->DecimationValueEntry->SetLabelWidth(30);
  this->DecimationValueEntry->SetLabelText("Decimation Percentage : ");
  this->DecimationValueEntry->SetBalloonHelpString("Fraction of vertices to remove");
  this->DecimationValueEntry->GetWidget()->SetValueFormat("%3.2f");
  this->DecimationValueEntry->GetWidget()->SetValue(0.5);
  this->DecimationValueEntry->GetWidget()->SetRestrictValueToDouble();
  this->DecimationValueEntry->GetWidget()->SetRange(0.0, 1.0);
  this->DecimationValueEntry->GetWidget()->SetIncrement(0.1);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
	  this->DecimationValueEntry->GetWidgetName());
  
  //VTK Smoothing Iterations
  if (!this->VTKiterationsEntry)
	  this->VTKiterationsEntry = vtkKWSpinBoxWithLabel::New();
  this->VTKiterationsEntry->SetParent(this->ParameterFrame);
  this->VTKiterationsEntry->Create();
  this->VTKiterationsEntry->SetLabelWidth(30);
  this->VTKiterationsEntry->GetWidget()->SetValue( 50 );
  this->VTKiterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->VTKiterationsEntry->GetWidget()->SetRange(0.0, 10000.0);
  this->VTKiterationsEntry->GetWidget()->SetIncrement(10);
  this->VTKiterationsEntry->SetLabelText("Surface Smoothing Iterations : ");
  this->VTKiterationsEntry->GetWidget()->SetBalloonHelpString(
    "Number of iterations to be used for windowed sinc surface smoothing");
  this->VTKiterationsEntry->GetWidget()->SetRestrictValueToDouble();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
    this->VTKiterationsEntry->GetWidgetName());
  
  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "CreateSurfaceFromImageApplyCallback");
  this->CancelButton->SetCommand(this, "CreateSurfaceFromImageCancelCallback");

}

//----------------------------------------------------------------------------
void vtkKWMimxCreateSurfaceFromImageGroup::Update()
{
	this->UpdateEnableState();
}

//---------------------------------------------------------------------------
void vtkKWMimxCreateSurfaceFromImageGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
int vtkKWMimxCreateSurfaceFromImageGroup::CreateSurfaceFromImageApplyCallback()
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
	const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose a valid image");
		combobox->SetValue("");
		return 0;
	}

	//Get combobox
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(
	this->ImageList->GetItem(combobox->GetValueIndex(name)));

	//Get selected values
	int labelImage = static_cast<int>(this->LabelImageButton->GetWidget()->GetSelectedState());
  float surfaceValue = static_cast<float>(this->SurfaceValueEntry->GetWidget()->GetValue());
  float rmsValue = static_cast<float>(this->RMSvalueEntry->GetWidget()->GetValue());
	int itkIterations = static_cast<int>(this->ITKiterationsEntry->GetWidget()->GetValue());
	int vtkIterations = static_cast<int>(this->VTKiterationsEntry->GetWidget()->GetValue());
	float decimationValue = static_cast<float>(this->DecimationValueEntry->GetWidget()->GetValue());
  
  
  //Check Input Parameter values
	if ( (rmsValue > 1) || (rmsValue < 0) )
    {
	  callback->ErrorMessage("RMS value must be between 0 and 1");
		return 0;
    }
  
	if ( (decimationValue < 0) || (decimationValue > 1) )
    {
	  callback->ErrorMessage("Decimation reduction must be between 0 and 1");
		return 0;
    }
  
  if ( itkIterations < 0)
    {
	  callback->ErrorMessage("Anti-aliasing iterations cannot be negative");
		return 0;
    }
  
  if ( vtkIterations < 0)
    {
	  callback->ErrorMessage("Number of laplacian iterations cannot be negative");
		return 0;
    }
  
  ImageType::Pointer inputImage = imageActor->GetITKImage();
  vtkPolyData *originalSurface;
	
  if ( labelImage )
    {
    originalSurface = CreateBinarySurface(inputImage, static_cast<ImagePixelType>(surfaceValue), rmsValue, itkIterations);
    }
  else
    {
    originalSurface = CreateContinuosSurface(inputImage, surfaceValue);
    }
  
	//VTK smoothing filter
	vtkWindowedSincPolyDataFilter *smoothSurfaceFilter = vtkWindowedSincPolyDataFilter::New();
	smoothSurfaceFilter->SetInput( originalSurface );
	smoothSurfaceFilter->SetNumberOfIterations( vtkIterations );
	smoothSurfaceFilter->Update();
  
	//Decimate surface
	vtkDecimatePro *decimateSurfaceFilter = vtkDecimatePro::New();
	decimateSurfaceFilter->SetInput( smoothSurfaceFilter->GetOutput() );
	decimateSurfaceFilter->SetTargetReduction( decimationValue );
	decimateSurfaceFilter->Update();
	
  vtkTransformPolyDataFilter *orientSurfaceFilter = vtkTransformPolyDataFilter::New();
  orientSurfaceFilter->SetInput( decimateSurfaceFilter->GetOutput() );
  orientSurfaceFilter->SetTransform( imageActor->GetIJKToRASTransform() );
  orientSurfaceFilter->Update( );
  
  //Add the Surface to the GUI
	callback->SetState(0);
  std::string foundationName = this->ImageList->GetItem(combobox->GetValueIndex(name))->GetFileName();
  this->AddSurfaceToDisplay(orientSurfaceFilter->GetOutput(), "Surface-", foundationName.c_str(), 1, NULL);
  this->GetMimxMainWindow()->SetStatusText("Created Surface");
  
  // Free Memory
  decimateSurfaceFilter->Delete();
  smoothSurfaceFilter->Delete();
  orientSurfaceFilter->Delete();
  originalSurface->Delete();
  
  return 1;
}

//----------------------------------------------------------------------------
vtkPolyData* vtkKWMimxCreateSurfaceFromImageGroup::CreateBinarySurface(ImageType::Pointer image, ImagePixelType surfaceValue, float rms, int iterations)
{
  // Extract the Label of Interest
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ExtractLabelFilterType;
  ExtractLabelFilterType::Pointer extractLabelImageFilter = ExtractLabelFilterType::New();
  extractLabelImageFilter->SetInput( image );
  extractLabelImageFilter->SetOutsideValue( 0 );
  extractLabelImageFilter->SetInsideValue( 1 );
  extractLabelImageFilter->SetLowerThreshold( surfaceValue );
  extractLabelImageFilter->SetUpperThreshold( surfaceValue );
  try
  {
    extractLabelImageFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    return 0;
  }
  
  typedef itk::AntiAliasBinaryImageFilter<ImageType, FloatImageType> AntiAliasFilterType;
  AntiAliasFilterType::Pointer antiAliasImageFilter = AntiAliasFilterType::New();
  antiAliasImageFilter->SetInput( extractLabelImageFilter->GetOutput() );
  antiAliasImageFilter->SetMaximumRMSError( rms );
  antiAliasImageFilter->SetNumberOfIterations( iterations );
  antiAliasImageFilter->Update();
  try
  {
    antiAliasImageFilter->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    return 0;
  }
  
  //Convert image from ITK to VTK
	typedef itk::ImageToVTKImageFilter<FloatImageType> ITKToVTKFilterType;
	ITKToVTKFilterType::Pointer convertITKToVTKImageFilter = ITKToVTKFilterType::New();
	convertITKToVTKImageFilter->SetInput( antiAliasImageFilter->GetOutput() );
	convertITKToVTKImageFilter->Update();
  
	vtkImageChangeInformation *changeImageInfoFilter = vtkImageChangeInformation::New();
	changeImageInfoFilter->SetInput (convertITKToVTKImageFilter->GetOutput());
	changeImageInfoFilter->SetOutputSpacing( 1, 1, 1 );
	changeImageInfoFilter->SetOutputOrigin( 0, 0, 0 );
	changeImageInfoFilter->Update();
  
	//Generate surface
	vtkMarchingCubes *marchingCubesFilter = vtkMarchingCubes::New();
	marchingCubesFilter->SetInput(changeImageInfoFilter->GetOutput());
  marchingCubesFilter->SetValue(0, 0);
	marchingCubesFilter->Update();
  marchingCubesFilter->GetOutput();
  
  vtkPolyData *surface = vtkPolyData::New();
  surface->DeepCopy( marchingCubesFilter->GetOutput() );
  changeImageInfoFilter->Delete();
  marchingCubesFilter->Delete();
  
  return surface;
}

//----------------------------------------------------------------------------
vtkPolyData* vtkKWMimxCreateSurfaceFromImageGroup::CreateContinuosSurface(ImageType::Pointer image, float surfaceValue)
{
  // Convert image from ITK to VTK
  typedef itk::ImageToVTKImageFilter<ImageType> ITKToVTKFilterType;
	ITKToVTKFilterType::Pointer convertITKToVTKImageFilter = ITKToVTKFilterType::New();
	convertITKToVTKImageFilter->SetInput( image );
	convertITKToVTKImageFilter->Update();
  
  // Change the image origin and spacing
	vtkImageChangeInformation *changeImageInfoFilter = vtkImageChangeInformation::New();
	changeImageInfoFilter->SetInput ( convertITKToVTKImageFilter->GetOutput() );
	changeImageInfoFilter->SetOutputSpacing( 1, 1, 1 );
	changeImageInfoFilter->SetOutputOrigin( 0, 0, 0 );
	changeImageInfoFilter->Update();
  
	//Generate surface
	vtkMarchingCubes *marchingCubesFilter = vtkMarchingCubes::New();
	marchingCubesFilter->SetInput(changeImageInfoFilter->GetOutput());
  marchingCubesFilter->SetValue(0, surfaceValue);
	marchingCubesFilter->Update();
  
  vtkPolyData *surface = vtkPolyData::New();
  surface->DeepCopy( marchingCubesFilter->GetOutput() );
  changeImageInfoFilter->Delete();
  marchingCubesFilter->Delete();
  
  return surface;
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateSurfaceFromImageGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateSurfaceFromImageGroup::CreateSurfaceFromImageCancelCallback()
{
	this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  
}

//------------------------------------------------------------------------------
void vtkKWMimxCreateSurfaceFromImageGroup::UpdateObjectLists()
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

//---------------------------------------------------------------------------
int vtkKWMimxCreateSurfaceFromImageGroup::LabelImageCallback(int state)
{
  if ( state )
    {
    this->RMSvalueEntry->SetEnabled(1);
    this->ITKiterationsEntry->SetEnabled(1);
    }
  else
    {
    this->RMSvalueEntry->SetEnabled(0);
    this->ITKiterationsEntry->SetEnabled(0);
    }
  
  return 1;
}

