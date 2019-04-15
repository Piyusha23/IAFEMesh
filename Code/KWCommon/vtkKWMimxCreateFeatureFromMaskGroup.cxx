/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateFeatureFromMaskGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/03/16 21:24:42 $
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
//For surface generation program
#include "vtkKWMimxCreateFeatureFromMaskGroup.h"

// VTK Headers
#include <vtkFieldData.h>
#include <vtkStringArray.h>
#include <vtkAbstractArray.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkMarchingContourFilter.h>
#include <vtkImageClip.h>
#include <vtkStripper.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkMaskPoints.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

// KWWidget Headers
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenu.h>
#include <vtkKWPushButton.h>
#include <vtkKWSpinBoxWithLabel.h>
#include <vtkKWSpinBox.h>

// MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxFeaturePolyDataActor.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "ManualTrace.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxCreateFeatureFromMaskGroup);
vtkCxxRevisionMacro(vtkKWMimxCreateFeatureFromMaskGroup, "$Revision: 1.2 $");

//----------------------------------------------------------------------------
vtkKWMimxCreateFeatureFromMaskGroup::vtkKWMimxCreateFeatureFromMaskGroup()
{
  this->ImageListComboBox = NULL;
  this->ComponentFrame = NULL;
  this->ParameterFrame = NULL;
  this->TraceAxisMenu = NULL;
  this->LabelValueEntry = NULL;
  this->MaxPointsEntry = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxCreateFeatureFromMaskGroup::~vtkKWMimxCreateFeatureFromMaskGroup()
{
  if(this->ImageListComboBox)
	  this->ImageListComboBox->Delete();
  if(this->ComponentFrame)
	  this->ComponentFrame->Delete();
  if(this->ParameterFrame)
	  this->ParameterFrame->Delete();
  if(this->TraceAxisMenu)
	  this->TraceAxisMenu->Delete();
  if(this->LabelValueEntry)
	  this->LabelValueEntry->Delete();
  if(this->MaxPointsEntry)
	  this->MaxPointsEntry->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateFeatureFromMaskGroup::CreateWidget()
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
  this->ComponentFrame->SetLabelText("Image & Feature");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ComponentFrame->GetWidgetName());
  this->ComponentFrame->CollapseFrame();

  if (!this->ImageListComboBox)	
      this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  this->ImageListComboBox->SetParent(this->ComponentFrame->GetFrame());
  this->ImageListComboBox->Create();
  this->ImageListComboBox->SetLabelText("Image : ");
  this->ImageListComboBox->SetLabelWidth(15);
  this->ImageListComboBox->GetWidget()->ReadOnlyOn();
  this->ImageListComboBox->GetWidget()->SetBalloonHelpString(
	  "Label map containing the region of interest for the feature");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->ImageListComboBox->GetWidgetName());
  
  //Frame for User Parameters
  if(!this->ParameterFrame)
	  this->ParameterFrame = vtkKWFrame::New();
  this->ParameterFrame->SetParent( this->MainFrame );
  this->ParameterFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
    this->ParameterFrame->GetWidgetName());
  
  // Axis Label
  if (!this->TraceAxisMenu)
	  this->TraceAxisMenu = vtkKWMenuButtonWithLabel::New();
  this->TraceAxisMenu->SetParent(this->ParameterFrame);
  this->TraceAxisMenu->Create();
  this->TraceAxisMenu->SetLabelWidth(30);
  this->TraceAxisMenu->SetLabelText("Feature Axis : ");
  this->TraceAxisMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->TraceAxisMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->TraceAxisMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->TraceAxisMenu->GetWidget()->SetValue("Axial");
  this->TraceAxisMenu->GetWidget()->SetBalloonHelpString(
        "Plane in which to create the feature");
  this->GetApplication()->Script(
       "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
      this->TraceAxisMenu->GetWidgetName());
  
  //Label Value
  if (!this->LabelValueEntry)
	  this->LabelValueEntry = vtkKWSpinBoxWithLabel::New();
  this->LabelValueEntry->SetParent(this->ParameterFrame);
  this->LabelValueEntry->Create();
  this->LabelValueEntry->SetLabelWidth(30);
  this->LabelValueEntry->SetLabelText("Label Value : ");
  this->LabelValueEntry->GetWidget()->SetRestrictValueToInteger();
  this->LabelValueEntry->GetWidget()->SetValue(1.0);
  this->LabelValueEntry->GetWidget()->SetRange(1, 4095);
  this->LabelValueEntry->GetWidget()->SetIncrement(1);
  this->LabelValueEntry->GetWidget()->SetIncrement(1);
  this->LabelValueEntry->GetWidget()->SetBalloonHelpString(
    "Value to create an feature from");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
     this->LabelValueEntry->GetWidgetName());
  
  //Number of Control Points
  if (!this->MaxPointsEntry)
	  this->MaxPointsEntry = vtkKWSpinBoxWithLabel::New();
  this->MaxPointsEntry->SetParent(this->ParameterFrame);
  this->MaxPointsEntry->Create();
  this->MaxPointsEntry->SetLabelWidth(30);
  this->MaxPointsEntry->SetLabelText("Max Control Points : ");
  this->MaxPointsEntry->GetWidget()->SetRestrictValueToInteger();
  this->MaxPointsEntry->GetWidget()->SetValue(7.0);
  this->MaxPointsEntry->GetWidget()->SetRange(1, 100);
  this->MaxPointsEntry->GetWidget()->SetIncrement(1);
  this->MaxPointsEntry->GetWidget()->SetBalloonHelpString(
      "Maximum number of control points for the feature");
  this->GetApplication()->Script(
      "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
      this->MaxPointsEntry->GetWidgetName());
  
  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "CreateMaskFromFeatureApplyCallback");
  this->CancelButton->SetCommand(this, "CreateMaskFromFeatureCancelCallback");

}

