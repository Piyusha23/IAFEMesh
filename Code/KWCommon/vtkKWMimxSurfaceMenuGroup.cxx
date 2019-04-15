/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSurfaceMenuGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:59 $
Version:   $Revision: 1.10 $

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
#include "vtkKWMimxBooleanSurfaceGroup.h"

#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxApplication.h"
#include "vtkKWMimxSaveSurfaceGroup.h"
#include "vtkKWMimxLoadSurfaceGroup.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"

#include "vtkMimxActorBase.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkMimxFeaturePolyDataActor.h"
#include "vtkKWMimxViewProperties.h"
#include "vtkKWMimxDeleteObjectGroup.h"
#include "vtkKWMimxCreateSurfaceFromImageGroup.h"

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
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "vtkKWComboBoxWithLabel.h"

#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSurfaceMenuGroup);
vtkCxxRevisionMacro(vtkKWMimxSurfaceMenuGroup, "$Revision: 1.10 $");

//----------------------------------------------------------------------------
vtkKWMimxSurfaceMenuGroup::vtkKWMimxSurfaceMenuGroup()
{
	this->ObjectMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton = NULL;
	this->TypeMenuButton = NULL;
	this->SurfaceList = vtkLinkedListWrapper::New();
  this->FeatureList = vtkLinkedListWrapper::New();
  this->SaveSurfaceGroup = NULL;
  this->DeleteObjectGroup = NULL;
  this->CreateSurfaceFromImageGroup = NULL;
  this->BooleanSurfaceGroup = NULL;
  
  // To access the procedures in load surface - Used to Load a Scene
  this->LoadSurfaceGroup = vtkKWMimxLoadSurfaceGroup::New();
}

