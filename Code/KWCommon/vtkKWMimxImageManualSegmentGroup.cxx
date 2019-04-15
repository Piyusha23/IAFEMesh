/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageManualSegmentGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/04/04 02:25:56 $
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

#include "vtkKWMimxImageManualSegmentGroup.h"

// VTK Headers
#include <vtkObjectFactory.h>
#include <vtkCamera.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkProperty.h>
#include <vtkImageActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkAppendPolyData.h>
#include <vtkIdList.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkIntArray.h>
#include <vtkStringArray.h>
#include <vtkCellData.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorObserver.h>
#include <vtkImageActorPointPlacer.h>
#include <vtkBoundedPlanePointPlacer.h>
#include <vtkPlane.h>
#include <vtksys/SystemTools.hxx>

// KWWidget Headers
#include <vtkKWFrameWithLabel.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWRadioButton.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenu.h>
#include <vtkKWScale.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWRenderWidget.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>

// MIMX Headers
#include "vtkKWMimxApplication.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxFeaturePolyDataActor.h"
#include "vtkMimxImagePlaneWidget.h"
#include "vtkMimxContourTraceWidget.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "mimxCommonDefine.h"



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxImageManualSegmentGroup);
vtkCxxRevisionMacro(vtkKWMimxImageManualSegmentGroup, "$Revision: 1.9 $");

