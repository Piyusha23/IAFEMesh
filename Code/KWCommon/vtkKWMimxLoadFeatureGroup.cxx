/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadFeatureGroup.cxx,v $
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

#include "vtkKWMimxLoadFeatureGroup.h"

#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

#include <vtkKWLoadSaveDialog.h>
#include <vtkKWMessageDialog.h>
#include <vtkKWPushButton.h>

#include "vtkMimxErrorCallback.h"
//#include "vtkMimxProgressBarCallback.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxLoadFeatureGroup);
vtkCxxRevisionMacro(vtkKWMimxLoadFeatureGroup, "$Revision: 1.1 $");


//----------------------------------------------------------------------------
vtkKWMimxLoadFeatureGroup::vtkKWMimxLoadFeatureGroup()
{
  
}

//----------------------------------------------------------------------------
vtkKWMimxLoadFeatureGroup::~vtkKWMimxLoadFeatureGroup()
{
	
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::CreateWidget()
{
	
  vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
	fileBrowserDialog->SetApplication(this->GetApplication());
	fileBrowserDialog->Create();
	fileBrowserDialog->SetTitle ("Load Feature(s)");
	fileBrowserDialog->SetFileTypes("{{VTK files} {.vtk}} {{VTK XML files} {.vtp}} {{All files} {*.*}}");
	fileBrowserDialog->SetDefaultExtension(".vtk");
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
  fileBrowserDialog->SetMultipleSelection(1);	
	fileBrowserDialog->Invoke();
  
	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		int numSelections = fileBrowserDialog->GetNumberOfFileNames();
    
    for (int i=0; i<numSelections; i++)
		{
      std::string surfaceFileName = vtksys::SystemTools::CollapseFullPath( fileBrowserDialog->GetNthFileName(i) );
			std::string extension = vtksys::SystemTools::LowerCase( vtksys::SystemTools::GetFilenameLastExtension( surfaceFileName ) );
			std::string surfaceDirectory = vtksys::SystemTools::GetFilenamePath( surfaceFileName );
			
			this->GetMimxMainWindow()->SetStatusText(surfaceFileName.c_str());

			if (extension == ".vtk")
			{
			  this->LoadVtkPolyData( surfaceFileName.c_str() );
			}
			else
			{
			  this->LoadVtkXMLPolyData( surfaceFileName.c_str() );
			}
	    
      this->UpdateObjectLists();
      this->GetApplication()->SetRegistryValue(1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( surfaceFileName ).c_str());
      fileBrowserDialog->SaveLastPathToRegistry("LastPath");
      //this->GetMimxMainWindow()->InitializeWorkingDirectory( surfaceDirectory.c_str() ); 
    }
    this->GetMimxMainWindow()->SetStatusText("Loaded Feature(s)");
	}
  fileBrowserDialog->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::UpdateObjectLists()
{
	
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::LoadVtkPolyData(const char *fileName)
{
  
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  
  callback->SetState(0);
  
	vtkPolyDataReader *vtkReader = vtkPolyDataReader::New();
  vtkReader->SetFileName( fileName );
  vtkReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //vtkReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  vtkReader->Update();
  
  if (!callback->GetState())
    {
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( fileName );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(fileName);
    this->AddFeatureToDisplay(vtkReader->GetOutput(), "", foundationName.c_str(), 0 , filePath.c_str());
    }
  else
    {
    callback->ErrorMessage("Failed to load the requested file.");
    }
  vtkReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}

//------------------------------------------------------------------------------
void vtkKWMimxLoadFeatureGroup::LoadVtkXMLPolyData(const char *fileName)
{
  
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
  //vtkMimxProgressBarCallback *progresscallback = this->GetMimxMainWindow()->GetProgressBarCallback();
  callback->SetState(0);
  
	vtkXMLPolyDataReader *xmlReader = vtkXMLPolyDataReader::New();
  xmlReader->SetFileName( fileName );
  xmlReader->AddObserver(vtkCommand::ErrorEvent, callback, 1.0);
  //xmlReader->AddObserver(vtkCommand::ProgressEvent, progresscallback, 1.0);
  xmlReader->Update();
  
  if (!callback->GetState())
    {
    std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( fileName );
    std::string filePath = vtksys::SystemTools::GetFilenamePath(fileName);
    this->AddFeatureToDisplay(xmlReader->GetOutput(), "", foundationName.c_str() , 0, filePath.c_str());
    }
  else
    {
    callback->ErrorMessage("Failed to load the requested file.");
    }
  xmlReader->Delete();
  //this->GetMimxMainWindow()->GetMainWindow()->GetProgressGauge()->SetValue(0.0);
}

//------------------------------------------------------------------------------
