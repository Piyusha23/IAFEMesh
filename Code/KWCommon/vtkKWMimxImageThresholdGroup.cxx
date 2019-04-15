/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxImageThresholdGroup.cxx,v $
Language:  C++
Date:      $Date: 2012/12/13 19:42:06 $
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

#include "vtkKWMimxImageThresholdGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxMainMenuGroup.h"
#include "vtkMimxErrorCallback.h"

#include "vtkLinkedListWrapper.h"

#include "vtkActor.h"
#include "vtkMimxBoundingBoxSource.h"
#include "vtkMimxImageActor.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkUnstructuredGrid.h"
#include "vtkRenderer.h"


#include "vtkKWApplication.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWOptions.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWTkUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWPushButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"

#include "vtkKWMimxApplication.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkMimxCreateBuildingBlockFromPickWidget.h"

#include <itkBinaryThresholdImageFilter.h>

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxImageThresholdGroup);
vtkCxxRevisionMacro(vtkKWMimxImageThresholdGroup, "$Revision: 1.2 $");

//----------------------------------------------------------------------------
vtkKWMimxImageThresholdGroup::vtkKWMimxImageThresholdGroup()
{
  this->ObjectListComboBox = NULL;
  this->ComponentFrame = NULL;
  this->EntryFrame = NULL;
  this->LowerThresholdEntry = NULL;
  this->UpperThresholdEntry = NULL;
  this->Count = 1;
}

