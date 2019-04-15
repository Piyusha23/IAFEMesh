/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSurfaceDensityGroup.cxx,v $
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

#include "vtkKWMimxSurfaceDensityGroup.h"

//VTK Headers
#include <vtkActor.h>
#include <vtkCleanPolyData.h>
#include <vtkDecimatePro.h>
#include <vtkFieldData.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkTriangleFilter.h>
#include <vtkRenderer.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//KWWidget Headers
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWPushButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWRenderWidget.h>

//MIMX Headers
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSurfaceDensityGroup);
vtkCxxRevisionMacro(vtkKWMimxSurfaceDensityGroup, "$Revision: 1.1 $");


//----------------------------------------------------------------------------

vtkKWMimxSurfaceDensityGroup::vtkKWMimxSurfaceDensityGroup()
{
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
  this->Decimate = NULL;
  this->LoopSubdivide = NULL;
  this->SelectedSurfaceActor = vtkActor::New();
  this->SelectedSurfaceActor->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper = vtkPolyDataMapper::New();
  this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
  this->Menubutton = NULL;
  this->Entrybutton = NULL;
  this->Entrybutton_InitTri = NULL;
  this->Entrybutton_FinaTri = NULL;
  this->Entrybutton1 = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxSurfaceDensityGroup::~vtkKWMimxSurfaceDensityGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->FileBrowserDialog)
     this->FileBrowserDialog->Delete();  
	 this->SelectedSurfaceActor->Delete();
	 this->SelectedSurfaceMapper->Delete();  
  if (this->Menubutton)
	 this->Menubutton->Delete();
  if (this->Decimate)
	 this->Decimate->Delete();
  if(this->LoopSubdivide)
	 this->LoopSubdivide->Delete();
  if(this->Entrybutton)
	 this->Entrybutton->Delete();
  if(this->Entrybutton_InitTri)
	  this->Entrybutton_InitTri->Delete();
  if(this->Entrybutton_FinaTri)
	  this->Entrybutton_FinaTri->Delete();
  if(this->Entrybutton1)
	  this->Entrybutton1->Delete();
 }
//----------------------------------------------------------------------------

void vtkKWMimxSurfaceDensityGroup::CreateWidget()
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
		"Decimate",this, "DecimateSurfaceCallback");
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Subdivide",this, "LoopSubdivisionSurfaceCallback");
  this->Menubutton->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 25 -fill x", 
  this->Menubutton->GetWidgetName());		

  
  // Entry for % Decimation
  if (!this->Entrybutton)
    this->Entrybutton = vtkKWEntryWithLabel::New();
  this->Entrybutton->SetParent(this->MainFrame);
  this->Entrybutton->Create();
  this->Entrybutton->GetWidget()->SetCommand(this,"Decimatevaluechangedcallback");    
  this->Entrybutton->SetWidth(5); 
  this->Entrybutton->SetLabelText("% Reduction  : ");
  this->Entrybutton->SetLabelWidth(30);
  this->Entrybutton->GetWidget()->SetValueAsDouble(0);
  this->Entrybutton->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 1 -fill x", 
  this->Entrybutton->GetWidgetName());
  
  // Entry for No of subdivisions
  if (!this->Entrybutton1)
    this->Entrybutton1 = vtkKWEntryWithLabel::New();
  this->Entrybutton1->SetParent(this->MainFrame);
  this->Entrybutton1->Create();
  this->Entrybutton1->GetWidget()->SetCommand(this,"Subdivisionvaluechangedcallback");    
  this->Entrybutton1->SetWidth(5); 
  this->Entrybutton1->SetLabelText(" Num. Divisions : ");
  this->Entrybutton1->SetLabelWidth(30);
  this->Entrybutton1->GetWidget()->SetValueAsDouble(1);
  this->Entrybutton1->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 1 -fill x", 
  this->Entrybutton1->GetWidgetName());


  // Initial no of cells on surface
  if (!this->Entrybutton_InitTri)
    this->Entrybutton_InitTri = vtkKWEntryWithLabel::New();
  this->Entrybutton_InitTri->SetParent(this->MainFrame);
  this->Entrybutton_InitTri->Create();
//  this->Entrybutton_InitTri->GetWidget()->SetCommand(this,"Decimatevaluechangedcallback");    
  this->Entrybutton_InitTri->SetWidth(5); 
  this->Entrybutton_InitTri->SetLabelText("Initial No of Cells:");
  this->Entrybutton_InitTri->SetLabelWidth(30);  
  this->Entrybutton_InitTri->GetWidget()->SetStateToReadOnly();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 1 -fill x", 
  this->Entrybutton_InitTri->GetWidgetName());

// Final no of cells on surface
  if (!this->Entrybutton_FinaTri)
    this->Entrybutton_FinaTri = vtkKWEntryWithLabel::New();
  this->Entrybutton_FinaTri->SetParent(this->MainFrame);
  this->Entrybutton_FinaTri->Create();
