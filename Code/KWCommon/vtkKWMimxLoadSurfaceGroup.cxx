/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadSurfaceGroup.cxx,v $
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

#include "vtkKWMimxLoadSurfaceGroup.h"

#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkStringArray.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

#include <vtkKWLoadSaveDialog.h>
#include <vtkKWRenderWidget.h>
#include <vtkKWMessageDialog.h>
#include <vtkKWPushButton.h>
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxApplication.h"
//#include "vtkMimxProgressBarCallback.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxLoadSurfaceGroup);
vtkCxxRevisionMacro(vtkKWMimxLoadSurfaceGroup, "$Revision: 1.3 $");


//----------------------------------------------------------------------------
vtkKWMimxLoadSurfaceGroup::vtkKWMimxLoadSurfaceGroup()
{
	 
}

//----------------------------------------------------------------------------
vtkKWMimxLoadSurfaceGroup::~vtkKWMimxLoadSurfaceGroup()
{
	
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::CreateWidget()
{
  vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
  fileBrowserDialog->SetApplication(this->GetApplication());
  fileBrowserDialog->Create();
  fileBrowserDialog->SetFileTypes("{{STL files} {.stl}} {{VTK files} {.vtk}} {{VTK XML files} {.vtp}} {{All files} {*.*}}");
  fileBrowserDialog->SetDefaultExtension(".stl");
  fileBrowserDialog->SetMultipleSelection(1);
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();
	
	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
    
    for (int i=0; i<fileBrowserDialog->GetNumberOfFileNames(); i++)
		{
			std::string surfaceFileName = vtksys::SystemTools::CollapseFullPath( fileBrowserDialog->GetNthFileName(i) );
			this->LoadSurface(surfaceFileName.c_str());
			fileBrowserDialog->SaveLastPathToRegistry("LastPath");
	
			this->GetMimxMainWindow()->SetStatusText(surfaceFileName.c_str());

		  this->UpdateObjectLists();
					
		  //std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( surfaceFileName );
		  //this->GetMimxMainWindow()->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
					
		}
    this->GetMimxMainWindow()->SetStatusText("Loaded Surface");
    //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
	}
	fileBrowserDialog->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::LoadSurface(const char *FilePath)
{
	std::string extension = vtksys::SystemTools::LowerCase( 
		vtksys::SystemTools::GetFilenameLastExtension( FilePath ) );

	this->GetMimxMainWindow()->SetStatusText(FilePath);

	if (extension == ".stl")
	{
		this->ReadStlPolyDataFile(FilePath);
	}
	else if (extension == ".vtk")
	{
		this->LoadVtkPolyData(FilePath);
	}
	else
	{
		this->LoadVtkXMLPolyData(FilePath);
	}
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::UpdateObjectLists()
{
	
}

//----------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::ReadStlPolyDataFile(const char* surfaceFile)
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  callback->SetState(0);
  
  vtkSTLReader *stlReader = vtkSTLReader::New();
  stlReader->SetFileName( surfaceFile );
  stlReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //stlReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  stlReader->Update();
 
  if (!callback->GetState())
  {
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( surfaceFile );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(surfaceFile);
    this->AddSurfaceToDisplay(stlReader->GetOutput(), "", foundationName.c_str(), 0, filePath.c_str() );
/*
    this->SurfaceList->AppendItem(vtkMimxSurfacePolyDataActor::New());
    int item = this->SurfaceList->GetNumberOfItems()-1;
    this->SurfaceList->GetItem(item)->SetDataType(ACTOR_POLYDATA_SURFACE);
    vtkMimxSurfacePolyDataActor *actor = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(item));
    actor->GetDataSet()->DeepCopy( stlReader->GetOutput() );
    actor->GetDataSet()->Modified();
    this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();    
    
    this->SurfaceList->GetItem(item)->SetFilePath( surfaceFile );
    this->SurfaceList->GetItem(item)->SetFoundationName( this->ExtractFileName(surfaceFile) );
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem(item) );
    this->UpdateObjectLists();
*/    
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( surfaceFile );
    app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
    this->GetMimxMainWindow()->SetStatusText("Loaded Surface");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested file.");
  }
  stlReader->Delete();
}

//------------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::LoadVtkPolyData(const char *surfaceFile)
{
  
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  
  callback->SetState(0);
  
	vtkPolyDataReader *vtkReader = vtkPolyDataReader::New();
  vtkReader->SetFileName( surfaceFile );
  vtkReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //vtkReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  vtkReader->Update();
  
  if (!callback->GetState())
  {
  
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( surfaceFile );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(surfaceFile);
    this->AddSurfaceToDisplay(vtkReader->GetOutput(), "", foundationName.c_str(), 0, filePath.c_str() );
  
/*  this->SurfaceList->AppendItem(vtkMimxSurfacePolyDataActor::New());
    int item = this->SurfaceList->GetNumberOfItems()-1;
    this->SurfaceList->GetItem(item)->SetDataType(ACTOR_POLYDATA_SURFACE);
    this->SurfaceList->GetItem(item)->SetFilePath( surfaceFile );
    this->SurfaceList->GetItem(item)->SetFoundationName( this->ExtractFileName(surfaceFile) );
    
    vtkMimxSurfacePolyDataActor *actor = vtkMimxSurfacePolyDataActor::SafeDownCast(
                                                                                   this->SurfaceList->GetItem(item));
    actor->GetDataSet()->DeepCopy( vtkReader->GetOutput() );
    actor->GetDataSet()->Modified();
    this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem(item) );
    this->UpdateObjectLists();
*/
    
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( surfaceFile );
    app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
    this->GetMimxMainWindow()->SetStatusText("Loaded Surface");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested file.");
  }
  vtkReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}

//------------------------------------------------------------------------------
void vtkKWMimxLoadSurfaceGroup::LoadVtkXMLPolyData(const char *surfaceFile)
{
  
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  callback->SetState(0);
  
	vtkXMLPolyDataReader *xmlReader = vtkXMLPolyDataReader::New();
  xmlReader->SetFileName( surfaceFile );
  xmlReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //xmlReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  xmlReader->Update();
  
  if (!callback->GetState())
  {
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( surfaceFile );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(surfaceFile);
    this->AddSurfaceToDisplay(xmlReader->GetOutput(), "", foundationName.c_str(), 0, filePath.c_str() );

/*
    this->SurfaceList->AppendItem(vtkMimxSurfacePolyDataActor::New());
    int item = this->SurfaceList->GetNumberOfItems()-1;
    this->SurfaceList->GetItem(item)->SetDataType(ACTOR_POLYDATA_SURFACE);
    this->SurfaceList->GetItem(item)->SetFilePath( surfaceFile );
    this->SurfaceList->GetItem(item)->SetFoundationName( this->ExtractFileName(surfaceFile) );
    
    vtkMimxSurfacePolyDataActor *actor = vtkMimxSurfacePolyDataActor::SafeDownCast(
                                                                                   this->SurfaceList->GetItem(item));
    actor->GetDataSet()->DeepCopy( xmlReader->GetOutput() );
    actor->GetDataSet()->Modified();
    this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
    this->GetMimxMainWindow()->GetRenderWidget()->Render();
    this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
    this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem(item) );
    this->UpdateObjectLists();
*/    
    vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
    std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( surfaceFile );
    app->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
    this->GetMimxMainWindow()->SetStatusText("Loaded Surface");
  }
  else
  {
    callback->ErrorMessage("Failed to load the requested file.");
  }
  xmlReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}
