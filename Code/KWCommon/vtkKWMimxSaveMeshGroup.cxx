/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveMeshGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/16 01:52:31 $
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

#include "vtkKWMimxSaveMeshGroup.h"

/* VTK Headers */
#include <vtkObjectFactory.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>
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
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWRenderWidget.h>

/* MIMX Headers */
#include "vtkMimxErrorCallback.h"
#include "vtkMimxMeshActor.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSaveMeshGroup);
vtkCxxRevisionMacro(vtkKWMimxSaveMeshGroup, "$Revision: 1.3 $");

//----------------------------------------------------------------------------
vtkKWMimxSaveMeshGroup::vtkKWMimxSaveMeshGroup()
{
  this->ObjectListComboBox = NULL;
  this->BinaryCheckButton = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxSaveMeshGroup::~vtkKWMimxSaveMeshGroup()
{
  if (this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if (this->BinaryCheckButton)
	  this->BinaryCheckButton->Delete();
 }
//----------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();

  if (!this->ObjectListComboBox)	
     this->ObjectListComboBox = vtkKWComboBoxWithLabel::New();  
  ObjectListComboBox->SetParent(this->MainFrame);
  ObjectListComboBox->Create();
  ObjectListComboBox->SetLabelText("Mesh : ");
  ObjectListComboBox->SetLabelWidth( 15 );
  ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

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
  this->ApplyButton->SetCommand(this, "SaveMeshApplyCallback");
  this->CancelButton->SetCommand(this, "SaveMeshCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSaveMeshGroup::SaveMeshApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  
	int meshItem = combobox->GetValueIndex(name);
	if ( meshItem < 0 || meshItem >= combobox->GetNumberOfValues())
	{
		callback->ErrorMessage("Please select a mesh to save");;
		return 0;
	}

  vtkUnstructuredGrid *ugrid = vtkMimxMeshActor::SafeDownCast(this->FEMeshList
     ->GetItem(meshItem))->GetDataSet();
  const char *filename = this->FEMeshList->GetItem(meshItem)->GetFileName();
  
  vtkKWLoadSaveDialog *fileBrowserDialog = vtkKWLoadSaveDialog::New() ;
	fileBrowserDialog->SaveDialogOn();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();	
	fileBrowserDialog->SetTitle ("Save Mesh");
	fileBrowserDialog->SetFileTypes ("{{VTK files} {.vtk}} {{VTK XML files} {.vtu}} {{All files} {*.*}}");
	fileBrowserDialog->SetDefaultExtension (".vtk");
	fileBrowserDialog->SetInitialFileName(filename);
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();
	
	int status = 0;
	
	if(fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if(fileBrowserDialog->GetFileName())
		{
			std::string meshFileName = fileBrowserDialog->GetFileName();
			std::string extension = vtksys::SystemTools::LowerCase( 
		    vtksys::SystemTools::GetFilenameLastExtension( meshFileName ) );
		    
			fileBrowserDialog->SaveLastPathToRegistry("LastPath");
			
      bool binary = static_cast<bool>(this->BinaryCheckButton->GetWidget()->GetSelectedState());
      
			if ( extension == ".vtk" )
			{
  			this->WriteUnstructuredGrid(meshFileName.c_str(), ugrid, binary);
  		}
			else
			{
  			this->WriteXMLUnstructuredGrid(meshFileName.c_str(), ugrid, binary);
  		}		
			this->GetMimxMainWindow()->SetStatusText("Saved Mesh");
			status = 1;
		}
	}
	
	fileBrowserDialog->Delete();
	
	return status;
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FEMeshList: " << this->FEMeshList << endl;
  os << indent << "ObjectListComboBox: " << this->ObjectListComboBox << endl;
  os << indent << "BinaryCheckButton: " << this->BinaryCheckButton << endl;
}

//----------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::SaveMeshCancelCallback()
{
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}

//------------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
  
  int defaultItem = -1;
  
  for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
    {
    ObjectListComboBox->GetWidget()->AddValue(
                                              this->FEMeshList->GetItem(i)->GetFileName());
    int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
                                                                                              this->FEMeshList->GetItem(i)->GetActor());
    /* Choose the first Viewed Item */
    if ( (defaultItem == -1) && ( viewedItem ) )
  		{
      defaultItem = i;
  		}
    }
  if (defaultItem != -1)
    {
    ObjectListComboBox->GetWidget()->SetValue(this->FEMeshList->GetItem(defaultItem)->GetFileName());
    }
  
}

//---------------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::WriteUnstructuredGrid(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary )
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
  
	vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
	writer->SetFileName( fileName );
	writer->SetInput(ugrid);
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

//---------------------------------------------------------------------------------
void vtkKWMimxSaveMeshGroup::WriteXMLUnstructuredGrid(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary )
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	callback->SetState(0);
  
	vtkXMLUnstructuredGridWriter *writer = vtkXMLUnstructuredGridWriter::New();
	writer->SetFileName( fileName );
	writer->SetInput(ugrid);
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