//  this->Entrybutton_FinaTri->GetWidget()->SetCommand(this,"Decimatevaluechangedcallback");    
  this->Entrybutton_FinaTri->SetWidth(5); 
  this->Entrybutton_FinaTri->SetLabelText("Final No of Cells:");
  this->Entrybutton_FinaTri->SetLabelWidth(30);  
  this->Entrybutton_FinaTri->GetWidget()->SetStateToReadOnly();
  this->GetApplication()->Script(
   "pack %s -side top -anchor nw -expand n -padx 2 -pady 1 -fill x", 
  this->Entrybutton_FinaTri->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "Dec_SubSurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "Dec_SubSurfaceCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceDensityGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSurfaceDensityGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSurfaceDensityGroup::Dec_SubSurfaceApplyCallback()
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

	const char * decimate_filename = Menubutton->GetWidget()->GetValue() ;
	
	vtkPolyData *changed_surface = NULL ;
		
	if(!strcmp(decimate_filename,"Decimate"))
		{
			if(Decimate)
			changed_surface = Decimate->GetOutput();
			vtkCleanPolyData *cleanPoly = vtkCleanPolyData::New();
			cleanPoly->SetInput(changed_surface);
			cleanPoly->SetPointMerging(1);
			cleanPoly->SetTolerance(0.0);
			cleanPoly->Update();
			// to remove node and element set names.
			cleanPoly->GetOutput()->GetFieldData()->RemoveArray("Element_Set_Names");
			cleanPoly->GetOutput()->GetFieldData()->RemoveArray("Node_Set_Names");
			vtkTriangleFilter *triFil = vtkTriangleFilter::New();
			triFil->SetInput(cleanPoly->GetOutput());
			triFil->SetPassLines(0);
			triFil->SetPassVerts(0);
			triFil->Update();
			this->AddSurfaceToDisplay(triFil->GetOutput(), "Decimate", foundation_name, 1, NULL);
			cleanPoly->Delete();
			triFil->Delete();
		}

	if(!strcmp(decimate_filename,"Subdivide"))
		{
			if(LoopSubdivide)
			changed_surface = LoopSubdivide->GetOutput();
			// to remove node and element set names.
			changed_surface->GetFieldData()->RemoveArray("Element_Set_Names");
			changed_surface->GetFieldData()->RemoveArray("Node_Set_Names");
			vtkTriangleFilter *triFil = vtkTriangleFilter::New();
			triFil->SetInput(changed_surface);
			triFil->SetPassLines(0);
			triFil->SetPassVerts(0);
			triFil->Update();
			this->AddSurfaceToDisplay(triFil->GetOutput(), "SubDivide", foundation_name, 1, NULL);
			triFil->Delete();
		}

	if(changed_surface)
		{
	//		vtkMimxSurfacePolyDataActor *changed_surface_actor = vtkMimxSurfacePolyDataActor::New();

	//		changed_surface_actor->SetFoundationName(foundation_name);

	//		changed_surface_actor->GetDataSet()->DeepCopy(changed_surface);
	//		changed_surface_actor->Modified();
	//		this->SurfaceList->AppendItem(changed_surface_actor);
	//		this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(changed_surface_actor->GetActor());			


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
	//		char surface_name[64]; 

	//		if(!strcmp(decimate_filename,"Decimate"))
	//		sprintf(surface_name, "Decimate-");

	//		if(!strcmp(decimate_filename,"Subdivide"))
	//		sprintf(surface_name, "Subdivide-");

	//		int item = this->SurfaceList->GetNumberOfItems()-1 ;
	//		this->Count ++ ;
	//		this->SurfaceList->GetItem(item)->SetObjectName(surface_name,this->Count);
	//		this->GetMimxMainWindow()->GetRenderWidget()->Render();
	//		this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();			
	//		this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem(item) );
	//		this->UpdateObjectLists();
			
	}
	
	return 0;
}
//----------------------------------------------------------------------------
// Callback  for changing the % decimation
void vtkKWMimxSurfaceDensityGroup::Decimatevaluechangedcallback(const char *pr)
{
	this->Entrybutton->SetEnabled(1);
	this->Entrybutton1->SetEnabled(0);

	if(this->Decimate)
	{	
			Decimate->SetTargetReduction(0.01* (this->Entrybutton->GetWidget()->GetValueAsDouble()));
			Decimate->Modified();
			Decimate->Update();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();	
			this->Entrybutton_FinaTri->GetWidget()->SetValueAsDouble(Decimate->GetOutput()->GetNumberOfCells());
	}	
}

