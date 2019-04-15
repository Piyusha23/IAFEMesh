/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveFeatureGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:58 $
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

#include "vtkKWMimxSaveFeatureGroup.h"

/* VTK Headers */
#include <vtkCellTypes.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkCommand.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSTLWriter.h>
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

/* MIMX Headers */
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxErrorCallback.h"
#include "vtkMimxFeaturePolyDataActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSaveFeatureGroup);
vtkCxxRevisionMacro(vtkKWMimxSaveFeatureGroup, "$Revision: 1.1 $");
//----------------------------------------------------------------------------
vtkKWMimxSaveFeatureGroup::vtkKWMimxSaveFeatureGroup()
{
  this->ObjectListComboBox = NULL;
  this->BinaryCheckButton = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxSaveFeatureGroup::~vtkKWMimxSaveFeatureGroup()
{
  if (this->ObjectListComboBox)
    this->ObjectListComboBox->Delete();
  if (this->BinaryCheckButton)
    this->BinaryCheckButton->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::CreateWidget()
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
  this->ObjectListComboBox->SetLabelText("Feature : ");
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
  this->ApplyButton->SetCommand(this, "SaveFeatureApplyCallback");
  this->CancelButton->SetCommand(this, "SaveFeatureCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSaveFeatureGroup::SaveFeatureApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();
  
	int item = combobox->GetValueIndex(name);
  if (item < 0 || item > combobox->GetNumberOfValues()-1)
    {
		callback->ErrorMessage("Invalid feature selected");
		combobox->SetValue("");
		return 0;
    }
  
  vtkPolyData *polydata = vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(item))->GetDataSet();
  const char *filename = this->FeatureList->GetItem(item)->GetFileName();
  
  vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
	fileBrowserDialog->SaveDialogOn();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
	fileBrowserDialog->SetTitle ("Save Feature");
	fileBrowserDialog->SetFileTypes ("{{VTK XML files} {.vtp}} {{VTK files} {.vtk}}");
	fileBrowserDialog->SetDefaultExtension (".vtp");
	fileBrowserDialog->SetInitialFileName(filename);
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();
	
	int status = 0;
	
	if(fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if ( fileBrowserDialog->GetFileName() )
		{
			std::string fileName = fileBrowserDialog->GetFileName();
			std::string extension = vtksys::SystemTools::LowerCase( 
		    vtksys::SystemTools::GetFilenameLastExtension( fileName ) );
		                        
			fileBrowserDialog->SaveLastPathToRegistry("LastPath");
			
      bool binary = static_cast<bool>(this->BinaryCheckButton->GetWidget()->GetSelectedState());
			if (extension == ".vtk")
			{
  			SaveVTKFeature(fileName.c_str(), polydata, binary);
			}
			else
			{
  			SaveVTKXMLFeature(fileName.c_str(), polydata, binary);
			}
				
			this->GetMimxMainWindow()->SetStatusText("Saved Feature");
			
			status = 1;
		}
	}
	
	fileBrowserDialog->Delete();
	
	return status;
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::SaveFeatureCancelCallback()
{
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}

//------------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::UpdateObjectLists()
{
	this->UpdateFeatureComboBox( this->ObjectListComboBox->GetWidget() );
}

//--------------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::SaveVTKFeature(const char *fileName, vtkPolyData *polyData, bool binary)
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  
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
  if (callback->GetState())
  {
    std::cerr << "Failed to write the file" << std::endl;
  }
  writer->Delete();	
}

//--------------------------------------------------------------------------------
void vtkKWMimxSaveFeatureGroup::SaveVTKXMLFeature(const char *fileName, vtkPolyData *polyData, bool binary)
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  
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
  if (callback->GetState())
  {
    std::cerr << "Failed to write the file" << std::endl;
  }
  writer->Delete();	
}
//--------------------------------------------------------------------------------