//----------------------------------------------------------------------------
vtkKWMimxImageManualSegmentGroup::vtkKWMimxImageManualSegmentGroup()
{
	//Initializers
	this->ImageListFrame = NULL;
	this->ImageListComboBox = NULL;
	this->FeatureListComboBox = NULL;
  this->NewContourButton = NULL;
	this->AxisFrame = NULL;
	this->XAxis = NULL;
	this->YAxis = NULL;
	this->ZAxis = NULL;
	this->AddRemoveButtonFrame = NULL;
	this->RemoveContourButton = NULL;
	this->AddContourButton = NULL;
	this->CopyButtonFrame = NULL;
	this->Copy2PrevContourButton = NULL;
	this->Copy2NextContourButton = NULL;
	this->SliceScale = NULL;
	this->ScaleFrame = NULL;
	this->ActiveActorName = "";
	this->ActiveTraceName = "";
  this->TracingAxis = 2;
	this->ImageActorWidgetX = NULL;
	this->ImageActorWidgetY = NULL;
	this->ImageActorWidgetZ = NULL;
  this->TraceInteractorStyle = vtkInteractorStyleImage::New();
  this->DefaultInteractorStyle = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxImageManualSegmentGroup::~vtkKWMimxImageManualSegmentGroup()
{
	if (this->ImageListFrame)
    this->ImageListFrame->Delete();
	if (this->ImageListComboBox)
    this->ImageListComboBox->Delete();
	if (this->FeatureListComboBox)
    this->FeatureListComboBox->Delete();
  if (this->NewContourButton)
    this->NewContourButton->Delete();
	if (this->AxisFrame)
    this->AxisFrame->Delete();
	if (this->XAxis)
    this->XAxis->Delete();
	if (this->YAxis)
    this->YAxis->Delete();
	if (this->ZAxis)
    this->ZAxis->Delete();
	if (this->AddRemoveButtonFrame)
    this->AddRemoveButtonFrame->Delete();
	if (this->RemoveContourButton)
    this->RemoveContourButton->Delete();
	if (this->AddContourButton)
    this->AddContourButton->Delete();
	if (this->CopyButtonFrame)
    this->CopyButtonFrame->Delete();
	if (this->Copy2PrevContourButton)
    this->Copy2PrevContourButton->Delete();
	if (this->Copy2NextContourButton)
    this->Copy2NextContourButton->Delete();
	if (this->SliceScale)
    this->SliceScale->Delete();
	if (this->ScaleFrame)
    this->ScaleFrame->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::CreateWidget()
{
  if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}

  this->Superclass::CreateWidget();
  

  //Select the image from the list
  if ( !this->ImageListFrame )
    this->ImageListFrame = vtkKWFrameWithLabel::New();
  this->ImageListFrame->SetParent( this->MainFrame );
  this->ImageListFrame->Create();
  this->ImageListFrame->SetLabelText("Image & Feature");
  this->ImageListFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ImageListFrame->GetWidgetName());
  
  if (!this->ImageListComboBox)	
      this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  this->ImageListComboBox->SetParent(this->ImageListFrame->GetFrame());
  this->ImageListComboBox->Create();
  this->ImageListComboBox->SetLabelText("Image : ");
  this->ImageListComboBox->SetLabelWidth(15);
  this->ImageListComboBox->GetWidget()->ReadOnlyOn();
  this->ImageListComboBox->GetWidget()->SetBalloonHelpString(
	  "Select the image to segment");
  this->ImageListComboBox->GetWidget()->SetCommand(this, "SetImageCallback");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ImageListComboBox->GetWidgetName());

  if (!this->FeatureListComboBox)	
      this->FeatureListComboBox = vtkKWComboBoxWithLabel::New();
  this->FeatureListComboBox->SetParent(this->ImageListFrame->GetFrame());
  this->FeatureListComboBox->Create();
  this->FeatureListComboBox->SetLabelText("Feature : ");
  this->FeatureListComboBox->SetLabelWidth(15);
  this->FeatureListComboBox->GetWidget()->ReadOnlyOn();
  this->FeatureListComboBox->GetWidget()->SetBalloonHelpString(
	  "Select contour to edit");
	this->FeatureListComboBox->GetWidget()->SetCommand(this, "SetFeatureCallback");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x",
    this->FeatureListComboBox->GetWidgetName());

  if(!this->NewContourButton)
	  this->NewContourButton = vtkKWPushButton::New();
  this->NewContourButton->SetParent(this->ImageListFrame->GetFrame());
  this->NewContourButton->Create();
  this->NewContourButton->SetText("New ROI");
  this->NewContourButton->SetCommand(this, "NewContourCallback");
  this->NewContourButton->Script(
    "pack %s -side right -anchor nw -expand n -padx 2 -pady 2",
    this->NewContourButton->GetWidgetName());
  
  //Add a scale bar to allow for moving through directional slices
  if ( !this->ScaleFrame )
    this->ScaleFrame = vtkKWFrameWithLabel::New();
  this->ScaleFrame->SetParent( this->MainFrame );
  this->ScaleFrame->Create();
  this->ScaleFrame->SetLabelText("Slices");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ScaleFrame->GetWidgetName());

  if( !this->SliceScale)
	  this->SliceScale = vtkKWScale::New();
  this->SliceScale->SetParent(this->ScaleFrame->GetFrame());
  this->SliceScale->Create();
  this->SliceScale->SetRange(0, 100);
  this->SliceScale->SetResolution(1);
  this->SliceScale->SetEndCommand(this, "SliderCallback");
  this->GetApplication()->Script(
    "pack %s -side left -anchor nw -expand y -fill x -padx 5 -pady 0", 
    this->SliceScale->GetWidgetName());

  //Select the image from the list
  if ( !this->AxisFrame )
    this->AxisFrame = vtkKWFrameWithLabel::New();
  this->AxisFrame->SetParent( this->MainFrame );
  this->AxisFrame->Create();
  this->AxisFrame->SetLabelText("Select Plane to Segment");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->AxisFrame->GetWidgetName());

  //Axis Radio Buttons
   if( !this->ZAxis)
	  this->ZAxis = vtkKWRadioButton::New();
  this->ZAxis->SetParent(this->AxisFrame->GetFrame());
  this->ZAxis->Create();
  this->ZAxis->SetText("Axial");
  this->ZAxis->SetCommand(this, "AxisCallback");
  this->ZAxis->SetValueAsInt(3);
  this->GetApplication()->Script(
    "pack %s -side left -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->ZAxis->GetWidgetName());
  
  if( !this->YAxis)
	  this->YAxis = vtkKWRadioButton::New();
  this->YAxis->SetParent(this->AxisFrame->GetFrame());
  this->YAxis->Create();
  this->YAxis->SetText("Coronal");
  this->YAxis->SetCommand(this, "AxisCallback");
  this->YAxis->SetValueAsInt(2);
  this->YAxis->SetVariableName(this->ZAxis->GetVariableName());
  this->GetApplication()->Script(
    "pack %s -side left -anchor nw -expand y -padx 40 -pady 6 -fill x", 
    this->YAxis->GetWidgetName());

  if( !this->XAxis)
	  this->XAxis = vtkKWRadioButton::New();
  this->XAxis->SetParent(this->AxisFrame->GetFrame());
  this->XAxis->Create();
  this->XAxis->SetText("Sagittal");
  this->XAxis->SetCommand(this, "AxisCallback");
  this->XAxis->SetValueAsInt(1);
  this->XAxis->SetVariableName(this->ZAxis->GetVariableName());
  this->GetApplication()->Script(
    "pack %s -side right -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->XAxis->GetWidgetName());

  this->ZAxis->SelectedStateOn();
  
  //Add and clear buttons
  if ( !this->AddRemoveButtonFrame )
    this->AddRemoveButtonFrame = vtkKWFrameWithLabel::New();
  this->AddRemoveButtonFrame->SetParent( this->MainFrame );
  this->AddRemoveButtonFrame->SetLabelText("Add / Remove Contour");
  this->AddRemoveButtonFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->AddRemoveButtonFrame->GetWidgetName());

  if(!this->AddContourButton)
	  this->AddContourButton = vtkKWPushButton::New();
  this->AddContourButton->SetParent(this->AddRemoveButtonFrame->GetFrame());
  this->AddContourButton->Create();
  this->AddContourButton->SetText("Add Contour");
  this->AddContourButton->SetCommand(this, "AddContourCallback");
  this->AddContourButton->Script(
    "pack %s -side left -anchor nw -expand n -padx 15 -pady 6 -fill x", 
    this->AddContourButton->GetWidgetName());

  if(!this->RemoveContourButton)
	  this->RemoveContourButton = vtkKWPushButton::New();
  this->RemoveContourButton->SetParent(this->AddRemoveButtonFrame->GetFrame());
  this->RemoveContourButton->Create();
  this->RemoveContourButton->SetText("Remove Slice Contour");
  this->RemoveContourButton->SetCommand(this, "RemoveContoursCallback");
  this->RemoveContourButton->Script(
    "pack %s -side right -anchor nw -expand n -padx 15 -pady 6 -fill x", 
    this->RemoveContourButton->GetWidgetName());
    
  //Copy buttons
  if ( !this->CopyButtonFrame)
    this->CopyButtonFrame = vtkKWFrameWithLabel::New();
  this->CopyButtonFrame->SetParent( this->MainFrame );
  this->CopyButtonFrame->SetLabelText("Copy Contours");
  this->CopyButtonFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->CopyButtonFrame->GetWidgetName());

  if(!this->Copy2PrevContourButton)
	  this->Copy2PrevContourButton = vtkKWPushButton::New();
  this->Copy2PrevContourButton->SetParent(this->CopyButtonFrame->GetFrame());
  this->Copy2PrevContourButton->Create();
  this->Copy2PrevContourButton->SetText("Copy To Prev");
  this->Copy2PrevContourButton->SetCommand(this, "CopyContourCallback -1");
  this->Copy2PrevContourButton->Script(
    "pack %s -side left -anchor nw -expand n -padx 15 -pady 6 -fill x", 
    this->Copy2PrevContourButton->GetWidgetName());

  if(!this->Copy2NextContourButton)
	  this->Copy2NextContourButton = vtkKWPushButton::New();
  this->Copy2NextContourButton->SetParent(this->CopyButtonFrame->GetFrame());
  this->Copy2NextContourButton->Create();
  this->Copy2NextContourButton->SetText("Copy To Next");
  this->Copy2NextContourButton->SetCommand(this, "CopyContourCallback 1");
  this->Copy2NextContourButton->Script(
    "pack %s -side right -anchor nw -expand n -padx 15 -pady 6 -fill x", 
    this->Copy2NextContourButton->GetWidgetName());

  
  // Callbacks for Apply and Cancel Buttons
  this->CancelButton->SetCommand(this, "CancelCallback");
  
  
  this->UpdateObjectLists();
  //this->AddRemoveCallback();
  //this->AxisCallback();
  //this->ResetWidgetCallback();
  
  vtkRenderWindowInteractor *interactor = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor();
  this->DefaultInteractorStyle = interactor->GetInteractorStyle();
  this->DefaultInteractorStyle->Register(this);
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::Update()
{
	this->UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::CancelCallback()
{
  this->UnpackGroup();
	this->MenuGroup->SetMenuButtonsEnabled(1);
	this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);

  //std::cout << "Image: " << this->ActiveActorName << std::endl;
  //std::cout << "Trace: " << this->ActiveTraceName << std::endl;
  
  vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  std::string actorName = name;
  //std::cout << "Actor : " << actorName << std::endl;
  
	if (actorName != "" )
    {
		vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
		const char *name = combobox->GetValue();
		vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
		
		imageActor->DisplayAxisActor(0,1);
		imageActor->DisplayAxisActor(1,1);
		imageActor->DisplayAxisActor(2,1);
		imageActor->SetInteraction(1);
    
    if (this->ActiveTraceName != "")
      {
      this->ResetActiveTrace();
      }
    }
  
  // Exit Tracing Mode Interaction
  this->DisableTracingInteractor();
  
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::ResetActiveTrace()
{
  this->ReplaceInterfaceContourWithEditedPolyData();
  
  this->UpdateSliceContoursCallback(false);
  while (!EditTraceList.empty())
    {
    EditTraceList.pop_front();
    }
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::UpdateImageList()
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
    this->FeatureListComboBox->SetEnabled(1);
    this->NewContourButton->SetEnabled(1);
    this->SliceScale->SetEnabled(1);
    this->ZAxis->SetEnabled(1);
    this->YAxis->SetEnabled(1);
    this->XAxis->SetEnabled(1);
    this->Copy2PrevContourButton->SetEnabled(1);
    this->Copy2NextContourButton->SetEnabled(1);
    this->SetImageCallback( this->ImageList->GetItem(defaultItem)->GetFileName() );
    }
  else
    {
    this->FeatureListComboBox->SetEnabled(0);
    this->NewContourButton->SetEnabled(0);
    this->SliceScale->SetEnabled(0);
    this->ZAxis->SetEnabled(0);
    this->YAxis->SetEnabled(0);
    this->XAxis->SetEnabled(0);
    this->Copy2PrevContourButton->SetEnabled(0);
    this->Copy2NextContourButton->SetEnabled(0);
    }
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::UpdateFeatureList()
{
  this->FeatureListComboBox->GetWidget()->DeleteAllValues();
	
	int tracePlane = this->GetTracePlane( );
  for (int i = 0; i < this->FeatureList->GetNumberOfItems(); i++)
    {
		vtkPolyData *feature = vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(i))->GetDataSet();
    int featurePlane = this->GetFeaturePlane(feature);
    
    //std::cout << "Feature Name : " << this->FeatureList->GetItem(i)->GetFileName() << std::endl;
    //std::cout << "Feature Plane : " << featurePlane << std::endl;
    //std::cout << "Trace Plane : " << tracePlane << std::endl;
    
    if (featurePlane == tracePlane )
      {
      this->FeatureListComboBox->GetWidget()->AddValue(this->FeatureList->GetItem(i)->GetFileName());
      }
    }
	
	//this->FeatureListComboBox->GetWidget()->AddValue("New Feature");
  //this->FeatureListComboBox->GetWidget()->SetValue("New Feature");
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::UpdateObjectLists()
{
	this->UpdateImageList();
  this->UpdateFeatureList();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::SliderCallback(double value)
{
  // Turn OFF display of Contours for previous slice
  this->UpdateSliceContoursCallback(false);
  
  int position;
  
  switch (this->TracingAxis) {
    case 2:
      position = static_cast<int>(this->SliceScale->GetValue());
      this->ImageActorWidgetZ->SetSliceIndex(position);
      //this->ViewCallback(2, false);
      break;
    case 1:
      position = static_cast<int>(this->SliceScale->GetValue());
      this->ImageActorWidgetY->SetSliceIndex(position);
      //this->ViewCallback(1, false);
    case 0:
      position = static_cast<int>(this->SliceScale->GetValue());
      this->ImageActorWidgetX->SetSliceIndex(position);
      //this->ViewCallback(0, false);
    default:
      break;
  }
  
  // Turn ON display of Contours for new slice
  this->UpdateSliceContoursCallback(true);
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
		
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::AxisCallback()
{
  vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  this->ActiveActorName = name;
  
  if (this->ActiveActorName == "" )
    {
    return;
    }
  
  
  // Reset the Active Traces
  this->ResetActiveTrace();
  
  // Update Slider Based on Image Dimensions
  vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(this->ActiveActorName.c_str()));
  int dims[3] = {0,0,0};
  imageActor->GetOrientedDataSet()->GetDimensions(dims);
  
  //std::cout << "Image DIms : " << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;
  
  int xpos = this->ImageActorWidgetX->GetSliceIndex();
  int ypos = this->ImageActorWidgetY->GetSliceIndex();
  int zpos = this->ImageActorWidgetZ->GetSliceIndex();
  //std::cout << "Slice Pos : " << xpos << " " << ypos << " " << zpos << std::endl;
  
  if (this->ZAxis->GetSelectedState() )
		{
    //std::cout << "Axial" << std::endl;
    //Update Actors
    //imageActor->SetInteraction(1);
    imageActor->DisplayAxisActor(0,0);
    imageActor->DisplayAxisActor(1,0);
    imageActor->DisplayAxisActor(2,1);
    imageActor->SetInteraction(0);
    //std::cout << "Update Actors" << std::endl;
    
    //Update Slices
    this->SliceScale->SetRange(0,static_cast<double>(dims[2]-1));
    this->SliceScale->SetValue(static_cast<double>(zpos));
    this->TracingAxis = 2;
    
    //Update Camera
    //std::cout << "Call ViewCallback" << std::endl;
    this->ViewCallback(2, true);
		}
  else if( this->YAxis->GetSelectedState() )
		{
    //std::cout << "Coronal" << std::endl;
    //Update Actors
    imageActor->DisplayAxisActor(0,0);
    imageActor->DisplayAxisActor(1,1);
    imageActor->DisplayAxisActor(2,0);
    imageActor->SetInteraction(0);
    
    //Update Slices
    this->SliceScale->SetRange(0,static_cast<double>(dims[1]-1));
    this->SliceScale->SetValue(ypos);
    this->TracingAxis = 1;
    
    //Update Camera
    this->ViewCallback(1, true);
		}
  else if ( this->XAxis->GetSelectedState() )
		{
    //std::cout << "Sagittal" << std::endl;
    //Update Actors
    imageActor->DisplayAxisActor(0,1);
    imageActor->DisplayAxisActor(1,0);
    imageActor->DisplayAxisActor(2,0);
    imageActor->SetInteraction(0);
    
    //Update Slices
    this->SliceScale->SetRange(0,static_cast<double>(dims[0]-1));
    this->SliceScale->SetValue(xpos);
    this->TracingAxis = 0;
    
    //Update Camera
    this->ViewCallback(0, true);
		}	

  this->ActiveTraceName = "";
  this->UpdateFeatureList();
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::AddContourCallback()
{
  double center[3] = {0,0,0};
  double sliceLocation;
  int sliceIndex;
  
  switch (this->TracingAxis) {
    case 2:
      sliceIndex = this->ImageActorWidgetZ->GetSliceIndex();
      this->ImageActorWidgetZ->GetCenter(center);
      sliceLocation = center[2];
      break;
    case 1:
      sliceIndex = this->ImageActorWidgetY->GetSliceIndex();
      this->ImageActorWidgetY->GetCenter(center);
      sliceLocation = center[1];
      break;
    case 0:
      sliceIndex = this->ImageActorWidgetX->GetSliceIndex();
      this->ImageActorWidgetX->GetCenter(center);
      sliceLocation = center[0];
      break;
  }
  
  ManualTrace *newTrace = this->CreateNewEmptyContour(this->TracingAxis, sliceIndex, sliceLocation);
  EditTraceList.push_back(newTrace);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();    
}

//----------------------------------------------------------------------------
ManualTrace* vtkKWMimxImageManualSegmentGroup::CreateNewEmptyContour(int axis, int sliceIndex, double sliceLocation)
{
  vtkBoundedPlanePointPlacer *pointPlacer = vtkBoundedPlanePointPlacer::New();
  ManualTrace *newTrace = new ManualTrace;
  
  double *xrange;
  double *yrange;
  double *zrange;
  vtkPlane *plane;
  
  switch ( axis )
  {
    case 2:
      xrange = this->ImageActorWidgetZ->GetTexturePlaneActor()->GetXRange();
      yrange = this->ImageActorWidgetZ->GetTexturePlaneActor()->GetYRange();
      zrange = this->ImageActorWidgetZ->GetTexturePlaneActor()->GetZRange();
      pointPlacer->SetProjectionNormalToZAxis();
      pointPlacer->SetProjectionPosition(zrange[0]);
    
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[0], yrange[0], zrange[0] );
      plane->SetNormal( 1.0, 0.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
    
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[1], yrange[1], zrange[1] );
      plane->SetNormal( -1.0, 0.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
    
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[0], yrange[0], zrange[0] );
      plane->SetNormal( 0.0, 1.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[1], yrange[1], zrange[1] );
      plane->SetNormal( 0.0, -1.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
    
      //std::cout << "X range: " << xrange[0] << " " << xrange[1] << std::endl;
      //std::cout << "Y range: " << yrange[0] << " " << yrange[1] << std::endl;
      //std::cout << "Z range: " << zrange[0] << " " << zrange[1] << std::endl;
    
      newTrace->SetSliceIndex( sliceIndex );
      newTrace->m_ContourWidget->AddPositionInfo(sliceLocation,2);
      newTrace->SetOrientationAxial();
      break;
    case 1:
      xrange = this->ImageActorWidgetY->GetTexturePlaneActor()->GetXRange();
      yrange = this->ImageActorWidgetY->GetTexturePlaneActor()->GetYRange();
      zrange = this->ImageActorWidgetY->GetTexturePlaneActor()->GetZRange();
      pointPlacer->SetProjectionNormalToYAxis();
      pointPlacer->SetProjectionPosition(yrange[0]);
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[0], yrange[0], zrange[0] );
      plane->SetNormal( 1.0, 0.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[1], yrange[1], zrange[1] );
      plane->SetNormal( -1.0, 0.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[0], yrange[0], zrange[0] );
      plane->SetNormal( 0.0, 0.0, 1.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[1], yrange[1], zrange[1] );
      plane->SetNormal( 0.0, 0.0, -1.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      //std::cout << "X range: " << xrange[0] << " " << xrange[1] << std::endl;
      //std::cout << "Y range: " << yrange[0] << " " << yrange[1] << std::endl;
      //std::cout << "Z range: " << zrange[0] << " " << zrange[1] << std::endl;
    
      newTrace->SetSliceIndex( sliceIndex );
      newTrace->m_ContourWidget->AddPositionInfo(sliceLocation,1);
      newTrace->SetOrientationCoronal();
      break;
    case 0:
      xrange = this->ImageActorWidgetX->GetTexturePlaneActor()->GetXRange();
      yrange = this->ImageActorWidgetX->GetTexturePlaneActor()->GetYRange();
      zrange = this->ImageActorWidgetX->GetTexturePlaneActor()->GetZRange();
      pointPlacer->SetProjectionNormalToXAxis();
      pointPlacer->SetProjectionPosition(xrange[0]);
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[0], yrange[0], zrange[0] );
      plane->SetNormal( 0.0, 1.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[1], yrange[1], zrange[1] );
      plane->SetNormal( 0.0, -1.0, 0.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[0], yrange[0], zrange[0] );
      plane->SetNormal( 0.0, 0.0, 1.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      plane = vtkPlane::New();
      plane->SetOrigin( xrange[1], yrange[1], zrange[1] );
      plane->SetNormal( 0.0, 0.0, -1.0 );
      pointPlacer->AddBoundingPlane( plane );
      plane->Delete();
      
      //std::cout << "X range: " << xrange[0] << " " << xrange[1] << std::endl;
      //std::cout << "Y range: " << yrange[0] << " " << yrange[1] << std::endl;
      //std::cout << "Z range: " << zrange[0] << " " << zrange[1] << std::endl;
    
      newTrace->SetSliceIndex( sliceIndex );
      newTrace->m_ContourWidget->AddPositionInfo(sliceLocation,0);
      newTrace->SetOrientationSagittal();
      break;
  }
  
  vtkOrientedGlyphContourRepresentation *glyphContour = vtkOrientedGlyphContourRepresentation::New();
  glyphContour->SetPointPlacer(pointPlacer);
  glyphContour->GetProperty()->SetColor(0,1,0);
  
  vtkRenderWindowInteractor *interactor = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor();
  newTrace->m_ContourWidget->SetRepresentation( glyphContour );
  newTrace->m_ContourWidget->SetInteractor( interactor );
  newTrace->m_ContourWidget->SetManagesCursor( false );
  newTrace->m_ContourWidget->SetEnabled(true);
  newTrace->m_ContourWidget->ProcessEventsOn();
  newTrace->m_ContourWidget->Render();
  
  return newTrace;
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::RemoveContoursCallback()
{
	int slice = static_cast<int>(this->SliceScale->GetValue());
  
  this->UpdateSliceContoursCallback(false);
  
  std::list<ManualTrace*>::iterator prev;
  bool deletePrev = false;
  
  for (std::list<ManualTrace*>::iterator it=EditTraceList.begin(); it != EditTraceList.end(); ++it)
    {
    if (deletePrev)
      {
      prev = EditTraceList.erase(prev);
      }
    if ( (*it)->GetSliceIndex() == slice )
      {
      prev = it;
      deletePrev = true;
      }
    else
      {
      deletePrev = false;
      }
    }
  if (deletePrev)
    {
    prev = EditTraceList.erase(prev);
    }

	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::CopyContourCallback(int shift)
{
  int copyFromSlice = static_cast<int>(this->SliceScale->GetValue());
  int copyToSlice = copyFromSlice + shift;
  
  int dims[3] = {0,0,0};
  vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(ActiveActorName.c_str()));
  imageActor->GetOrientedDataSet()->GetDimensions(dims);
  
  if ((copyToSlice < 0) || (copyToSlice >= dims[this->TracingAxis]))
    {
    return;
    }
  
  // Adjust Slice Slider 
  this->SliceScale->SetValue(static_cast<double>(copyToSlice));
  this->SliderCallback(copyToSlice);
  
  //Copy Slice Contours
  this->CopySliceContours(copyFromSlice, copyToSlice);
    
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::CopySliceContours(int copyFromSlice, int copyToSlice)
{
  double center[3] = {0,0,0};
  double sliceLocation;
  
  //Isolate a given slices polydata
  switch (this->TracingAxis) {
    case 2:
      this->ImageActorWidgetZ->GetCenter(center);
      sliceLocation = center[2];
      break;
    case 1:
      this->ImageActorWidgetY->GetCenter(center);
      sliceLocation = center[1];
      break;
    case 0:
      this->ImageActorWidgetX->GetCenter(center);
      sliceLocation = center[0];
      break;
  }
  
  for (std::list<ManualTrace*>::iterator it=EditTraceList.begin(); it != EditTraceList.end(); ++it)
    {
    if ( (*it)->GetSliceIndex() == copyFromSlice )
      {
      vtkOrientedGlyphContourRepresentation *glyphContour = vtkOrientedGlyphContourRepresentation::SafeDownCast( (*it)->m_ContourWidget->GetRepresentation( ) );
      vtkPolyData *tmpNodes = vtkPolyData::New(); 
      glyphContour->GetNodePolyData(tmpNodes);
      
      ManualTrace *newTrace = this->CreateNewEmptyContour(this->TracingAxis, copyToSlice, sliceLocation);
      
      //std::cout << "Match with # points: " << tmpNodes->GetNumberOfPoints() << std::endl;
      //std::cout << "Center " << center[0] << " " << center[1] << " " << center[2] << std::endl;
      
      for (int i=0;i<tmpNodes->GetNumberOfPoints();i++)
        {
        double point[3];
        tmpNodes->GetPoint(i, point);
        //std::cout << "Orig point: " << point[0] << " " << point[1] << " " << point[2] << std::endl;
        point[this->TracingAxis] = center[this->TracingAxis];
        //std::cout << "New point: " << point[0] << " " << point[1] << " " << point[2] << std::endl;
        tmpNodes->GetPoints()->SetPoint(i,point);
        }
      newTrace->m_ContourWidget->Initialize(tmpNodes);
      EditTraceList.push_back(newTrace);
      }
    }

  this->GetMimxMainWindow()->GetRenderWidget()->Render();

  return;
    
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::ViewCallback(int axis, bool resetCamera)
{
	vtkCamera *camera = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->GetActiveCamera();
	
  switch(axis)
	{
		case 0:
      camera->SetViewUp(0,1,0);
      camera->SetPosition(1,0,0);
			break;
		case 1:
			camera->SetViewUp(0,0,1);
      camera->SetPosition(0,1,0);
			break;
		case 2:
			camera->SetViewUp(0,1,0);
      camera->SetPosition(0,0,1);
			break;
	}
	camera->SetFocalPoint(0.0,0.0,0.0);
	camera->ComputeViewPlaneNormal();
	camera->OrthogonalizeViewUp();
	this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->ResetCamera();    
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::UpdateSliceContoursCallback(bool displayContours)
{
  int axis = ManualTrace::Unknown;
  int sliceIndex = -1;
  
  if ( this->ZAxis->GetSelectedState() )
		{
    axis = ManualTrace::Axial;
    sliceIndex = this->ImageActorWidgetZ->GetSliceIndex();
		}
  else if( this->YAxis->GetSelectedState() )
		{
    axis = ManualTrace::Coronal;
    sliceIndex = this->ImageActorWidgetY->GetSliceIndex();
		}
  else if( this->XAxis->GetSelectedState() )
		{
    axis = ManualTrace::Sagittal;
    sliceIndex = this->ImageActorWidgetX->GetSliceIndex();
		}
  
  if ( displayContours )
    {
    for (std::list<ManualTrace*>::iterator it=EditTraceList.begin(); it != EditTraceList.end(); ++it)
      {
      if ( ((*it)->GetOrientation() == axis) &&
           ((*it)->GetSliceIndex() == sliceIndex))
        {
        //std::cout << "Display contour" << std::endl;
        (*it)->m_ContourWidget->SetEnabled(true);
        (*it)->m_ContourWidget->ProcessEventsOn();
        (*it)->m_ContourWidget->Render();
        }
      }
    }
  else
    {
    for (std::list<ManualTrace*>::iterator it=EditTraceList.begin(); it != EditTraceList.end(); ++it)
      {
      (*it)->m_ContourWidget->SetEnabled(false);
      (*it)->m_ContourWidget->ProcessEventsOn();
      (*it)->m_ContourWidget->Render();
      }
    }

  return;
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::InitializeActorCallback()
{
	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	const char *name = combobox->GetValue();	
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
  //std::cout << "Initialize Editting" << std::endl;
	this->ImageActorWidgetX = imageActor->GetXaxisImageActorWidget();
	this->ImageActorWidgetY = imageActor->GetYaxisImageActorWidget();
	this->ImageActorWidgetZ = imageActor->GetZaxisImageActorWidget();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::SetImageCallback(const char *value)
{
  std::string imageName(value);
  //std::cout << "Set Image " << imageName << std::endl;
  if (imageName == "")
    {
    this->FeatureListComboBox->SetEnabled(0);
    this->NewContourButton->SetEnabled(0);
    this->SliceScale->SetEnabled(0);
    this->ZAxis->SetEnabled(0);
    this->YAxis->SetEnabled(0);
    this->XAxis->SetEnabled(0);
    this->Copy2PrevContourButton->SetEnabled(0);
    this->Copy2NextContourButton->SetEnabled(0);
    }
  else
    {
    this->FeatureListComboBox->SetEnabled(1);
    this->NewContourButton->SetEnabled(1);
    this->SliceScale->SetEnabled(1);
    this->ZAxis->SetEnabled(1);
    this->YAxis->SetEnabled(1);
    this->XAxis->SetEnabled(1);
    this->Copy2PrevContourButton->SetEnabled(1);
    this->Copy2NextContourButton->SetEnabled(1);
    this->ActiveActorName = imageName;
    //std::cout << "Set Active Image " << this->ActiveActorName << std::endl;
    this->InitializeActorCallback();
    //std::cout << "Set Active Image (after Init) " << this->ActiveActorName << std::endl;
    this->AxisCallback();
    //std::cout << "Set Active Image (after Axis) " << this->ActiveActorName << std::endl;
    }
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::NewContourCallback()
{
  // Display dialog to get the new ROI name
  vtkKWSimpleEntryDialog *nameContourDialog = vtkKWSimpleEntryDialog::New();
  nameContourDialog->SetApplication(this->GetApplication());
  nameContourDialog->SetStyleToOkCancel();
  nameContourDialog->Create();
  nameContourDialog->SetTitle("Set New Contour Name");
  nameContourDialog->GetEntry()->SetLabelText("Contour Name: ");
  nameContourDialog->SetDisplayPositionToPointer();
  nameContourDialog->SetModal(1);
  //nameContourDialog->SetIconName(const char *name)
  nameContourDialog->Invoke();
  
  //Only create a new contour if Apply Button is pressed
  if (nameContourDialog->GetStatus() == vtkKWDialog::StatusOK)
    {
    std::string contourName = nameContourDialog->GetEntry()->GetWidget()->GetValue();
    if (contourName == "")
      {
      return;
      }
    
    
    //Duplicate ROI names are not allowed
    for (int i = 0; i < this->FeatureList->GetNumberOfItems(); i++)
      {
      std::string featureName = this->FeatureList->GetItem(i)->GetFileName();
      
      if (contourName == featureName )
        {
        vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
        callback->ErrorMessage("Invalid ROI name. The specified name already exists.");
        return;
        }
      }
    
    this->ResetActiveTrace();
    //std::cout << "New Contour: " << contourName << std::endl;
    int plane = ManualTrace::Unknown;
    
    if ( this->ZAxis->GetSelectedState() )
      plane = ManualTrace::Axial;
    if ( this->YAxis->GetSelectedState() )
      plane = ManualTrace::Coronal;
    if ( this->XAxis->GetSelectedState() )
      plane = ManualTrace::Sagittal;
    
    vtkPolyData *contour = CreateEmptyContourPolyData(plane);
    
    this->AddFeatureToDisplay(contour,"", contourName.c_str(), 1, NULL);
    this->UpdateFeatureList();
    
    this->ActiveTraceName = contourName;
    int index = this->FeatureList->GetNumberOfItems() - 1;
    
    //Reset interface state with new ROI
    this->UpdateSliceContoursCallback(true);
    this->AddContourButton->SetEnabled(1);
    this->RemoveContourButton->SetEnabled(1);
    this->FeatureListComboBox->GetWidget()->SetValue( this->ActiveTraceName.c_str() );
    this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->FeatureList->GetItem(index)->GetActor());
    this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
    
    }
  
  return;
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::SetFeatureCallback(const char *value)
{
  //std::cout << "Set Feature: " << value << std::endl;
  std::string featureName(value);
  
  /* Disable Exisiting Trace - Feed back to GUI */
  int index = 0;

    if (featureName == "")
      {
      return;
      }
    this->ResetActiveTrace();
    
    this->ActiveTraceName = featureName;
    for (int i=0;i<this->FeatureList->GetNumberOfItems();i++)
      {
      std::string currentName = this->FeatureList->GetItem(i)->GetFileName();
      if (featureName == currentName)
        {
        index = i;
        }
      }
  
  //Disable the display of the feature
  vtkMimxFeaturePolyDataActor *featureActor = vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(index));
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(featureActor->GetActor());
  this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  
  //Convert Feature to a trace that the user can edit
  this->ConvertFeatureToTrace(featureActor);
  
  // Update the GUI Buttons and Display Information
  this->UpdateSliceContoursCallback(true);
  this->AddContourButton->SetEnabled(1);
  this->RemoveContourButton->SetEnabled(1);
  this->FeatureListComboBox->GetWidget()->SetValue( this->ActiveTraceName.c_str() );
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->FeatureList->GetItem(index)->GetActor());
  this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
  
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::ConvertFeatureToTrace(vtkMimxFeaturePolyDataActor *actor)
{
  
  //Create Trace from Feature
  vtkPolyData *contourPolyData = actor->GetDataSet();
  vtkIntArray *sliceIndexArray = vtkIntArray::SafeDownCast( contourPolyData->GetCellData()->GetArray("SliceIndex") );
  vtkIntArray *pointNodeArray = vtkIntArray::SafeDownCast( contourPolyData->GetPointData()->GetArray("NodePoints") );
  
  for (int i=0;i<contourPolyData->GetNumberOfCells();i++)
    {
    int sliceIndex = sliceIndexArray->GetValue(i);
    vtkCell *cell = contourPolyData->GetCell(i);
    vtkIdType pointId = cell->GetPointId(0);
    
    //Get Slice location for the current trace
    double point[3];
    contourPolyData->GetPoints()->GetPoint(pointId, point);
    double sliceLocation;
    switch ( this->TracingAxis )
      {
        case 2:
        sliceLocation = point[2];
        break;
        case 1:
        sliceLocation = point[1];
        break;
        case 0:
        sliceLocation = point[0];
        break;
      }
    
    ManualTrace *newTrace = this->CreateNewEmptyContour(this->TracingAxis, sliceIndex, sliceLocation);
    
    std::cout << "CELL INFO " << i << std::endl;
    std::cout << "  Index: " << sliceIndex << " Location: " << sliceLocation << std::endl;
    std::cout << "  # Points " << cell->GetNumberOfPoints() << std::endl;
    
    vtkPolyData *slicePolyData = vtkPolyData::New();
    vtkCellArray *cellLine = vtkCellArray::New();
    vtkIdList *nodeIdList = vtkIdList::New();
    vtkPoints *nodePoints = vtkPoints::New();
    vtkIdType numberOfNodes = 0;
    for (int j=0;j<cell->GetNumberOfPoints();j++)
      {
      pointId = cell->GetPointId(j);
      if ( pointNodeArray->GetValue(pointId) )
        {
        numberOfNodes++;
        }
      }
    cellLine->InsertNextCell( numberOfNodes );
    
    vtkIdType currentId = 0;
    for (int j=0;j<cell->GetNumberOfPoints();j++)
      {
      pointId = cell->GetPointId(j);
      if ( pointNodeArray->GetValue(pointId) )
        {
        nodeIdList->InsertNextId(currentId);
        contourPolyData->GetPoints()->GetPoint(pointId, point);
        std::cout << "     Add Point Id " << j << " " << pointId << " " << currentId;
        std::cout << " " << point[0] << " " << point[1] << " " << point[2] << " " << std::endl;
        if ((pointId != cell->GetPointId(0)) || ( j == 0 ))
          {
          nodePoints->InsertPoint(currentId, point);
          cellLine->InsertCellPoint( currentId );
          currentId++;
          }
        else
          {
          cellLine->InsertCellPoint( 0 );
          //std::cout << "     Add Last Point - Same as first " << std::endl;
          }
        }
      }
    
    slicePolyData->SetPoints( nodePoints );
    slicePolyData->SetLines( cellLine );
    newTrace->m_ContourWidget->Initialize(slicePolyData);
    newTrace->m_ContourWidget->SetEnabled(false);
    newTrace->m_ContourWidget->ProcessEventsOn();
    newTrace->m_ContourWidget->Render();
    EditTraceList.push_back(newTrace);
    
    std::cout << "END INFO" << std::endl;
    }
}

//----------------------------------------------------------------------------
vtkPolyData* vtkKWMimxImageManualSegmentGroup::CreateEmptyContourPolyData(int plane)
{
  vtkPolyData *contour = vtkPolyData::New();
  
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
  contour->GetFieldData()->AddArray( fieldData );
  
  return contour;
}

//----------------------------------------------------------------------------
int vtkKWMimxImageManualSegmentGroup::GetFeaturePlane(vtkPolyData *feature)
{
  int plane = ManualTrace::Unknown;
  
  //std::cout << "Feature: " << feature << std::endl;
  //std::cout << "Field Data: " << feature->GetFieldData() << std::endl;
  //std::cout << "Field Data Type: " << feature->GetFieldData()->GetAbstractArray("FEATURE-PLANE") << std::endl;
  
  vtkStringArray *stringArray = vtkStringArray::SafeDownCast(feature->GetFieldData()->GetAbstractArray("FEATURE-PLANE"));
  //std::cout << "String Array: " << stringArray << std::endl;
  if (stringArray)
    {
    vtkStdString planeStr = stringArray->GetValue(0);
    //std::cout << "Feature Plane : " << planeStr << std::endl;
    
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

//----------------------------------------------------------------------------
int vtkKWMimxImageManualSegmentGroup::GetTracePlane()
{
  int plane = ManualTrace::Unknown;
  
  if (this->ZAxis->GetSelectedState())
    {
    plane = ManualTrace::Axial;
    }
  if (this->YAxis->GetSelectedState())
    {
    plane = ManualTrace::Coronal;
    }
  if (this->XAxis->GetSelectedState())
    {
    plane = ManualTrace::Sagittal;
    }
  
  return plane;
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::ReplaceInterfaceContourWithEditedPolyData()
{
  if (this->ActiveTraceName != "")
    {
    vtkPolyData *contour = vtkPolyData::New();
    this->GetActiveContourPolyData( contour );
    //std::cout << "Replace Contour with points: " << contour->GetNumberOfPoints() << std::endl;
    
    int index = -1;
    
    for (int i = 0; i < this->FeatureList->GetNumberOfItems(); i++)
      {
      std::string currentFeatureName = this->FeatureList->GetItem(i)->GetFileName();
      //std::cout << "Active Contour" << this->ActiveTraceName << std::endl;
      //std::cout << "Current Contour" << currentFeatureName << std::endl;
      
      if (this->ActiveTraceName == currentFeatureName )
        {
        index = i;
        }
      }
    
    if (index == -1)
      {
      return;
      }
    
    //std::cout << "Final Contour with points: " << contour->GetNumberOfPoints() << std::endl;
    
    vtkMimxFeaturePolyDataActor *featureActor = vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(index));
    featureActor->GetDataSet()->DeepCopy( contour );
    this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(featureActor->GetActor());
    //contour->Delete();
    }
  
  this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::GetActiveContourPolyData(vtkPolyData *contour)
{
  contour->Initialize();
  
  int plane = ManualTrace::Unknown;
  
  for (std::list<ManualTrace*>::iterator it=EditTraceList.begin(); it != EditTraceList.end(); ++it)
    {
    vtkOrientedGlyphContourRepresentation *glyphContour = vtkOrientedGlyphContourRepresentation::SafeDownCast( (*it)->m_ContourWidget->GetRepresentation( ) );
    plane = (*it)->GetOrientation();
    
    
    // Get the data defining the entire ROI
    vtkPolyData *currentTrace = glyphContour->GetContourRepresentationAsPolyData();
    vtkIntArray *cellData = vtkIntArray::New();
    cellData->SetNumberOfTuples(1);
    cellData->SetValue(0, (*it)->GetSliceIndex());
    cellData->SetName("SliceIndex");
    currentTrace->GetCellData()->AddArray( cellData );
    cellData->Delete();
    
    // Get the data defining the ROI nodes
    vtkPolyData *tmpNodes = vtkPolyData::New();
    glyphContour->GetNodePolyData(tmpNodes);
    vtkIntArray *pointData = vtkIntArray::New();
    pointData->SetNumberOfTuples( currentTrace->GetNumberOfPoints() );
    //std::cout << "Add Contour with points: " << currentTrace->GetNumberOfPoints() << std::endl;
    for (vtkIdType i=0;i<currentTrace->GetNumberOfPoints();i++)
      {
      pointData->SetValue(i, 0);
      }
    
    for (vtkIdType i=0;i<tmpNodes->GetNumberOfPoints();i++)
      {
      double point[3];
      tmpNodes->GetPoint(i, point);
      vtkIdType nodeIndex = currentTrace->FindPoint(point);
      pointData->SetValue(nodeIndex, 1);
      }
    pointData->SetName("NodePoints");
    currentTrace->GetPointData()->AddArray( pointData );
    pointData->Delete();
    
    vtkAppendPolyData *appendfilter = vtkAppendPolyData::New();
    appendfilter->SetInput( contour );
    appendfilter->AddInput( currentTrace );
    appendfilter->Update();
  
    contour->DeepCopy( appendfilter->GetOutput() );
    appendfilter->Delete();
    
    }
  
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
  contour->GetFieldData()->AddArray( fieldData );
  fieldData->Delete();
  
  //std::cout << "Final Contour with points: " << contour->GetNumberOfPoints() << std::endl;
  
  /*
  vtkPolyDataWriter *contourWriter = vtkPolyDataWriter::New();
  contourWriter->SetFileName("contour.vtk");
  contourWriter->SetInput(contour);
  contourWriter->Update();
   */
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::EnableTracingInteractor( )
{
  //std::cout << "EnableTracingInteractor" << std::endl;
  
  vtkRenderWindowInteractor *interactor = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor();
  interactor->SetInteractorStyle( this->TraceInteractorStyle );
  this->ActiveActorName = "";
  this->ActiveTraceName = "";
  this->AddContourButton->SetEnabled(0);
  this->RemoveContourButton->SetEnabled(0);
}

//----------------------------------------------------------------------------
void vtkKWMimxImageManualSegmentGroup::DisableTracingInteractor( )
{
  vtkRenderWindowInteractor *interactor = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor();
  interactor->SetInteractorStyle( this->DefaultInteractorStyle );
}