//----------------------------------------------------------------------------
void vtkKWMimxCreateFeatureFromMaskGroup::Update()
{
	this->UpdateEnableState();
}

//---------------------------------------------------------------------------
void vtkKWMimxCreateFeatureFromMaskGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
int vtkKWMimxCreateFeatureFromMaskGroup::CreateMaskFromFeatureApplyCallback()
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

  std::string imageName = this->ImageListComboBox->GetWidget()->GetValue();
	if (imageName == "" )
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}
  
	int imageNumber = this->ImageListComboBox->GetWidget()->GetValueIndex(imageName.c_str());
	if (imageNumber < 0 || imageNumber > this->ImageListComboBox->GetWidget()->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose a valid image");
		return 0;
	}
  
	//Get combobox
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(imageNumber));
  
  //Get selected values
	int labelValue = static_cast<int>(this->LabelValueEntry->GetWidget()->GetValue());
  int numCtrlPoints = static_cast<int>(this->MaxPointsEntry->GetWidget()->GetValue());
  std::string planeName = this->TraceAxisMenu->GetWidget()->GetValue();
  
  // Define Plane of the ROI
  int plane = ManualTrace::Unknown;
  if (planeName == "Axial")
    {
    plane = ManualTrace::Axial;
    }
  if (planeName == "Coronal")
    {
    plane = ManualTrace::Coronal;
    }
  if (planeName == "Sagittal")
    {
    plane = ManualTrace::Sagittal;
    }

  

  int extents[6];
  imageActor->GetOrientedDataSet()->GetExtent(extents);
  
  int loop;
  switch (plane) {
    case ManualTrace::Axial:
      loop = extents[5]-extents[4];
      break;
    case ManualTrace::Coronal:
      loop = extents[3]-extents[2];
      break;
    case ManualTrace::Sagittal:
      loop = extents[1]-extents[0];
      break;
    default:
      break;
  }
  
  
  int clipExtent[6] = {extents[0], extents[1], extents[2], extents[3], extents[4], extents[5]};
  vtkPolyData *resultContour = vtkPolyData::New();
  resultContour->Initialize();
  
  for (int i = 0; i<loop; i++)
		{
    //Reassign extents to get a single slice for direction of interest
    switch(plane)
			{
      case ManualTrace::Sagittal:
        clipExtent[0] = extents[0] + (i);
        clipExtent[1] = clipExtent[0];
        break;
      case ManualTrace::Coronal:
        clipExtent[2] = extents[2] + (i);
        clipExtent[3] = clipExtent[2];
        break;
      case ManualTrace::Axial:
        clipExtent[4] = extents[4] + (i);
        clipExtent[5] = clipExtent[4];
        break;
			}
    
    //Obtain a single slice using clipping filter
    vtkImageClip *clip = vtkImageClip::New();
    clip->SetInput(imageActor->GetOrientedDataSet());
    clip->SetOutputWholeExtent(clipExtent);
    clip->ClipDataOn();
    clip->Update();
    
    
    // Create the Contour
    vtkMarchingContourFilter *contourFilter = vtkMarchingContourFilter::New();
    contourFilter->SetValue(0,static_cast<double>(labelValue));
    contourFilter->ComputeNormalsOff();
    contourFilter->ComputeGradientsOff();
    contourFilter->ComputeScalarsOff();
    contourFilter->UseScalarTreeOn();
    contourFilter->SetInput(clip->GetOutput());
    contourFilter->Update();
    if (contourFilter->GetOutput()->GetNumberOfPoints() == 0)
      {
      contourFilter->Delete();
      clip->Delete();
      continue;
      }
    
    // Convert the contour into a single line per region
    vtkStripper *stripperFilter = vtkStripper::New();
    stripperFilter->SetInput(contourFilter->GetOutput());
    stripperFilter->Update();
    vtkPolyData *pd = vtkPolyData::New();
    pd->DeepCopy( stripperFilter->GetOutput() );
    
    
    // Set the Slice Index for Cell Data
    int numberOfCells = pd->GetNumberOfCells();
    vtkIntArray *cellData = vtkIntArray::New();
    cellData->SetNumberOfTuples( numberOfCells );
    for (int j=0;j<numberOfCells;j++)
      {
      cellData->SetValue(j, i);
      }
    cellData->SetName("SliceIndex");
    pd->GetCellData()->AddArray( cellData );
    cellData->Delete();
    
    
    // Decimate PolyData to define Node Points
    int reduce = pd->GetNumberOfPoints() / (numCtrlPoints * pd->GetNumberOfCells());
    vtkMaskPoints *maskFilter = vtkMaskPoints::New();
    maskFilter->SetInput(stripperFilter->GetOutput());
    maskFilter->SetOnRatio(reduce);
    maskFilter->Update();
    
    
    // Set the Node Points
    vtkIntArray *pointData = vtkIntArray::New();
    pointData->SetNumberOfTuples( pd->GetNumberOfPoints() );
    for (vtkIdType j=0;j<pd->GetNumberOfPoints();j++)
      {
      pointData->SetValue(j, 0);
      }
    
    for (vtkIdType j=0;j<maskFilter->GetOutput()->GetNumberOfPoints();j++)
      {
      double point[3];
      maskFilter->GetOutput()->GetPoint(j, point);
      vtkIdType nodeIndex = pd->FindPoint(point);
      pointData->SetValue(nodeIndex, 1);
      }
    
    //First Point of a cell must be a nodal point
    for (vtkIdType j=0;j<pd->GetNumberOfCells();j++)
      {
      vtkIdType nodeIndex = pd->GetCell(j)->GetPointId(0);
      pointData->SetValue(nodeIndex, 1);
      }
    pointData->SetName("NodePoints");
    pd->GetPointData()->AddArray( pointData );
    pointData->Delete();
    
    vtkAppendPolyData *appendfilter = vtkAppendPolyData::New();
    appendfilter->SetInput( resultContour );
    appendfilter->AddInput( pd );
    appendfilter->Update();
    resultContour->DeepCopy( appendfilter->GetOutput() );
    
    contourFilter->Delete();
    stripperFilter->Delete();
    clip->Delete();
    appendfilter->Delete();
    maskFilter->Delete();
		}
  
  // Set the Feature Field, Cell, and Point Data
  vtkStringArray *fieldData = vtkStringArray::New();
  fieldData->SetNumberOfTuples(1);
  fieldData->SetName("FEATURE-PLANE");
  switch (plane)
  {
    case ManualTrace::Axial:
      fieldData->SetValue(0, "AXIAL");
      break;
    case ManualTrace::Coronal:
      fieldData->SetValue(0, "CORONAL");
      break;
    case ManualTrace::Sagittal:
      fieldData->SetValue(0, "SAGITTAL");
      break;
    default:
      fieldData->SetValue(0, "UNKNOWN");
      break;
  }
  resultContour->GetFieldData()->AddArray( fieldData );
  fieldData->Delete();
  resultContour->GetPointData()->SetScalars(NULL);
  
  // Update the GUI
  this->AddFeatureToDisplay(resultContour,"ROI-", imageName.c_str(), 1, NULL);
  this->GetMimxMainWindow()->SetStatusText("Created ROI");
  this->UpdateObjectLists();
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateFeatureFromMaskGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateFeatureFromMaskGroup::CreateMaskFromFeatureCancelCallback()
{
	this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  
}

//----------------------------------------------------------------------------
int vtkKWMimxCreateFeatureFromMaskGroup::GetFeaturePlane(vtkPolyData *feature)
{
  int plane = ManualTrace::Unknown;
  
  vtkStringArray *stringArray = vtkStringArray::SafeDownCast(feature->GetFieldData()->GetAbstractArray("FEATURE-PLANE"));

  if (stringArray)
    {
    vtkStdString planeStr = stringArray->GetValue(0);
    
    if (planeStr == "AXIAL")
      {
      plane = ManualTrace::Axial;
      }
    if (planeStr == "CORONAL")
      {
      plane = ManualTrace::Coronal;
      }
    if (planeStr == "SAGITTAL")
      {
      plane = ManualTrace::Sagittal;
      }
    }
  
  return plane;
}

//------------------------------------------------------------------------------
void vtkKWMimxCreateFeatureFromMaskGroup::UpdateObjectLists()
{
	this->ImageListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
    {
		this->ImageListComboBox->GetWidget()->AddValue(this->ImageList->GetItem(i)->GetFileName());
    
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
    this->ImageListComboBox->GetWidget()->SetValue(this->ImageList->GetItem(defaultItem)->GetFileName());
    }
}

