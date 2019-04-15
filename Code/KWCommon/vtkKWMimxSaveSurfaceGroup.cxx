/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveSurfaceGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/16 01:52:31 $
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

#include "vtkKWMimxSaveSurfaceGroup.h"

/* VTK Headers */
#include <vtkCellTypes.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkCommand.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSTLWriter.h>
#include <vtkRenderer.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

/* KWWidget Headers */
#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWPushButton.h>
#include <vtkObjectFactory.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWRenderWidget.h>

/* MIMX Headers */
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSaveSurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxSaveSurfaceGroup, "$Revision: 1.2 $");
//----------------------------------------------------------------------------
vtkKWMimxSaveSurfaceGroup::vtkKWMimxSaveSurfaceGroup()
{
  this->ObjectListComboBox = NULL;
  this->BinaryCheckButton = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxSaveSurfaceGroup::~vtkKWMimxSaveSurfaceGroup()
{
  if (this->ObjectListComboBox)
    this->ObjectListComboBox->Delete();
  if (this->BinaryCheckButton)
    this->BinaryCheckButton->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::CreateWidget()
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
  this->ObjectListComboBox->SetLabelText("Surface : ");
  this->ObjectListComboBox->SetLabelWidth( 15 );
  this->ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->ObjectListComboBox->GetWidgetName());

  if (!this->BinaryCheckButton)
    this->BinaryCheckButton = vtkKWCheckButtonWithLabel::New();
  this->BinaryCheckButton->SetParent(this->MainFrame);
  this->BinaryCheckButton->Create();
  this->BinaryCheckButton->SetLabelText("Binary Format : ");
  this->BinaryCheckButton->SetLabelWidth( 15 );
  this->BinaryCheckButton->SetBalloonHelpString(
    "Save data using a binary format (may not always support transfer of data across platforms).");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 2",
    this->BinaryCheckButton->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "SaveSurfaceApplyCallback");
  this->CancelButton->SetCommand(this, "SaveSurfaceCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSaveSurfaceGroup::SaveSurfaceApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  
  
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  
	int item = combobox->GetValueIndex(name);
	if(item < 0 || item > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose valid Surface");
		combobox->SetValue("");
		return 0;
	}
  
	vtkPolyData *polydata = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(item))->GetDataSet();
  const char *filename = this->SurfaceList->GetItem(item)->GetFileName();
  
  
  vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
	fileBrowserDialog->SaveDialogOn();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
	fileBrowserDialog->SetTitle ("Save Surface");
	fileBrowserDialog->SetFileTypes ("{{STL files} {.stl}} {{VTK files} {.vtk}} {{VTK XML files} {.vtp}} {{All files} {*.*}}");
  fileBrowserDialog->SetDefaultExtension (".vtk");
	fileBrowserDialog->SetInitialFileName(filename);
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();
	
	int status = 0;
	
	if(fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if ( fileBrowserDialog->GetFileName() )
		{
			std::string surfaceFileName = fileBrowserDialog->GetFileName();
			std::string extension = vtksys::SystemTools::LowerCase( 
		    vtksys::SystemTools::GetFilenameLastExtension( surfaceFileName ) );
		                        
			fileBrowserDialog->SaveLastPathToRegistry("LastPath");
			
      bool binary = static_cast<bool>(this->BinaryCheckButton->GetWidget()->GetSelectedState());
			if (extension == ".stl")
			{
  			SaveSTLSurface(surfaceFileName.c_str(), polydata, binary);
			}
			else if (extension == ".vtk")
			{
  			SaveVTKSurface(surfaceFileName.c_str(), polydata, binary);
			}
			else
			{
  			SaveVTKXMLSurface(surfaceFileName.c_str(), polydata, binary);
			}
				
			this->GetMimxMainWindow()->SetStatusText("Saved Surface");
			
			status = 1;
		}
	}
	
	fileBrowserDialog->Delete();
	
	return status;
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::SaveSurfaceCancelCallback()
{
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}

//------------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->SurfaceList->GetNumberOfItems(); i++)
  {
    ObjectListComboBox->GetWidget()->AddValue(this->SurfaceList->GetItem(i)->GetFileName());
    int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(this->SurfaceList->GetItem(i)->GetActor());
    if ( (defaultItem == -1) && ( viewedItem ) )
    {
      defaultItem = i;
    }
  }
    
  if (defaultItem != -1)
  {
    ObjectListComboBox->GetWidget()->SetValue(this->SurfaceList->GetItem(defaultItem)->GetFileName());
  }
}

//--------------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::SaveSTLSurface(const char *fileName, vtkPolyData *polyData, bool binary)
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  callback->SetState(0);
  
  vtkSTLWriter *writer = vtkSTLWriter::New();
  writer->SetFileName( fileName );
  writer->SetInput( polyData );
  writer->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  if (binary)
  {
	  writer->SetFileTypeToBinary();
  }
  else
  {
    writer->SetFileTypeToASCII();
  }
  writer->Update();
  writer->RemoveObserver(callback);
  writer->Delete();	
}

//--------------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::SaveVTKSurface(const char *fileName, vtkPolyData *polyData, bool binary)
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  callback->SetState(0);
  
  vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
  writer->SetFileName( fileName );
	writer->SetInput( polyData );
	writer->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  if (binary)
  {
	  writer->SetFileTypeToBinary();
  }
  else
  {
    writer->SetFileTypeToASCII();
  }
	writer->Update();
	writer->RemoveObserver(callback);
  writer->Delete();	
}

//--------------------------------------------------------------------------------
void vtkKWMimxSaveSurfaceGroup::SaveVTKXMLSurface(const char *fileName, vtkPolyData *polyData, bool binary)
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  callback->SetState(0);
  
  vtkXMLPolyDataWriter *writer = vtkXMLPolyDataWriter::New();
	writer->SetFileName( fileName );
	writer->SetInput( polyData );
	writer->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  if (binary)
  {
	  writer->SetDataModeToBinary();
  }
  else
  {
    writer->SetDataModeToAscii();
  }
	writer->Update();
	writer->RemoveObserver(callback);
  writer->Delete();	
}
//--------------------------------------------------------------------------------