//----------------------------------------------------------------------------
vtkKWMimxImageThresholdGroup::~vtkKWMimxImageThresholdGroup()
{
  if (this->ObjectListComboBox)
    this->ObjectListComboBox->Delete();
  if (this->ComponentFrame)
    this->ComponentFrame->Delete();   
  if (this->EntryFrame)
    this->EntryFrame->Delete();
  if (this->LowerThresholdEntry)
    this->LowerThresholdEntry->Delete();
  if (this->UpperThresholdEntry)
    this->UpperThresholdEntry->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxImageThresholdGroup::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();
  
  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  //this->MainFrame->SetLabelText("Create Building Block From Bounds");

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 0 -fill x", 
    this->MainFrame->GetWidgetName());

  if (!this->ComponentFrame)	
     this->ComponentFrame = vtkKWFrameWithLabel::New();
  ComponentFrame->SetParent(this->MainFrame);
  ComponentFrame->Create();
  ComponentFrame->SetLabelText("Image");
  ComponentFrame->CollapseFrame( );
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
    this->ComponentFrame->GetWidgetName());
  
  if (!this->ObjectListComboBox)	
     this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();
  ObjectListComboBox->SetParent(this->ComponentFrame->GetFrame());
  ObjectListComboBox->Create();
  ObjectListComboBox->SetLabelText("Image : ");
  ObjectListComboBox->SetLabelWidth( 15 );
  ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

  if (!this->EntryFrame)	
     this->EntryFrame = vtkKWFrame::New();
  this->EntryFrame->SetParent(this->MainFrame);
  this->EntryFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -padx 2 -pady 6 -fill x", 
    this->EntryFrame->GetWidgetName());
  
  if (!this->LowerThresholdEntry)
	  this->LowerThresholdEntry = vtkKWEntryWithLabel::New();
  this->LowerThresholdEntry->SetParent(this->EntryFrame);
  this->LowerThresholdEntry->Create();
  this->LowerThresholdEntry->SetLabelText("Lower Threshold : ");
  this->LowerThresholdEntry->GetWidget()->SetValueAsInt(0);
  this->LowerThresholdEntry->GetWidget()->SetRestrictValueToInteger();
  this->LowerThresholdEntry->SetWidth(8);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor n -padx 2 -pady 2", 
	  this->LowerThresholdEntry->GetWidgetName());
  
  if (!this->UpperThresholdEntry)
	  this->UpperThresholdEntry = vtkKWEntryWithLabel::New();
  this->UpperThresholdEntry->SetParent(this->EntryFrame);
  this->UpperThresholdEntry->Create();
  this->UpperThresholdEntry->SetLabelText("Upper Threshold : ");
  this->UpperThresholdEntry->GetWidget()->SetValueAsInt(1000);
  this->UpperThresholdEntry->GetWidget()->SetRestrictValueToInteger();
  this->UpperThresholdEntry->SetWidth(8);
  this->GetApplication()->Script(
      "pack %s -side top -anchor n -padx 2 -pady 2",
      this->UpperThresholdEntry->GetWidgetName());

  this->ApplyButton->SetParent(this->MainFrame);
  this->ApplyButton->Create();
  this->ApplyButton->SetReliefToFlat();
  this->ApplyButton->SetImageToIcon( this->GetMimxMainWindow()->GetApplyButtonIcon() );
  this->ApplyButton->SetCommand(this, "ThresholdImageApplyCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 5 -pady 6", 
	  this->ApplyButton->GetWidgetName());

  this->CancelButton->SetParent(this->MainFrame);
  this->CancelButton->Create();
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetImageToIcon( this->GetMimxMainWindow()->GetCancelButtonIcon() );
  this->CancelButton->SetCommand(this, "ThresholdImageCancelCallback");
  this->GetApplication()->Script(
    "pack %s -side right -anchor ne -expand y -padx 5 -pady 6", 
    this->CancelButton->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkKWMimxImageThresholdGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxImageThresholdGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxImageThresholdGroup::ThresholdImageApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
  {
		callback->ErrorMessage("Please select an image");
		return 0;
  }
  
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if (num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("An invalid image was selected");
		combobox->SetValue("");
		return 0;
	}
	
	int upperThreshold = this->UpperThresholdEntry->GetWidget()->GetValueAsInt();
	int lowerThreshold = this->LowerThresholdEntry->GetWidget()->GetValueAsInt();
  
  std::cout << "Lower Threshold : " << lowerThreshold << std::endl;
  std::cout << "Upper Threshold : " << upperThreshold << std::endl;
  
  vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));
  
	
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(imageActor->GetITKImage());
  filter->SetOutsideValue(0);
  filter->SetInsideValue(255);
  filter->SetLowerThreshold(static_cast<signed short>(lowerThreshold));
  filter->SetUpperThreshold(static_cast<signed short>(upperThreshold));
  filter->Update();

  typedef itk::Image< signed short,  3 >   ImageType;
  ImageType::Pointer threshimage = filter->GetOutput();
  
  std::cout << "Result Image : " << threshimage << std::endl;
  
  this->ImageList->AppendItem(vtkMimxImageActor::New());
  int item = this->ImageList->GetNumberOfItems()-1;
  this->ImageList->GetItem(item)->SetDataType(ACTOR_IMAGE);
  vtkMimxImageActor *thresholdImageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(item));
  thresholdImageActor->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
  thresholdImageActor->SetITKImage( threshimage );
  
  std::string imagePrefix = "Thresh-";
  std::string foundationName = imageActor->GetFoundationName();
  char buffer[16];
  sprintf(buffer, "%d", static_cast<int>(this->Count));
  std::string imageIndex(buffer);
  std::string imageName = imagePrefix + foundationName + "-" + imageIndex;
  
  this->ImageList->GetItem(item)->SetFoundationName( imageName.c_str() );
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->ImageList->GetItem(item) );
  this->Count++;
  
	return 1;
}
//----------------------------------------------------------------------------
void vtkKWMimxImageThresholdGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxImageThresholdGroup::ThresholdImageCancelCallback()
{
  this->GetApplication()->Script("pack forget %s", this->MainFrame->GetWidgetName() );
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}
//------------------------------------------------------------------------------
void vtkKWMimxImageThresholdGroup::UpdateObjectLists()
{
  this->ObjectListComboBox->GetWidget()->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
  {
    ObjectListComboBox->GetWidget()->AddValue(
      this->ImageList->GetItem(i)->GetFileName());
      
    int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(this->ImageList->GetItem(i)->GetActor());
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
//------------------------------------------------------------------------------

