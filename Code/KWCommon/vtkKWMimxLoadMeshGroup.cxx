/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadMeshGroup.cxx,v $
Language:  C++
Date:      $Date: 2012/12/11 22:25:36 $
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

#include "vtkKWMimxLoadMeshGroup.h"

#include <vtkActor.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkObjectFactory.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

#include "vtkKWFileBrowserDialog.h"
#include "vtkKWRenderWidget.h"

#include "vtkMimxMeshActor.h"
#include "vtkMimxErrorCallback.h"
//#include "vtkMimxProgressBarCallback.h"
#include "vtkKWMimxApplication.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxLoadMeshGroup);
vtkCxxRevisionMacro(vtkKWMimxLoadMeshGroup, "$Revision: 1.2 $");


//----------------------------------------------------------------------------
vtkKWMimxLoadMeshGroup::vtkKWMimxLoadMeshGroup()
{
  
}

//----------------------------------------------------------------------------
vtkKWMimxLoadMeshGroup::~vtkKWMimxLoadMeshGroup()
{
  
}

//----------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::CreateWidget()
{
  vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
	fileBrowserDialog->SetTitle ("Load Mesh");
	fileBrowserDialog->SetFileTypes("{{VTK files} {.vtk}} {{VTK XML files} {.vtu}} {{All files} {*.*}}");
	fileBrowserDialog->SetDefaultExtension(".vtk");
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
  fileBrowserDialog->SetMultipleSelection(1);
	fileBrowserDialog->Invoke();
	
  
	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		int numSelections = fileBrowserDialog->GetNumberOfFileNames();
		for (int i=0; i<numSelections; i++)
		{
      vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
      callback->SetState(0);

      if(!fileBrowserDialog->GetFileName())
      {
        callback->ErrorMessage("File name not chosen");
        return;
      }

      std::string meshFileName = vtksys::SystemTools::CollapseFullPath( fileBrowserDialog->GetNthFileName(i) );
      std::string meshDirectory = vtksys::SystemTools::GetFilenamePath( meshFileName );
      
	  this->LoadMesh(meshFileName.c_str());
      
	  this->GetApplication()->SetRegistryValue(
		  1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( meshFileName ).c_str());
      fileBrowserDialog->SaveLastPathToRegistry("LastPath");
    //this->GetMimxMainWindow()->InitializeWorkingDirectory( meshDirectory.c_str() );
      this->UpdateObjectLists();
    }

    this->GetMimxMainWindow()->SetStatusText("Loaded Mesh(s)");
      
	}
  fileBrowserDialog->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::LoadMesh(const char *FilePath)
{
	std::string extension = vtksys::SystemTools::LowerCase( 
		vtksys::SystemTools::GetFilenameLastExtension( FilePath ) );

	if (extension == ".vtk")
	{
		this->LoadVtkMesh(FilePath);
	}
	else
	{
		this->LoadVtkXMLMesh(FilePath);
	}
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::UpdateObjectLists()
{
	
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::LoadVtkMesh(const char *meshFileName)
{

	//vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  callback->SetState(0);
  
	vtkUnstructuredGridReader *vtkReader = vtkUnstructuredGridReader::New();
  vtkReader->SetFileName( meshFileName );
  vtkReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //vtkReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  vtkReader->Update();
  
  if(!callback->GetState())
  {
    vtkMimxMeshActor *meshActor = vtkMimxMeshActor::New();
    this->FEMeshList->AppendItem(meshActor);
    meshActor->SetDataSet(vtkReader->GetOutput());
    meshActor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
    meshActor->SetInteractor( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor() );
    
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
    int item = this->FEMeshList->GetNumberOfItems()-1;
    this->FEMeshList->GetItem(item)->SetFilePath(meshFileName);
    this->FEMeshList->GetItem(item)->SetFoundationName(this->ExtractFileName(meshFileName));
    this->UpdateObjectLists();
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(this->FEMeshList->GetItem(item));
    
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string meshDirectory = vtksys::SystemTools::GetFilenamePath( meshFileName );
    app->InitializeWorkingDirectory( meshDirectory.c_str() ); 
    
    this->GetMimxMainWindow()->SetStatusText("Loaded VTK Mesh");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested mesh.");
  }
  vtkReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}

//------------------------------------------------------------------------------
void vtkKWMimxLoadMeshGroup::LoadVtkXMLMesh(const char *meshFileName)
{
 
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  callback->SetState(0);
  
	vtkXMLUnstructuredGridReader *xmlReader = vtkXMLUnstructuredGridReader::New();
  xmlReader->SetFileName( meshFileName );
  xmlReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //xmlReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  xmlReader->Update();
  
  if(!callback->GetState())
  {
    vtkMimxMeshActor *meshActor = vtkMimxMeshActor::New();
    this->FEMeshList->AppendItem(meshActor);
    meshActor->SetDataSet(xmlReader->GetOutput());
    meshActor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
    meshActor->SetInteractor( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor() );
    
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
    int item = this->FEMeshList->GetNumberOfItems()-1;
    this->FEMeshList->GetItem(item)->SetFilePath(meshFileName);
    this->FEMeshList->GetItem(item)->SetFoundationName(this->ExtractFileName(meshFileName));
    this->UpdateObjectLists();
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(this->FEMeshList->GetItem(item));
    
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string meshDirectory = vtksys::SystemTools::GetFilenamePath( meshFileName );
    app->InitializeWorkingDirectory( meshDirectory.c_str() ); 
    
    this->GetMimxMainWindow()->SetStatusText("Loaded VTK Mesh");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested mesh.");
  }
  xmlReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}
//---------------------------------------------------------------------------------
