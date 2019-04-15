/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSmoothSurfaceGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/03/16 22:16:55 $
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

#include "vtkKWMimxSmoothSurfaceGroup.h"

#include <vtkActor.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkRenderer.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWRenderWidget.h>

#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSmoothSurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxSmoothSurfaceGroup, "$Revision: 1.1 $");

//----------------------------------------------------------------------------

vtkKWMimxSmoothSurfaceGroup::vtkKWMimxSmoothSurfaceGroup()
{
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
  this->Smooth = NULL;
  this->SelectedSurfaceActor = vtkActor::New();
  this->SelectedSurfaceActor->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper = vtkPolyDataMapper::New();
  this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
  this->Menubutton = NULL;
  this->Entrybutton = NULL;
  this->LSmooth = NULL ;
}

//----------------------------------------------------------------------------
vtkKWMimxSmoothSurfaceGroup::~vtkKWMimxSmoothSurfaceGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->FileBrowserDialog)
     this->FileBrowserDialog->Delete();  
	 this->SelectedSurfaceActor->Delete();
	 this->SelectedSurfaceMapper->Delete();  
  if (this->Menubutton)
	 this->Menubutton->Delete();
  if (this->Smooth)
	 this->Smooth->Delete();
  if(this->Entrybutton)
	 this->Entrybutton->Delete();

  if(this->LSmooth)
	  this->LSmooth->Delete();
 }
//----------------------------------------------------------------------------

void vtkKWMimxSmoothSurfaceGroup::CreateWidget()
{
  if(this->IsCreated())
  {
  vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();

  if (!this->ObjectListComboBox)
    this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  this->ObjectListComboBox->SetParent(this->MainFrame);
  this->ObjectListComboBox->Create();
  this->ObjectListComboBox->SetWidth(20);
  this->ObjectListComboBox->SetLabelText("Surface ");
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->ObjectListComboBox->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 2 -fill x", 
  this->ObjectListComboBox->GetWidgetName());			


  if (!this->Menubutton)
    this->Menubutton = vtkKWMenuButtonWithLabel::New();  
  this->Menubutton->SetParent(this->MainFrame);
  this->Menubutton->Create();
  this->Menubutton->SetWidth(20);
  this->Menubutton->SetLabelText(" Options ");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Laplacian Smoothing",this, "LSmoothSurfaceCallback"); 
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Windowed sinc Smoothing",this, "WSSmoothSurfaceCallback"); 

  this->Menubutton->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 25 -fill x", 
  this->Menubutton->GetWidgetName());		

  
// entry button widget code

  if (!this->Entrybutton)
    this->Entrybutton = vtkKWEntryWithLabel::New();
  this->Entrybutton->SetParent(this->MainFrame);
  this->Entrybutton->Create();
  this->Entrybutton->GetWidget()->SetCommand(this,"Smoothvaluechangedcallback");    
  this->Entrybutton->SetWidth(5); 
  this->Entrybutton->SetLabelText("# Smoothing Iterations");
  this->Entrybutton->SetLabelWidth(30);
  this->Entrybutton->GetWidget()->SetValueAsInt(5);
  this->Entrybutton->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 1 -fill x", 
  this->Entrybutton->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "SaveVTKSurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "SaveVTKSurfaceCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxSmoothSurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSmoothSurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSmoothSurfaceGroup::SaveVTKSurfaceApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return 0;
	}

	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();
	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return 0;
	}

	vtkMimxSurfacePolyDataActor *OriginalActor = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)));

	const char *foundation_name = OriginalActor->GetFoundationName();

	//vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    //->GetItem(combobox->GetValueIndex(name)))->GetDataSet();

	const char * Smooth_filename = Menubutton->GetWidget()->GetValue() ;
	
	vtkPolyData *changed_surface = NULL ;
		
	if(!strcmp(Smooth_filename,"Windowed sinc Smoothing"))
		{
			if(Smooth)
			changed_surface = Smooth->GetOutput();								
		}

	if(!strcmp(Smooth_filename,"Laplacian Smoothing"))
		{
			if(LSmooth)
			changed_surface = LSmooth->GetOutput();								
		}

	if(changed_surface)
		{
			vtkMimxSurfacePolyDataActor *changed_surface_actor = vtkMimxSurfacePolyDataActor::New();
			changed_surface_actor->SetFoundationName(foundation_name);
			changed_surface_actor->GetDataSet()->DeepCopy(changed_surface);
			changed_surface_actor->Modified();
			this->SurfaceList->AppendItem(changed_surface_actor);
			this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(changed_surface_actor->GetActor());			


// added to remove the original surface from the window and unselect the surface from the list-------------

	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

// added to remove the original surface from the window and unselect the surface from the list-------------

		char surface_name[64]; 
		if(!strcmp(Smooth_filename,"Laplacian Smoothing"))
		sprintf(surface_name, "LSmooth-");

		if(!strcmp(Smooth_filename,"Windowed sinc Smoothing"))
		sprintf(surface_name, "WSSmooth-");

		int item = this->SurfaceList->GetNumberOfItems()-1 ;
		this->Count ++ ;
		this->SurfaceList->GetItem(item)->SetObjectName(surface_name,this->Count);
		this->GetMimxMainWindow()->GetRenderWidget()->Render();
		this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();			
		this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem(item) );
		this->UpdateObjectLists();
	}
	
	return 0;
}
//----------------------------------------------------------------------------

