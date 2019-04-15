/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxConnectivitySurfaceGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/03/17 00:39:12 $
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

#include "vtkKWMimxConnectivitySurfaceGroup.h"

#include <vtkActor.h>
#include <vtkCleanPolyData.h>
#include <vtkLinkedListWrapper.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWIcon.h>
#include <vtkKWRenderWidget.h>

#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


// define the option types
//#define VTK_KW_OPTION_NONE       0
//#define VTK_KW_OPTION_LOAD		   1

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxConnectivitySurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxConnectivitySurfaceGroup, "$Revision: 1.1 $");

//----------------------------------------------------------------------------

vtkKWMimxConnectivitySurfaceGroup::vtkKWMimxConnectivitySurfaceGroup()
{
  
  this->SelectedSurfaceActor = vtkActor::New();
  this->SelectedSurfaceActor->GetProperty()->SetColor(0,1,0);
  this->SelectedSurfaceMapper = vtkPolyDataMapper::New();
  this->SelectedSurfaceActor->SetMapper(this->SelectedSurfaceMapper);
  
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
  this->Menubutton = NULL;
  this->Connectivity = NULL;
  this->MultiColumnList = NULL ;
  this->ComponentFrame = NULL ;
}

//----------------------------------------------------------------------------
vtkKWMimxConnectivitySurfaceGroup::~vtkKWMimxConnectivitySurfaceGroup()
{
  if (this->SelectedSurfaceActor)
    this->SelectedSurfaceActor->Delete();
  if (this->SelectedSurfaceMapper)
    this->SelectedSurfaceMapper->Delete();  
  if (this->ObjectListComboBox)
    this->ObjectListComboBox->Delete();
  if (this->FileBrowserDialog)
    this->FileBrowserDialog->Delete();  
  if (this->Menubutton)
	 this->Menubutton->Delete();
  if (this->Connectivity)
	  this->Connectivity->Delete();
  if(this->MultiColumnList)
	  this->MultiColumnList->Delete();
  if(this->ComponentFrame)
	  this->ComponentFrame->Delete();

 }
//----------------------------------------------------------------------------

void vtkKWMimxConnectivitySurfaceGroup::CreateWidget()
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
		"Connectivity",this, "ConnectivitySurfaceCallback");   
  this->Menubutton->GetWidget()->GetMenu()->AddRadioButton(
		"Append",this, "AppendSurfaceCallback");   
  this->Menubutton->GetWidget()->SetStateToNormal();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand 0 -padx 2 -pady 25 -fill x", 
  this->Menubutton->GetWidgetName());		


  // for appending different surfaces
  if (!this->ComponentFrame)	
    this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent(this->MainFrame);
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Merge");
  this->ComponentFrame->CollapseFrame( );


  // for merging the meshes
  if (!this->MultiColumnList)
	  this->MultiColumnList = vtkKWMultiColumnListWithScrollbars::New();
  this->MultiColumnList->SetParent(this->ComponentFrame->GetFrame());
  this->MultiColumnList->Create();
  this->MultiColumnList->SetHorizontalScrollbarVisibility(1);
  this->MultiColumnList->SetVerticalScrollbarVisibility(1);
  this->MultiColumnList->SetHeight(3);
  int col_index;
  col_index = MultiColumnList->GetWidget()->AddColumn("Name");
  col_index = MultiColumnList->GetWidget()->AddColumn(NULL);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->GetWidget()->SetColumnLabelImageToPredefinedIcon(col_index, vtkKWIcon::IconPlus);
  this->MultiColumnList->GetWidget()->SetSortArrowVisibility(0);
  this->MultiColumnList->GetWidget()->ColumnSeparatorsVisibilityOff();
  this->MultiColumnList->GetWidget()->SetHeight(3);
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->MultiColumnList->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "ConnectivitySurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "ConnectivitySurfaceCancelCallback");

}
//----------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
int vtkKWMimxConnectivitySurfaceGroup::ConnectivitySurfaceApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if (!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
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

	const char * Smooth_filename = Menubutton->GetWidget()->GetValue() ;
	vtkPolyData *changed_surface = NULL ;
		
	if (!strcmp(Smooth_filename,"Connectivity"))
	{
		if (Connectivity) 
		  changed_surface = Connectivity->GetOutput();				
	}

	if (changed_surface)
	{
	  Connectivity->SetExtractionModeToSpecifiedRegions();

	  for (int i = 0; i<Connectivity->GetNumberOfExtractedRegions(); i++ )
		{
			Connectivity->InitializeSpecifiedRegionList();
			Connectivity->AddSpecifiedRegion(i);
			Connectivity->Modified();
			Connectivity->Update();
			
			vtkCleanPolyData *cleanpoly = vtkCleanPolyData::New();
			cleanpoly->SetInput(Connectivity->GetOutput());
			cleanpoly->Update();

			this->AddSurfaceToDisplay(cleanpoly->GetOutput(), "Conn-", name , 1, NULL);

			cleanpoly->Delete();
		}
	
    // Remove the original surface from the window and unselect the surface from the list

	  int surface1Index = -1;
	  vtkKWComboBox *combobox_surf1 = this->ObjectListComboBox->GetWidget();
    const char *name_surf1 = combobox_surf1->GetValue();  	
	  surface1Index = combobox_surf1->GetValueIndex(name_surf1);

	  if ( surface1Index != -1 )
	  {
  	  this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();	
	    this->GetMimxMainWindow()->GetViewProperties()->UpdateEnableState();
	  }
	}
	
	return 0;
}

