/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSaveSceneGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/01/15 23:05:54 $
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

#include "vtkKWMimxSaveSceneGroup.h"

#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkSTLWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <vtkKWCheckButton.h>
#include <vtkKWComboBox.h>
#include <vtkKWDirectoryPresetSelector.h>
#include <vtkKWIcon.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWLoadSaveButton.h>
#include <vtkKWMessageDialog.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWPushButton.h>
#include <vtkKWLoadSaveButton.h>

#include "vtkMimxErrorCallback.h"
#include "vtkMimxActorBase.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxMeshActor.h"
#include "vtkMimxSurfacePolyDataActor.h"

#include "vtkKWMimxMainMenuGroup.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxSaveImageGroup.h"
#include "vtkKWMimxSaveSurfaceGroup.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

#include "vtkLinkedListWrapper.h"

#include <vtksys/SystemTools.hxx>

#include "Resources/mimxCancel.h"
#include "Resources/mimxApply.h"
#include "Resources/mimxDirectory.h"

#include <itkImageFileWriter.h>

#include "itkMimxSceneXMLFile.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxSaveSceneGroup);
vtkCxxRevisionMacro(vtkKWMimxSaveSceneGroup, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkKWMimxSaveSceneGroup::vtkKWMimxSaveSceneGroup()
{
  this->CancelButton = NULL;
  this->ApplyButton = NULL;
  this->SetDirButton = NULL;
  this->WithdrawOnApply = 0;
  this->BBoxList = NULL;
  this->FEMeshList = NULL;
  this->SurfaceList = NULL;
  this->ImageList = NULL;
  this->MimxMainWindow = NULL;
  this->MainNotebook = NULL;
  this->MultiColumnList = NULL;
  this->Dialog = vtkKWMessageDialog::New();
}

//----------------------------------------------------------------------------
vtkKWMimxSaveSceneGroup::~vtkKWMimxSaveSceneGroup()
{
  if (this->CancelButton)
	  this->CancelButton->Delete();
  if (this->ApplyButton)
	  this->ApplyButton->Delete();
  if(this->MultiColumnList)
	  this->MultiColumnList->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::CreateWidget()
{

  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }
  
  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  this->SetResizable(1, 1);
  this->SetMinimumSize(275, 150);
  this->ModalOff( );
  this->SetMasterWindow(this->GetMimxMainWindow());
  this->SetTitle("Save Scene");
 
  // multi-column list to set the save parameters
  if (!this->MultiColumnList)
	  this->MultiColumnList = vtkKWMultiColumnListWithScrollbars::New();
  this->MultiColumnList->SetParent(this);
  this->MultiColumnList->Create();
  this->MultiColumnList->SetHorizontalScrollbarVisibility(1);
  this->MultiColumnList->SetVerticalScrollbarVisibility(1);
  this->MultiColumnList->GetWidget()->SetHeight(5);
  

  // Add the columns 
  int col_index;

  col_index = this->MultiColumnList->GetWidget()->AddColumn(NULL);
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(0);
  this->MultiColumnList->GetWidget()->SetColumnLabelImageToPredefinedIcon(
	  0, vtkKWIcon::IconPlus);

  col_index = this->MultiColumnList->GetWidget()->AddColumn("Name");

  this->MultiColumnList->GetWidget()->ColumnEditableOn(col_index);
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);

  col_index = this->MultiColumnList->GetWidget()->AddColumn("Data Type");
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);

  col_index = this->MultiColumnList->GetWidget()->AddColumn("Status");
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);

  col_index = this->MultiColumnList->GetWidget()->AddColumn("File Format");
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);

  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(4);
  //this->MultiColumnList->GetWidget()->SetColumnWidth( 4, 15);
  col_index = this->MultiColumnList->GetWidget()->AddColumn("Directory");
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);

  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(5);
  
  col_index = this->MultiColumnList->GetWidget()->AddColumn("Save Type");
  this->MultiColumnList->GetWidget()->SetColumnAlignmentToCenter(col_index);
  this->MultiColumnList->GetWidget()->SetColumnFormatCommandToEmptyOutput(6);

  this->MultiColumnList->GetWidget()->SetSortArrowVisibility(0);
  this->MultiColumnList->GetWidget()->ColumnSeparatorsVisibilityOff();

  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
	  this->MultiColumnList->GetWidgetName());

	if (!this->ApplyButton)
    this->ApplyButton = vtkKWPushButton::New();
  this->ApplyButton->SetParent( this );
  this->ApplyButton->Create();
  vtkKWIcon *applyIcon = vtkKWIcon::New();
  applyIcon->SetImage(  image_mimxApply, 
                        image_mimxApply_width, 
                        image_mimxApply_height, 
                        image_mimxApply_pixel_size);
  this->ApplyButton->SetImageToIcon( applyIcon );
  this->ApplyButton->SetReliefToFlat();
  this->ApplyButton->SetCommand(this, "ApplyButtonCallback");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor nw -expand y -padx 2 -pady 0", 
	  this->ApplyButton->GetWidgetName());
	
	vtkKWIcon *directoryIcon = vtkKWIcon::New();
  directoryIcon->SetImage( image_mimxDirectory, 
                       image_mimxDirectory_width, 
                       image_mimxDirectory_height, 
                       image_mimxDirectory_pixel_size);  
                       
	if (!this->SetDirButton)
    this->SetDirButton = vtkKWLoadSaveButton::New();
  this->SetDirButton->SetParent( this );
  this->SetDirButton->Create();
  this->SetDirButton->SetImageToIcon(directoryIcon);
  this->SetDirButton->SetReliefToFlat();
  //this->SetDirButton->SetText("Set Directory");
  this->SetDirButton->GetLoadSaveDialog()->ChooseDirectoryOn();
  this->SetDirButton->SetCommand(this, "SetSceneDirectory");
  this->GetApplication()->Script(
	  "pack %s -side left -anchor n -expand y -padx 2 -pady 0", 
	  this->SetDirButton->GetWidgetName());
	  
	        
	if (!this->CancelButton)
    this->CancelButton = vtkKWPushButton::New();
  this->CancelButton->SetParent( this );
  this->CancelButton->Create();
  vtkKWIcon *cancelIcon = vtkKWIcon::New();
  cancelIcon->SetImage(  image_mimxCancel, 
                         image_mimxCancel_width, 
                         image_mimxCancel_height, 
                         image_mimxCancel_pixel_size);
  this->CancelButton->SetImageToIcon( cancelIcon );
  this->CancelButton->SetReliefToFlat();
  this->CancelButton->SetCommand(this, "Withdraw");
  this->GetApplication()->Script(
	  "pack %s -side right -anchor ne -expand y -padx 2 -pady 0", 
	  this->CancelButton->GetWidgetName()); 
	
  this->AddBinding("<Return>", this, "ApplyButtonCallback");
  this->AddBinding("<Escape>", this, "Withdraw");
}
//----------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}
//-----------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::UpdateObjectLists()
{
	vtkKWMultiColumnList *multList = this->MultiColumnList->GetWidget();

	multList->DeleteAllRows();
	
	char lastpath[1024];

	this->GetApplication()->GetRegistryValue(1, "RunTime", "LastPath", lastpath);

	int count = 0;
	
	static int index = 0;

	// to store scene data
	multList->InsertCellTextAsInt(count, 0, 1);
	multList->SetCellWindowCommandToCheckButton(count, 0);
  
  char sceneFile[32];
  sprintf(sceneFile, "scene%d", index);
  index++;
  
	multList->InsertCellText(count, 1, sceneFile);
  
	multList->InsertCellText(count, 2, "Scene");

	multList->InsertCellText(count, 3, "Modified");

	multList->InsertCellText(count, 4, NULL);
	multList->SetCellWindowCommandToComboBox(count, 4);
	multList->GetCellWindowAsComboBox(count, 4)->AddValue(".xml");
	multList->GetCellWindowAsComboBox(count, 4)->SetValue(".xml");

	multList->InsertCellText(count, 5, NULL);
	multList->SetCellWindowCommandToPickDirectoryButton(count, 5);


	if(vtksys::SystemTools::FileIsDirectory(lastpath))
	{
		multList->SetCellText(count, 5, lastpath);
		multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
		
		this->SetDirButton->GetLoadSaveDialog()->SetInitialFileName(lastpath);
	}

	count++;


	int numItems;
	vtkMimxActorBase *actorBase;

	// populate the rows.
	// image
	if(this->ImageList)
	{
		numItems = this->ImageList->GetNumberOfItems();
		
		for (int i=0; i<numItems; i++)
		{
			actorBase = this->ImageList->GetItem(i);
			multList->InsertCellTextAsInt(count, 0, 1);
			multList->SetCellWindowCommandToCheckButton(count, 0);

			multList->InsertCellText(count, 1, actorBase->GetFileName());

			multList->InsertCellText(count, 2, "Image");

			if(actorBase->GetIsModified())
			{
				multList->InsertCellText(count, 3, "Modified");
			}
			else
			{
				multList->InsertCellText(count, 3, "Unmodified");
			}

			multList->InsertCellText(count, 4, NULL);
			multList->SetCellWindowCommandToComboBox(count, 4);
			multList->GetCellWindowAsComboBox(count, 4)->AddValue(".hdr");
      multList->GetCellWindowAsComboBox(count, 4)->AddValue(".nii");
      multList->GetCellWindowAsComboBox(count, 4)->AddValue(".nhdr");
			multList->GetCellWindowAsComboBox(count, 4)->SetValue(".hdr");

			multList->InsertCellText(count, 5, NULL);
			multList->SetCellWindowCommandToPickDirectoryButton(count, 5);

			const char *filePath = actorBase->GetFilePath();
			if(filePath)
			{
				if(vtksys::SystemTools::FileIsDirectory(filePath))
				{
					multList->SetCellText(count, 5, filePath);
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(filePath);
				}
				else
				{
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
				}
			}
			else
			{
				multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
			}

			multList->InsertCellText(count, 6, NULL);
			multList->SetCellWindowCommandToComboBox(count, 6);
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("ASCII");
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("Binary");
			multList->GetCellWindowAsComboBox(count, 6)->SetValue("ASCII");
			count++;
		}
	}

	// surface
	if(this->SurfaceList)
	{
		numItems = this->SurfaceList->GetNumberOfItems();

		for (int i=0; i<numItems; i++)
		{
			actorBase = this->SurfaceList->GetItem(i);
			multList->InsertCellTextAsInt(count, 0, 1);
			multList->SetCellWindowCommandToCheckButton(count, 0);

			multList->InsertCellText(count, 1, actorBase->GetFileName());
			
			multList->InsertCellText(count, 2, "Surface");

			if(actorBase->GetIsModified())
			{
				multList->InsertCellText(count, 3, "Modified");
			}
			else
			{
				multList->InsertCellText(count, 3, "Unmodified");
			}

			multList->InsertCellText(count, 4, NULL);
			multList->SetCellWindowCommandToComboBox(count, 4);
			multList->GetCellWindowAsComboBox(count, 4)->AddValue(".vtk");
			multList->GetCellWindowAsComboBox(count, 4)->AddValue(".stl");
      multList->GetCellWindowAsComboBox(count, 4)->AddValue(".vtp");
			multList->GetCellWindowAsComboBox(count, 4)->SetValue(".vtk");
			
			multList->InsertCellText(count, 5, NULL);
			multList->SetCellWindowCommandToPickDirectoryButton(count, 5);

			const char *filePath = actorBase->GetFilePath();
			if(filePath)
			{
				if(vtksys::SystemTools::FileIsDirectory(filePath))
				{
					multList->SetCellText(count, 5, filePath);
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(filePath);
				}
				else
				{
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
				}
			}
			else
			{
				multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
			}

			multList->InsertCellText(count, 6, NULL);
			multList->SetCellWindowCommandToComboBox(count, 6);
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("ASCII");
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("Binary");
			multList->GetCellWindowAsComboBox(count, 6)->SetValue("ASCII");
			count++;
		}
	}

	// Building block
	if(this->BBoxList)
	{
		numItems = this->BBoxList->GetNumberOfItems();

		for (int i=0; i<numItems; i++)
		{
			actorBase = this->BBoxList->GetItem(i);
			multList->InsertCellTextAsInt(count, 0, 1);
			multList->SetCellWindowCommandToCheckButton(count, 0);

			multList->InsertCellText(count, 1, actorBase->GetFileName());

			multList->InsertCellText(count, 2, "BBlock");

			if(actorBase->GetIsModified())
			{
				multList->InsertCellText(count, 3, "Modified");
			}
			else
			{
				multList->InsertCellText(count, 3, "Unmodified");
			}

			multList->InsertCellText(count, 4, NULL);
			multList->SetCellWindowCommandToComboBox(count, 4);
			multList->GetCellWindowAsComboBox(count, 4)->AddValue(".vtk");
      multList->GetCellWindowAsComboBox(count, 4)->AddValue(".vtu");
			multList->GetCellWindowAsComboBox(count, 4)->SetValue(".vtk");

			multList->InsertCellText(count, 5, NULL);
			multList->SetCellWindowCommandToPickDirectoryButton(count, 5);

			const char *filePath = actorBase->GetFilePath();
			if(filePath)
			{
				if(vtksys::SystemTools::FileIsDirectory(filePath))
				{
					multList->SetCellText(count, 5, filePath);
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(filePath);
				}
				else
				{
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
				}
			}
			else
			{
				multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
			}
			multList->InsertCellText(count, 6, NULL);
			multList->SetCellWindowCommandToComboBox(count, 6);
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("ASCII");
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("Binary");
			multList->GetCellWindowAsComboBox(count, 6)->SetValue("ASCII");
			count++;
		}
	}

	// FE Mesh
	if(this->FEMeshList)
	{
		numItems = this->FEMeshList->GetNumberOfItems();

		for (int i=0; i<numItems; i++)
		{
			actorBase = this->FEMeshList->GetItem(i);
			multList->InsertCellTextAsInt(count, 0, 1);
			multList->SetCellWindowCommandToCheckButton(count, 0);

			multList->InsertCellText(count, 1, actorBase->GetFileName());

			multList->InsertCellText(count, 2, "Mesh");

			if(actorBase->GetIsModified())
			{
				multList->InsertCellText(count, 3, "Modified");
			}
			else
			{
				multList->InsertCellText(count, 3, "Unmodified");
			}

			multList->InsertCellText(count, 4, NULL);
			multList->SetCellWindowCommandToComboBox(count, 4);
			multList->GetCellWindowAsComboBox(count, 4)->AddValue(".vtk");
			multList->GetCellWindowAsComboBox(count, 4)->SetValue(".vtk");

			multList->InsertCellText(count, 5, NULL);
			multList->SetCellWindowCommandToPickDirectoryButton(count, 5);

			const char *filePath = actorBase->GetFilePath();
			if(filePath)
			{
				if(vtksys::SystemTools::FileIsDirectory(filePath))
				{
					multList->SetCellText(count, 5, filePath);
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(filePath);
				}
				else
				{
					multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
				}
			}
			else
			{
				multList->GetCellWindowAsPickDirectoryButton(count, 5)->SetInitialFileName(lastpath);
			}
			multList->InsertCellText(count, 6, NULL);
			multList->SetCellWindowCommandToComboBox(count, 6);
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("ASCII");
			multList->GetCellWindowAsComboBox(count, 6)->AddValue("Binary");
			multList->GetCellWindowAsComboBox(count, 6)->SetValue("ASCII");
			count++;
		}
	}
}
//-----------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
//void vtkKWMimxSaveSceneGroup::SetMimxMainWindow(vtkKWMimxMainWindow *window)
//{
//	this->MimxMainWindow = window;
//}
////----------------------------------------------------------------------------
//vtkKWMimxMainWindow* vtkKWMimxSaveSceneGroup::GetMimxMainWindow( )
//{
//	return this->MimxMainWindow;
//}
//---------------------------------------------------------------------------------------
int vtkKWMimxSaveSceneGroup::ApplyButtonCallback()
{
	typedef itk::MimxSceneXMLFileWriter SceneWriterType;

	SceneWriterType::Pointer SceneWriter = itk::MimxSceneXMLFileWriter::New();

	vtkKWMultiColumnList *multList = this->MultiColumnList->GetWidget();
	int numItems = this->MultiColumnList->GetWidget()->GetNumberOfRows();
	
	for (int i=1; i<numItems; i++)
	{
		if(multList->GetCellWindowAsCheckButton(i, 0)->GetSelectedState())
		{
			bool savetype = false;
			const char *savetypeentry = multList->GetCellWindowAsComboBox(i, 6)->GetValue();
			if(!strcmp(savetypeentry, "Binary"))
			{
				savetype = true;
			}

			// get the path minus file name
			char filePath[1024];
			const char  *filepath = multList->GetCellWindowAsPickDirectoryButton(i, 5)->GetLoadSaveDialog()->GetLastPath();
			strcpy(filePath, filepath);	

			// add the file name
			const char *filename = multList->GetCellText(i, 1);
			strcat(filePath, "/");
			strcat(filePath, filename);

			// check for file extension and add
			const char *fileext = multList->GetCellWindowAsComboBox(i, 4)->GetValue();
			strcat(filePath, fileext);
			
			vtkStdString fileExt;
			if(!strcmp(fileext, ".stl"))	fileExt = "STL";
			if(!strcmp(fileext, ".vtk"))	fileExt = "VTK";
			if(!strcmp(fileext, ".xml"))	fileExt = "XML";
			if(!strcmp(fileext, ".hdr"))	fileExt = "HDR";

			// check for the type of data stored
			const char *datatype = multList->GetCellText(i, 2);
			if(!strcmp(datatype, "Image"))	SceneWriter->AddImageFile(filePath, savetype, fileExt.c_str());
			if(!strcmp(datatype, "Surface"))	SceneWriter->AddSurfaceFile(filePath, savetype, fileExt.c_str());
			if(!strcmp(datatype, "BBlock"))	SceneWriter->AddBuildignBlockFile(filePath, savetype, fileExt.c_str());
			if(!strcmp(datatype, "Mesh"))	SceneWriter->AddMeshFile(filePath, savetype, fileExt.c_str());
		}
	}
	
	// for storing the scene information


	char scenePath[1024];

	const char *scenepath = multList->GetCellWindowAsPickDirectoryButton(0, 5)->GetLoadSaveDialog()->GetLastPath();
	strcpy(scenePath, scenepath);
	strcat(scenePath, "/");
	const char *scenename = multList->GetCellText(0, 1);
	strcat(scenePath, scenename); 
	strcat(scenePath,multList->GetCellWindowAsComboBox(0, 4)->GetValue()); 

	SceneWriter->SetFilename(scenePath);
	SceneWriter->WriteFile();
	
	// write the contents of the SceneWriter.
	int numFiles = SceneWriter->GetSceneData().size();
    
	//std::list<itk::MimxSceneInfo*>::iterator it;

//	for (it=SceneWriter->GetSceneData().begin();it!=SceneWriter->GetSceneData().end();it++)
	for(int i=0; i<numFiles; i++)
	{
		itk::MimxSceneInfo *it = SceneWriter->GetSceneData()[i];
		// check if the file already exists. If Yes user decides if it needs
		// to be overwritten.
		//if(vtksys::SystemTools::FileExists((*it)->m_FileName.c_str()))
		if(vtksys::SystemTools::FileExists(it->m_FileName.c_str()))
		{
			this->Dialog->SetStyleToYesNo();
			this->Dialog->SetApplication(this->GetApplication());
			this->Dialog->Create();
			this->Dialog->SetTitle("Your Attention Please!");
			char Message[1024];
			sprintf(Message, "File %s already exists. Would you like to overwrite?", (it)->m_FileName.c_str());
			this->Dialog->SetText(Message);
			this->Dialog->Invoke();
			if(this->Dialog->GetStatus() == vtkKWMessageDialog::StatusCanceled)
			{
				continue;
			}
		}

		// get the file name from the entire path.
		std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension((it)->m_FileName);

		// check for the type of datatype
		if((it)->m_ObjectType == "Image")
		{
			vtkMimxImageActor *ImageActor = vtkMimxImageActor::SafeDownCast(
				this->ImageList->GetItem(filename.c_str()));
			this->SaveImage((it)->m_FileName.c_str(), ImageActor);
			continue;
		}

		if((it)->m_ObjectType == "Surface")
		{
			vtkMimxSurfacePolyDataActor *SurfActor = vtkMimxSurfacePolyDataActor::
				SafeDownCast(this->SurfaceList->GetItem(filename.c_str()));
			vtkPolyData *polydata = SurfActor->GetDataSet();
			if((it)->m_FileType == "STL")
			{
				this->SaveSTLSurface((it)->m_FileName.c_str(), polydata, (it)->m_Binary);	
			}
			else if((it)->m_FileType == "XML")
			{
				this->SaveVTKXMLSurface((it)->m_FileName.c_str(), polydata, (it)->m_Binary);
			}
			else
			{
				this->SaveVTKSurface((it)->m_FileName.c_str(), polydata, (it)->m_Binary);
			}
			continue;
		}

		if((it)->m_ObjectType == "BuildingBlock")
		{
			vtkMimxUnstructuredGridActor *BBlockActor = vtkMimxUnstructuredGridActor::
				SafeDownCast(this->BBoxList->GetItem(filename.c_str()));
			vtkUnstructuredGrid *ugrid = BBlockActor->GetDataSet();
			if((it)->m_FileType == "XML")
			{
				this->SaveVTKXMLBuildingBlock((it)->m_FileName.c_str(), ugrid, (it)->m_Binary);
			}
			else
			{
				this->SaveVTKBuildingBlock((it)->m_FileName.c_str(), ugrid, (it)->m_Binary);
			}
			continue;
		}

		if((it)->m_ObjectType == "Mesh")
		{
			vtkMimxMeshActor *MeshActor = vtkMimxMeshActor::SafeDownCast(
				this->FEMeshList->GetItem(filename.c_str()));
			vtkUnstructuredGrid *mesh = MeshActor->GetDataSet();
			if((it)->m_FileType == "XML")
			{
				this->WriteXMLUnstructuredGrid((it)->m_FileName.c_str(), mesh, (it)->m_Binary);
			}
			else
			{
				this->WriteUnstructuredGrid((it)->m_FileName.c_str(), mesh, (it)->m_Binary);
			}
			continue;
		}
	}
	this->Withdraw();
	
	return 1;	
}

