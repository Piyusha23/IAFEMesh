/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadBBGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/01 21:45:59 $
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

#include "vtkKWMimxLoadBBGroup.h"

/* VTK Headers */
#include <vtkObjectFactory.h>
#include <vtkStdString.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkActor.h>
#include <vtkCellTypes.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

/* KWWidgets Headers */
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWRenderWidget.h"

/* MIMX Headers */
#include "vtkMimxErrorCallback.h"
//#include "vtkMimxProgressBarCallback.h"
#include "vtkKWMimxApplication.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxLoadBBGroup);
vtkCxxRevisionMacro(vtkKWMimxLoadBBGroup, "$Revision: 1.3 $");

//----------------------------------------------------------------------------
vtkKWMimxLoadBBGroup::vtkKWMimxLoadBBGroup()
{
  
}

//----------------------------------------------------------------------------
vtkKWMimxLoadBBGroup::~vtkKWMimxLoadBBGroup()
{
  
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::CreateWidget()
{
  vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
	fileBrowserDialog->SetTitle ("Load Building Block");
	fileBrowserDialog->SetFileTypes("{{VTK files} {.vtk}} {{VTK XML files} {.vtu}} {{All files} {*.*}}");
	fileBrowserDialog->SetDefaultExtension(".vtk");
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
  fileBrowserDialog->SetMultipleSelection(1);	
	fileBrowserDialog->Invoke();
	
	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		int numSelections =  fileBrowserDialog->GetNumberOfFileNames();
		for (int i=0; i<numSelections; i++)
		{
      vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
      callback->SetState(0);

      if (!fileBrowserDialog->GetFileName())
      {
        callback->ErrorMessage("File name not chosen");
        return;
      }

      std::string bbFileName = vtksys::SystemTools::CollapseFullPath( fileBrowserDialog->GetNthFileName(i) );
      
      std::string bbDirectory = vtksys::SystemTools::GetFilenamePath( bbFileName );
        
      this->LoadBlock(bbFileName.c_str());
			
      this->GetApplication()->SetRegistryValue(
		  1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( bbFileName ).c_str());
      fileBrowserDialog->SaveLastPathToRegistry("LastPath");
      //this->GetMimxMainWindow()->InitializeWorkingDirectory( bbDirectory.c_str() ); 
      this->UpdateObjectLists();
    }
    
    this->GetMimxMainWindow()->SetStatusText("Loaded Building Block");
  }  
  
  fileBrowserDialog->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::LoadBlock(const char *FilePath)
{
	std::string extension = vtksys::SystemTools::LowerCase( 
		vtksys::SystemTools::GetFilenameLastExtension( FilePath ) );

	if (extension == ".vtk")
	{
		this->LoadVtkBuildingBlock( FilePath );
	}
	else
	{
		this->LoadVtkXMLBuildingBlock( FilePath );
	}
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::LoadVtkBuildingBlock(const char *blockFileName)
{
  
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  callback->SetState(0);
  
	vtkUnstructuredGridReader *vtkReader = vtkUnstructuredGridReader::New();
  vtkReader->SetFileName( blockFileName );
  vtkReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //vtkReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  vtkReader->Update();
  
  if (!callback->GetState())
  {
    if(!this->CheckCellTypesInUgrid(vtkReader->GetOutput()))
    {
      callback->ErrorMessage("File contains elements other than hexahedra");
      vtkReader->Delete();
      return;
    }
  
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( blockFileName );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(blockFileName);
    this->AddBuildingBlockToDisplay(vtkReader->GetOutput(), "", foundationName.c_str(), 1, filePath.c_str() );
  
    /*
    this->BBoxList->AppendItem(vtkMimxUnstructuredGridActor::New());
    
    this->DoUndoTree->AppendItem(new Node);
    int item = this->DoUndoTree->GetNumberOfItems()-1;
    int bbIndex = this->BBoxList->GetNumberOfItems()-1;
    this->DoUndoTree->GetItem(item)->Data = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(bbIndex));
    this->DoUndoTree->GetItem(item)->Child = NULL;
    this->DoUndoTree->GetItem(item)->Parent = NULL;
    this->BBoxList->GetItem(bbIndex)->SetDataType(ACTOR_BUILDING_BLOCK);
  
    vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(bbIndex));
    ugridActor->GetDataSet()->DeepCopy(vtkReader->GetOutput());
    ugridActor->GetDataSet()->Modified();
    this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(ugridActor->GetActor()); 
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
    this->BBoxList->GetItem(bbIndex)->SetFilePath( blockFileName );
    this->BBoxList->GetItem(bbIndex)->SetFoundationName( this->ExtractFileName(blockFileName) );
    this->UpdateObjectLists();
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(this->BBoxList->GetItem(bbIndex));
    */
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( blockFileName );
    app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
    
    this->GetMimxMainWindow()->SetStatusText("Loaded Building Block");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested file.");
  }
  vtkReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}

//------------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::LoadVtkXMLBuildingBlock(const char *blockFileName)
{
  
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  callback->SetState(0);
  
	vtkXMLUnstructuredGridReader *xmlReader = vtkXMLUnstructuredGridReader::New();
  xmlReader->SetFileName( blockFileName );
  xmlReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //xmlReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  xmlReader->Update();
  
  if (!callback->GetState())
  {
    if(!this->CheckCellTypesInUgrid(xmlReader->GetOutput()))
    {
      callback->ErrorMessage("File contains elements other than hexahedra");
      xmlReader->Delete();
      return;
    }
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( blockFileName );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(blockFileName);
    this->AddBuildingBlockToDisplay(xmlReader->GetOutput(), "", foundationName.c_str(), 1, filePath.c_str() );
  
/*
    this->BBoxList->AppendItem(vtkMimxUnstructuredGridActor::New());
    
    this->DoUndoTree->AppendItem(new Node);
    int item = this->DoUndoTree->GetNumberOfItems()-1;
    int bbIndex = this->BBoxList->GetNumberOfItems()-1;
    this->DoUndoTree->GetItem(item)->Data = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(bbIndex));
    this->DoUndoTree->GetItem(item)->Child = NULL;
    this->DoUndoTree->GetItem(item)->Parent = NULL;
    this->BBoxList->GetItem(bbIndex)->SetDataType(ACTOR_BUILDING_BLOCK);
    
    vtkMimxUnstructuredGridActor *ugridActor = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(bbIndex));
    ugridActor->GetDataSet()->DeepCopy(xmlReader->GetOutput());
    ugridActor->GetDataSet()->Modified();
    this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(ugridActor->GetActor()); 
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
    this->BBoxList->GetItem(bbIndex)->SetFilePath( blockFileName );
    this->BBoxList->GetItem(bbIndex)->SetFoundationName( this->ExtractFileName(blockFileName) );
    this->UpdateObjectLists();
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList(this->BBoxList->GetItem(bbIndex));
*/    
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( blockFileName );
    app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
    
    this->GetMimxMainWindow()->SetStatusText("Loaded Building Block");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested file.");
  }
  xmlReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}

//------------------------------------------------------------------------------
void vtkKWMimxLoadBBGroup::UpdateObjectLists()
{
	
}
//------------------------------------------------------------------------------------------
int vtkKWMimxLoadBBGroup::CheckCellTypesInUgrid(vtkUnstructuredGrid *Input)
{
	vtkCellTypes *celltypes = vtkCellTypes::New();
	Input->GetCellTypes(celltypes);
	int i;
	for(i =0; i < celltypes->GetNumberOfTypes(); i++)
	{
		if(celltypes->GetCellType(i) != 12)
		{
			celltypes->Delete();
			return 0;
		}
	}
	celltypes->Delete();
	return 1;
}