// Callback for changing the no of subdivisions
void vtkKWMimxSurfaceDensityGroup::Subdivisionvaluechangedcallback(const char *pr)
{
			this->Entrybutton->SetEnabled(0);
			this->Entrybutton1->SetEnabled(1);

	if(this->LoopSubdivide)
	{	
			LoopSubdivide->SetNumberOfSubdivisions(this->Entrybutton1->GetWidget()->GetValueAsDouble());
			LoopSubdivide->Modified();
			LoopSubdivide->Update();
			this->GetMimxMainWindow()->GetRenderWidget()->Render();	
			this->Entrybutton_FinaTri->GetWidget()->SetValueAsDouble(LoopSubdivide->GetOutput()->GetNumberOfCells());
	}	
}

// Decimate surface callback function
void vtkKWMimxSurfaceDensityGroup::DecimateSurfaceCallback()
{
			this->Entrybutton->SetEnabled(1);
			this->Entrybutton1->SetEnabled(0);
	
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

    polydata->GetNumberOfCells();
	this->Entrybutton_InitTri->GetWidget()->SetValueAsDouble(polydata->GetNumberOfCells());

	if (!this->Decimate)	  
	{
		Decimate = vtkDecimatePro::New();	 
		Decimate->SetBoundaryVertexDeletion(0);
	}
	  
	  Decimate->SetInput(polydata);	  
	  Decimate->SetTargetReduction(0.01* (this->Entrybutton->GetWidget()->GetValueAsDouble())); 
	  Decimate->SetPreserveTopology(1);
	  this->SelectedSurfaceMapper->SetInput(Decimate->GetOutput());
	  Decimate->Update();
	  Decimate->GetOutput()->GetNumberOfCells();	  
	  this->Entrybutton_FinaTri->GetWidget()->SetValueAsDouble(Decimate->GetOutput()->GetNumberOfCells());	


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


	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;

	this->Entrybutton->SetEnabled(0);
}

//----------------------------------------------------------------------------
// Loopsubdivision surface callback function

void vtkKWMimxSurfaceDensityGroup::LoopSubdivisionSurfaceCallback()
{
	this->Entrybutton->SetEnabled(0);
	this->Entrybutton1->SetEnabled(1);

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

	polydata->GetNumberOfCells();
	this->Entrybutton_InitTri->GetWidget()->SetValueAsDouble(polydata->GetNumberOfCells());
	
	if(!this->LoopSubdivide)	
	  LoopSubdivide = vtkLinearSubdivisionFilter::New();
	  LoopSubdivide->SetInput(polydata);
	  LoopSubdivide->SetNumberOfSubdivisions(this->Entrybutton1->GetWidget()->GetValueAsDouble());
	  this->SelectedSurfaceMapper->SetInput(LoopSubdivide->GetOutput());
      LoopSubdivide->Update();
	  this->Entrybutton_FinaTri->GetWidget()->SetValueAsDouble(LoopSubdivide->GetOutput()->GetNumberOfCells());	

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

	this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(this->SelectedSurfaceActor);
	this->GetMimxMainWindow()->GetRenderWidget()->Render();
	return ;
}
//----------------------------------------------------------------
void vtkKWMimxSurfaceDensityGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "ObjectListComboBox: " << this->ObjectListComboBox << endl;
  os << indent << "FileBrowserDialog: " << this->FileBrowserDialog << endl;
  os << indent << "Decimate: " << this->Decimate << endl;
  os << indent << "LoopSubdivide: " << this->LoopSubdivide << endl;
  os << indent << "Menubutton: " << this->Menubutton << endl;
  os << indent << "Entrybutton: " << this->Entrybutton << std::endl;
  os << indent << "Entrybutton_InitTri: " << this->ObjectListComboBox << endl;
  os << indent << "Entrybutton_FinaTri: " << this->Entrybutton_FinaTri << endl;
  os << indent << "Entrybutton1: " << this->Entrybutton1 << endl;
  
  os << indent << "SelectedSurfaceActor: " << this->SelectedSurfaceActor << endl;
  os << indent << "SelectedSurfaceMapper: " << this->SelectedSurfaceMapper << endl;
}
//----------------------------------------------------------------------------
void vtkKWMimxSurfaceDensityGroup::Dec_SubSurfaceCancelCallback()
{
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
  this->GetMimxMainWindow()->GetRenderWidget()->Render();

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

  this->Entrybutton->GetWidget()->SetValueAsDouble(0);
  this->Entrybutton1->GetWidget()->SetValueAsDouble(1);
    Menubutton->GetWidget()->SetValue("");

}
//------------------------------------------------------------------------------
void vtkKWMimxSurfaceDensityGroup::UpdateObjectLists()
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
void vtkKWMimxSurfaceDensityGroup::SaveVTKSurfaceDoneCallback()
{
	if(this->Dec_SubSurfaceApplyCallback())
	this->Dec_SubSurfaceCancelCallback();	
}
//--------------------------------------------------------------------------------



