/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxLoadImageGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/31 14:51:36 $
Version:   $Revision: 1.9 $

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

#include "vtkKWMimxLoadImageGroup.h"

//VTK Headers
#include <vtkActor.h>
#include <vtkObjectFactory.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>
#include <vtksys/SystemTools.hxx>

//ITK Headers
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkGDCMImageIO.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMSeriesFileNames.h>
#include <gdcmSerieHelper.h>
#include <gdcmFile.h>

//KWWidgets Headers
#include <vtkKWFileBrowserDialog.h>
#include <vtkKWRenderWidget.h>
#include <vtkMimxImageActor.h>

//MIMX Headers
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxApplication.h"
//#include "vtkMimxProgressBarCallback.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxLoadImageGroup);
vtkCxxRevisionMacro(vtkKWMimxLoadImageGroup, "$Revision: 1.9 $");


//----------------------------------------------------------------------------
vtkKWMimxLoadImageGroup::vtkKWMimxLoadImageGroup()
{
	
}

//----------------------------------------------------------------------------
vtkKWMimxLoadImageGroup::~vtkKWMimxLoadImageGroup()
{
	
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::CreateWidget()
{
	vtkKWFileBrowserDialog *fileBrowserDialog = vtkKWFileBrowserDialog::New();
  fileBrowserDialog->SetApplication(this->GetApplication());
  fileBrowserDialog->Create();
  fileBrowserDialog->SetFileTypes("{{NifTI} {.nii}} {{Compressed Images} {.gz}} {{Analyze} {.hdr}}  {{DICOM} {.dcm}} {{Meta-Image} {.mhd}} {{Meta-Image} {.mha}} {{All Files} {*.*}}");
	fileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	fileBrowserDialog->Invoke();
  
	if (fileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		
		const char *filename = fileBrowserDialog->GetFileName();
		fileBrowserDialog->SaveLastPathToRegistry("LastPath");
		
    typedef itk::GDCMImageIO DicomIOType;
    DicomIOType::Pointer dicomIO = DicomIOType::New();
    bool isDicom = dicomIO->CanReadFile( filename );
    
    if ( isDicom )
    {
      this->LoadDicomSeries( filename );
		}
    else
    {
      this->LoadItkImage( filename );
    }
    
		this->UpdateObjectLists();
    this->GetMimxMainWindow()->SetStatusText("Loaded image");
	}
  
  fileBrowserDialog->Delete();
	
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::UpdateObjectLists()
{
	
}

//----------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::LoadImage(const char *FilePath)
{
  typedef itk::GDCMImageIO DicomIOType;
  DicomIOType::Pointer dicomIO = DicomIOType::New();
  bool isDicom = dicomIO->CanReadFile( FilePath );
  
  if ( isDicom )
    {
    this->LoadDicomSeries( FilePath );
		}
  else
    {
    this->LoadItkImage( FilePath );
    }
  
}
//------------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::LoadDicomSeries(const char *filename)
{
  std::string dicomDir = itksys::SystemTools::GetParentDirectory( filename );
  
  itk::GDCMSeriesFileNames::Pointer fileNameGenerator = itk::GDCMSeriesFileNames::New();
  fileNameGenerator->SetUseSeriesDetails( true );
  fileNameGenerator->SetDirectory( dicomDir );
  itk::SerieUIDContainer seriesUID = fileNameGenerator->GetSeriesUIDs( );
  
  //Use GDCM to get the SeriesUID for the selected file
  gdcm::File *header = new gdcm::File;
  header->SetLoadMode( gdcm::LD_NOSHADOW );
  header->SetMaxSizeLoadEntry( 0xfff );
  header->SetFileName( filename );
  header->Load();
  const std::string &uid = header->GetEntryValue(0x0020, 0x000e);
  //std::cout << "Series UID: " << uid << std::endl;
  delete header;
  
  // Match the GDCM UID with those from ITK
  int indexSeriesUID = 0;
  for (int i=0; i<seriesUID.size(); i++)
  {
    unsigned found = seriesUID[0].find(uid);
    if ( found != std::string::npos )
    {
      indexSeriesUID = i;
    }
  }
  //std::cout << "Selected Series UID: " << seriesUID[indexSeriesUID] << std::endl;

  // Load the DICOM Series
  typedef itk::GDCMImageIO DicomIOType;
  DicomIOType::Pointer dicomIO = DicomIOType::New();
  typedef itk::ImageSeriesReader< ImageType >     ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileNames( fileNameGenerator->GetFileNames( seriesUID[indexSeriesUID] ) );
  reader->SetImageIO( dicomIO );
  try
  {
    reader->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
    callback->SetState(1);
    callback->ErrorMessage("Failed to load the requested DICOM images.");
    return;
  }
  
  std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( filename );
  std::string filePath = vtksys::SystemTools::GetFilenamePath(filename);
  this->AddImageToDisplay(reader->GetOutput(), "", foundationName.c_str(), 0 , filePath.c_str());

}

//------------------------------------------------------------------------------
void vtkKWMimxLoadImageGroup::LoadItkImage(const char *filename)
{
	typedef itk::ImageFileReader<ImageType> ReaderType; 
  ReaderType::Pointer imageReader = ReaderType::New();
  imageReader->SetFileName( filename );
  try
  {
    imageReader->Update();
  }
  catch (itk::ExceptionObject & err)
  {
    vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
    callback->SetState(1);
    callback->ErrorMessage("Failed to load the requested image.");
    return;
  }
  
  std::string foundationName = vtksys::SystemTools::GetFilenameWithoutExtension( filename );
  std::string filePath = vtksys::SystemTools::GetFilenamePath(filename);
  this->AddImageToDisplay(imageReader->GetOutput(), "", foundationName.c_str(), 0, filePath.c_str() );
  
  /*
  vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetApplication());
  std::string imageDirectory = vtksys::SystemTools::GetFilenamePath( filename );
  app->InitializeWorkingDirectory( imageDirectory.c_str() );
  */
}
//----------------------------------------------------------------------------