void vtkKWMimxConnectivitySurfaceGroup::ConnectivitySurfaceCallback()
{

	this->ComponentFrame->Unpack();

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

	if (!this->Connectivity)	  
	{
		Connectivity = vtkPolyDataConnectivityFilter::New();
	}
	  
  Connectivity->SetInput(polydata);
  Connectivity->SetExtractionModeToAllRegions();
  Connectivity->SetColorRegions(1);	  
  Connectivity->GetNumberOfExtractedRegions();
  Connectivity->Update();
  this->SelectedSurfaceMapper->SetInput(Connectivity->GetOutput());
  this->SelectedSurfaceMapper->ScalarVisibilityOn();
  this->SelectedSurfaceMapper->SetScalarRange(Connectivity->GetOutput()->GetScalarRange());
  Connectivity->Update();
  Connectivity->GetNumberOfExtractedRegions();	  


  // Remove the original surface from the window and unselect the surface from the list
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
  return;
}


//----------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::ConnectivitySurfaceCancelCallback()
{

  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  this->GetMimxMainWindow()->GetRenderWidget()->RemoveViewProp(this->SelectedSurfaceActor);
  this->GetMimxMainWindow()->GetRenderWidget()->Render();

  // Add the existing surfaces back to the renderer
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
}

//------------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::UpdateObjectLists()
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
//void vtkKWMimxConnectivitySurfaceGroup::ConnectivitySurfaceDoneCallback()
//{
//	if (this->ConnectivitySurfaceApplyCallback())
//	  this->ConnectivitySurfaceCancelCallback();	
//}

//---------------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::UpdateMergeFrame()
{
	this->MultiColumnList->GetWidget()->DeleteAllRows();
	for (int i=0; i < this->SurfaceList->GetNumberOfItems(); i++)
	{
		vtkMimxSurfacePolyDataActor *OriginalActor = vtkMimxSurfacePolyDataActor::
			SafeDownCast(this->SurfaceList->GetItem(i));

		this->MultiColumnList->GetWidget()->InsertCellText(i, 0, OriginalActor->GetFileName());
		this->MultiColumnList->GetWidget()->InsertCellTextAsInt(i, 1, 0);
		this->MultiColumnList->GetWidget()->SetCellWindowCommandToCheckButton(i, 1);
	}

}

//---------------------------------------------------------------------------------
void vtkKWMimxConnectivitySurfaceGroup::AppendSurfaceCallback()
{

  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x -before %s", 
    this->ComponentFrame->GetWidgetName(), this->ApplyButton->GetWidgetName());

	
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

	this->UpdateMergeFrame();

	if (!this->Connectivity)	  
	{
		Connectivity = vtkPolyDataConnectivityFilter::New();
	}
	  
  Connectivity->SetInput(polydata);
  Connectivity->SetExtractionModeToAllRegions();
  Connectivity->SetColorRegions(1);	  
  Connectivity->GetNumberOfExtractedRegions();
  Connectivity->Update();
  this->SelectedSurfaceMapper->SetInput(Connectivity->GetOutput());
  this->SelectedSurfaceMapper->ScalarVisibilityOn();
  this->SelectedSurfaceMapper->SetScalarRange(Connectivity->GetOutput()->GetScalarRange());
  Connectivity->Update();
  Connectivity->GetNumberOfExtractedRegions();	  


  // Remove the original surface from the window and unselect the surface from the list
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
  return;

}