//---------------------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::SetSceneDirectory()
{
	char *directory = this->SetDirButton->GetLoadSaveDialog()->GetFileName();
	int numItems = this->MultiColumnList->GetWidget()->GetNumberOfRows();
	
	for (int i=0; i<numItems; i++)
	{
		this->MultiColumnList->GetWidget()->GetCellWindowAsPickDirectoryButton(i, 5)->SetText(directory);
		this->MultiColumnList->GetWidget()->GetCellWindowAsPickDirectoryButton(i, 5)->GetLoadSaveDialog()->SetInitialFileName(directory);
		this->MultiColumnList->GetWidget()->GetCellWindowAsPickDirectoryButton(i, 5)->GetLoadSaveDialog()->SetFileName(directory);
	}
	this->SetDirButton->SetText("");
}

//----------------------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::SaveImage(const char *FileName, vtkMimxImageActor *ImageActor)
{
	typedef itk::ImageFileWriter< ImageType >  WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(FileName);
	writer->SetInput(ImageActor->GetITKImage());
	writer->Update();
}
//---------------------------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::Withdraw()
{
	this->Superclass::Withdraw();
	this->MimxMainWindow->GetMainUserInterfacePanel()->SetEnabled(1);
}
//----------------------------------------------------------------------------------------

void vtkKWMimxSaveSceneGroup::SaveSTLSurface(const char *fileName, vtkPolyData *polyData, bool binary)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

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
	if (callback->GetState())
	{
		std::cerr << "Failed to write the file" << std::endl;
	}
	writer->Delete();	
}