void vtkKWMimxSmoothSurfaceGroup::Smoothvaluechangedcallback(const char *pr)
{
	this->Entrybutton->SetEnabled(1);
	if(this->Smooth)
	{	
		Smooth->SetNumberOfIterations(this->Entrybutton->GetWidget()->GetValueAsInt());		
		Smooth->Modified();
		Smooth->Update();		
		this->GetMimxMainWindow()->GetRenderWidget()->Render();	
	}	

	if(this->LSmooth)
	{	
		LSmooth->SetNumberOfIterations(this->Entrybutton->GetWidget()->GetValueAsInt());		
		LSmooth->Modified();
		LSmooth->Update();
		this->GetMimxMainWindow()->GetRenderWidget()->Render();	
	}	
}

void vtkKWMimxSmoothSurfaceGroup::WSSmoothSurfaceCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();     

	if (!this->Smooth)	  
	{
//		Smooth = vtkSmoothPolyDataFilter::New();
		Smooth = vtkWindowedSincPolyDataFilter::New();
	}
	  
	  Smooth->SetInput(polydata);	  
	  Smooth->SetNumberOfIterations(this->Entrybutton->GetWidget()->GetValueAsInt());
	  Smooth->BoundarySmoothingOn();
	  Smooth->SetPassBand(0.05);
	  this->SelectedSurfaceMapper->SetInput(Smooth->GetOutput());
	  Smooth->Update();

// added to remove the original surface from the window and unselect the surface from the list-------------

	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
		this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(vtkMimxSurfacePolyDataActor::SafeDownCast(
			this->SurfaceList->GetItem(surface1Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

// added to remove the original surface from the window and unselect the surface from the list-------------

	  this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	  this->GetMimxMainWindow()->GetRenderWidget()->Render();
	  return ;
	  this->Entrybutton->SetEnabled(0);
}

void vtkKWMimxSmoothSurfaceGroup::LSmoothSurfaceCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Surface selection required");;
		return ;
	}
	vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
    const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return ;
	}

	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList
    ->GetItem(combobox->GetValueIndex(name)))->GetDataSet();     

	if (!this->LSmooth)	  
	{
		LSmooth = vtkSmoothPolyDataFilter::New();
//		Smooth = vtkWindowedSincPolyDataFilter::New();
	}
	  
	  LSmooth->SetInput(polydata);	  
	  LSmooth->SetNumberOfIterations(this->Entrybutton->GetWidget()->GetValueAsInt());
	  LSmooth->BoundarySmoothingOn();
	  LSmooth->SetRelaxationFactor(0.025);
	  this->SelectedSurfaceMapper->SetInput(LSmooth->GetOutput());
	  LSmooth->Update();

// added to remove the original surface from the window and unselect the surface from the list-------------

	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}

// added to remove the original surface from the window and unselect the surface from the list-------------

	  this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	  this->GetMimxMainWindow()->GetRenderWidget()->Render();
	  return ;
	  this->Entrybutton->SetEnabled(0);
}


//----------------------------------------------------------------------------
void vtkKWMimxSmoothSurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "ObjectListComboBox: " << this->ObjectListComboBox << endl;
  os << indent << "FileBrowserDialog: " << this->FileBrowserDialog << endl;
  os << indent << "Smooth: " << this->Smooth << endl;
  os << indent << "Menubutton: " << this->Menubutton << endl;
  os << indent << "Entrybutton: " << this->Entrybutton << endl;
  os << indent << "LSmooth: " << this->LSmooth << endl;
  
  os << indent << "SelectedSurfaceActor: " << this->SelectedSurfaceActor << endl;
  os << indent << "SelectedSurfaceMapper: " << this->SelectedSurfaceMapper << endl;
}
//----------------------------------------------------------------------------
void vtkKWMimxSmoothSurfaceGroup::SaveVTKSurfaceCancelCallback()
{
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
  this->GetMimxMainWindow()->GetRenderWidget()->Render();

// added to add the existing surfaces back to the renderer----------------------------------------------

	int surface1Index = -1;
	vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	if ( surface1Index != -1 )
	{
	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(
		vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(surface1Index))->GetActor());
  	this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	}
// added to add the existing surfaces back to the renderer----------------------------------------------

	Menubutton->GetWidget()->SetValue("");

}
//------------------------------------------------------------------------------
void vtkKWMimxSmoothSurfaceGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->SurfaceList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox->GetWidget()->AddValue(
		this->SurfaceList->GetItem(i)->GetFileName());			
		int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
			vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(i))->GetActor());

		if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
	}
	
		if (defaultItem != -1)
  {
		ObjectListComboBox->GetWidget()->SetValue(
		this->SurfaceList->GetItem(defaultItem)->GetFileName());
  }	
}
//--------------------------------------------------------------------------------
void vtkKWMimxSmoothSurfaceGroup::SaveVTKSurfaceDoneCallback()
{
	if(this->SaveVTKSurfaceApplyCallback())
	this->SaveVTKSurfaceCancelCallback();	
}
//---------------------------------------------------------------------------------
