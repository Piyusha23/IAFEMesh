/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveImageGroup.cxx,v $
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

#include "vtkKWMimxSaveImageGroup.h"

#include <itkImageFileWriter.h>

#include <vtkObjectFactory.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtksys/SystemTools.hxx>

#include <vtkKWLoadSaveDialog.h>

#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkMimxImageActor.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSaveImageGroup);
vtkCxxRevisionMacro(vtkKWMimxSaveImageGroup, "$Revision: 1.3 $");

//----------------------------------------------------------------------------
vtkKWMimxSaveImageGroup::vtkKWMimxSaveImageGroup()
{
  this->ObjectListComboBox = NULL;
  this->FileBrowserDialog = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxSaveImageGroup::~vtkKWMimxSaveImageGroup()
{
  if(this->ObjectListComboBox)
     this->ObjectListComboBox->Delete();
  if(this->FileBrowserDialog)
	  this->FileBrowserDialog->Delete();
 }
//----------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::CreateWidget()
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
  ObjectListComboBox->SetLabelText("Image : ");
  ObjectListComboBox->GetWidget()->ReadOnlyOn();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ObjectListComboBox->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "SaveImageApplyCallback");
  this->CancelButton->SetCommand(this, "SaveImageCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxSaveImageGroup::SaveImageApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ObjectListComboBox->GetWidget()->GetValue(),""))
	{
		callback->ErrorMessage("Image selection required");;
		return 0;
	}
  vtkKWComboBox *combobox = this->ObjectListComboBox->GetWidget();
  const char *name = combobox->GetValue();

	int num = combobox->GetValueIndex(name);
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Invalid image selected");
		combobox->SetValue("");
		return 0;
	}

	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(combobox->GetValueIndex(name)));

	if(!this->FileBrowserDialog)
	{
		this->FileBrowserDialog = vtkKWLoadSaveDialog::New() ;
		this->FileBrowserDialog->SaveDialogOn();
		this->FileBrowserDialog->SetApplication(this->GetApplication());
		this->FileBrowserDialog->Create();
		this->FileBrowserDialog->RetrieveLastPathFromRegistry("ImageDataPath");
		this->FileBrowserDialog->SetTitle ("Save Image File Format");
		this->FileBrowserDialog->SetFileTypes ("{{NifTI} {.nii .nii.gz}} {{Analyze files} {.hdr}} {{Meta-Image} {.mhd .mha}} {{All Files} {*.*}}");
		this->FileBrowserDialog->SetDefaultExtension (".nii");
	}
	this->FileBrowserDialog->RetrieveLastPathFromRegistry("LastPath");
	this->FileBrowserDialog->Invoke();
	if(this->FileBrowserDialog->GetStatus() == vtkKWDialog::StatusOK)
	{
		if(this->FileBrowserDialog->GetFileName())
		{
			const char *filename = FileBrowserDialog->GetFileName();
			this->GetApplication()->SetRegistryValue(
				1, "RunTime", "LastPath", vtksys::SystemTools::GetFilenamePath( filename ).c_str());
			this->FileBrowserDialog->SaveLastPathToRegistry("LastPath");

			this->SaveImage(this->FileBrowserDialog->GetFileName(), imageActor);

			this->GetMimxMainWindow()->SetStatusText("Saved Image");
			return 1;
		}
	}
	return 0;
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::SaveImage(char *FileName, vtkMimxImageActor *ImageActor)
{
	typedef itk::ImageFileWriter< ImageType >  WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(FileName);
	writer->SetInput(ImageActor->GetITKImage());
	try
  {
    writer->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
    callback->SetState(1);
    callback->ErrorMessage("Failed to save the image file. Check the path and permissions.");
    return;
  }
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::SaveImageCancelCallback()
{
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
}
//------------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::UpdateObjectLists()
{
	this->ObjectListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
	{
		ObjectListComboBox->GetWidget()->AddValue(
			this->ImageList->GetItem(i)->GetFileName());
			
		int viewedItem = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(i))->GetActorVisibility();
		if ( (defaultItem == -1) && ( viewedItem ) )
		{
		  defaultItem = i;
		}
	}
	
	if ((this->ImageList->GetNumberOfItems() > 0) && (defaultItem == -1))
    defaultItem = this->ImageList->GetNumberOfItems()-1;
    
	if (defaultItem != -1)
  {
    ObjectListComboBox->GetWidget()->SetValue(
          this->ImageList->GetItem(defaultItem)->GetFileName());
  }
}
//--------------------------------------------------------------------------------
void vtkKWMimxSaveImageGroup::SaveImageDoneCallback()
{
	if(this->SaveImageApplyCallback())
		this->SaveImageCancelCallback();
}
//---------------------------------------------------------------------------------