//--------------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::SaveVTKSurface(const char *fileName, vtkPolyData *polyData, bool binary)
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
void vtkKWMimxSaveSceneGroup::SaveVTKXMLSurface(const char *fileName, vtkPolyData *polyData, bool binary)
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
void vtkKWMimxSaveSceneGroup::SaveVTKBuildingBlock(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
	writer->SetFileName( fileName );
	writer->SetInput( ugrid );
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
void vtkKWMimxSaveSceneGroup::SaveVTKXMLBuildingBlock(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary)
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

	vtkXMLUnstructuredGridWriter *writer = vtkXMLUnstructuredGridWriter::New();
	writer->SetFileName( fileName );
	writer->SetInput( ugrid );
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
//---------------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::WriteUnstructuredGrid(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary )
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

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
	if (callback->GetState())
	{
		std::cerr << "Error writing file" << std::endl;
	}
	writer->Delete();
}

//---------------------------------------------------------------------------------
void vtkKWMimxSaveSceneGroup::WriteXMLUnstructuredGrid(const char *fileName, vtkUnstructuredGrid *ugrid, bool binary )
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

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
	if (callback->GetState())
	{
		std::cerr << "Error writing file" << std::endl;
	}
	writer->Delete();
}
//-----------------------------------------------------------------------------------