//----------------------------------------------------------------------------
vtkKWMimxSurfaceMenuGroup::~vtkKWMimxSurfaceMenuGroup()
{
	if (this->SurfaceList) {
		for (int i=0; i < this->SurfaceList->GetNumberOfItems(); i++) {
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(i))->Delete();
		}
		this->SurfaceList->Delete();
	}
  
  if (this->FeatureList) {
		for (int i=0; i < this->FeatureList->GetNumberOfItems(); i++) {
			vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(i))->Delete();
		}
		this->FeatureList->Delete();
	}
  
	this->ObjectMenuButton->Delete();
  
  if(this->OperationMenuButton)
    this->OperationMenuButton->Delete();
  if(this->TypeMenuButton)
    this->TypeMenuButton->Delete();
  if(this->SaveSurfaceGroup)
	  this->SaveSurfaceGroup->Delete();
  if(this->LoadSurfaceGroup)
	  this->LoadSurfaceGroup->Delete();
  if(this->DeleteObjectGroup)
	  this->DeleteObjectGroup->Delete();
  if(this->CreateSurfaceFromImageGroup)
	  this->CreateSurfaceFromImageGroup->Delete();
  if(this->BooleanSurfaceGroup)
	  this->BooleanSurfaceGroup->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}
	this->Superclass::CreateWidget();
  
	// add menu button with options for Surfaces
  if(!this->MainFrame)
    this->MainFrame = vtkKWFrame::New();

  this->MainFrame->SetParent(this->GetParent());
  this->MainFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->MainFrame->GetWidgetName());

	// Declare operations menu 
	if(!this->OperationMenuButton)	
		this->OperationMenuButton = vtkKWMenuButtonWithLabel::New();
	this->OperationMenuButton->SetParent(this->MainFrame);
	this->OperationMenuButton->Create();
	this->OperationMenuButton->SetBorderWidth(0);
	this->OperationMenuButton->SetReliefToGroove();
	this->OperationMenuButton->SetPadX(2);
	this->OperationMenuButton->SetPadY(2);
	this->OperationMenuButton->GetWidget()->SetWidth(40);
	this->GetApplication()->Script("pack %s -side top -anchor n -padx 2 -pady 15", 
		this->OperationMenuButton->GetWidgetName());
	this->OperationMenuButton->SetEnabled(1);
	
	// Options for Surfaces
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
    "Create",this, "CreateSurfaceFromImageCallback");
  this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Load",this, "LoadSurfaceCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Delete",this, "DeleteSurfaceCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Save",this, "SaveSurfaceCallback");
	this->OperationMenuButton->GetWidget()->GetMenu()->AddRadioButton(
		"Boolean Operations", this, "BooleanOperationsCallback"); 


	if(!this->TypeMenuButton)	
		this->TypeMenuButton = vtkKWMenuButtonWithLabel::New();
	this->TypeMenuButton->SetParent(this->MainFrame);
	this->TypeMenuButton->Create();

	this->OperationMenuButton->GetWidget()->SetValue( "Load" );	
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::Update()
{
	this->UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::LoadSurfaceCallback()
{
	
  //this->LoadSurfaceGroup = vtkKWMimxLoadSurfaceGroup::New();
  this->LoadSurfaceGroup->SetParent( this->GetParent() );
	this->LoadSurfaceGroup->SetSurfaceList(this->SurfaceList);
	this->LoadSurfaceGroup->SetMenuGroup(this);
	this->LoadSurfaceGroup->SetMimxMainWindow(this->GetMimxMainWindow());
  this->LoadSurfaceGroup->Create();
  //this->LoadSurfaceGroup->Delete();
  //this->LoadSurfaceGroup = NULL;
	
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::SaveSurfaceCallback()
{

  if (!this->SaveSurfaceGroup)
	{
		this->SaveSurfaceGroup = vtkKWMimxSaveSurfaceGroup::New();
		this->SaveSurfaceGroup->SetParent( this->GetParent() );
		this->SaveSurfaceGroup->SetSurfaceList(this->SurfaceList);
		this->SaveSurfaceGroup->SetMenuGroup(this);
		this->SaveSurfaceGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->SaveSurfaceGroup->Create();
	}
	else
	{
		this->SaveSurfaceGroup->UpdateObjectLists();
	}
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
  
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->SaveSurfaceGroup->GetBaseFrame()->GetWidgetName());
		
}

//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::DeleteSurfaceCallback()
{
	if(!this->DeleteObjectGroup)
	{
		this->DeleteObjectGroup = vtkKWMimxDeleteObjectGroup::New();
		this->DeleteObjectGroup->SetParent( this->GetParent() );
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(this->SurfaceList);
		this->DeleteObjectGroup->SetMenuGroup(this);
		this->DeleteObjectGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->DeleteObjectGroup->Create();
		this->TypeMenuButton->GetWidget()->SetValue("");
		this->TypeMenuButton->GetWidget()->GetMenu()->DeleteAllItems();
		this->TypeMenuButton->SetEnabled(0);
	}
	else
  {
		this->DeleteObjectGroup->SetFEMeshList(NULL);
		this->DeleteObjectGroup->SetBBoxList(NULL);
		this->DeleteObjectGroup->SetSurfaceList(this->SurfaceList);
		this->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
	  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->DeleteObjectGroup->GetObjectListComboBox()->SetLabelText("Surface: ");
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x", 
		this->DeleteObjectGroup->GetMainFrame()->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::CreateSurfaceFromImageCallback()
{
	if (!this->CreateSurfaceFromImageGroup)
	{
		this->CreateSurfaceFromImageGroup = vtkKWMimxCreateSurfaceFromImageGroup::New();
		this->CreateSurfaceFromImageGroup->SetApplication(this->GetApplication());
		this->CreateSurfaceFromImageGroup->SetParent( this->GetParent() );
		this->CreateSurfaceFromImageGroup->SetImageList(this->ImageList);
    this->CreateSurfaceFromImageGroup->SetSurfaceList(this->SurfaceList);
		this->CreateSurfaceFromImageGroup->SetMimxMainWindow(this->GetMimxMainWindow());
		this->CreateSurfaceFromImageGroup->SetMenuGroup(this);
		this->CreateSurfaceFromImageGroup->Create();
	}
	else
	{
		this->CreateSurfaceFromImageGroup->UpdateObjectLists();
	}
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);

	this->GetApplication()->Script(
		"pack %s -side top -anchor nw -expand n -padx 0 -pady 2 -fill x", 
		this->CreateSurfaceFromImageGroup->GetBaseFrame()->GetWidgetName());
}
//-----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::BooleanOperationsCallback()
{
	if (!this->BooleanSurfaceGroup)
    {
    this->BooleanSurfaceGroup = vtkKWMimxBooleanSurfaceGroup::New();
		this->BooleanSurfaceGroup->SetParent( this->GetParent() );
		this->BooleanSurfaceGroup->SetImageList(this->ImageList);
		this->BooleanSurfaceGroup->SetMenuGroup(this);
		this->BooleanSurfaceGroup->SetMimxMainWindow(this->GetMimxMainWindow());
    this->BooleanSurfaceGroup->SetUseFrameWithSlider( 0 );
		this->BooleanSurfaceGroup->Create();
    }
	else
    {
		this->BooleanSurfaceGroup->UpdateObjectLists();
    }
  
	this->SetMenuButtonsEnabled(0);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(0);
	this->GetApplication()->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 5 -fill x",
                                 this->BooleanSurfaceGroup->GetMainFrame()->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::UpdateObjectLists()
{
	if(this->SaveSurfaceGroup)
		this->SaveSurfaceGroup->UpdateObjectLists();
	if(this->DeleteObjectGroup)
		this->DeleteObjectGroup->UpdateObjectLists();
	if(this->CreateSurfaceFromImageGroup)
		this->CreateSurfaceFromImageGroup->UpdateObjectLists();
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceMenuGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//---------------------------------------------------------------------------
