/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxGroupBase.cxx,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:58 $
Version:   $Revision: 1.7 $

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

#include "vtkKWMimxGroupBase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* VTK Headers */
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkLinkedListWrapper.h>
#include <vtkMath.h>
#include <vtkCellData.h>
#include <vtkObjectFactory.h>
#include <vtkCommand.h>
#include <vtkLinkedListWrapperTree.h>
#include <vtksys/stl/list>
#include <vtksys/stl/algorithm>

/* KWWidget Headers */
#include <vtkKWRenderWidget.h>
#include <vtkKWApplication.h>
#include <vtkKWFileBrowserDialog.h>
#include <vtkKWEvent.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWIcon.h>
#include <vtkKWInternationalization.h>
#include <vtkKWLabel.h>
#include <vtkKWMenu.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWNotebook.h>
#include <vtkKWOptions.h>
#include <vtkKWRenderWidget.h>
#include <vtkKWTkUtilities.h>
#include <vtkKWPushButton.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWComboBox.h>

/* MIMX Headers */
#include "vtkMimxUnstructuredGridWidget.h"
#include "vtkMimxBoundingBoxSource.h"
#include "vtkMimxSurfacePolyDataActor.h"
#include "vtkMimxFeaturePolyDataActor.h"
#include "vtkMimxUnstructuredGridActor.h"
#include "vtkMimxEntryValueChangedCallback.h"
#include "vtkMimxMeshActor.h"
#include "vtkMimxImageActor.h"
#include "vtkKWMimxApplication.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxGroupBase);
vtkCxxRevisionMacro(vtkKWMimxGroupBase, "$Revision: 1.7 $");

//----------------------------------------------------------------------------
vtkKWMimxGroupBase::vtkKWMimxGroupBase()
{
  this->BBoxList = NULL;
  this->FEMeshList = NULL;
  this->SurfaceList = NULL;
  this->FeatureList = NULL;
  this->ImageList = NULL;
  this->MimxMainWindow = NULL;
  this->CancelButton = vtkKWPushButton::New();
  this->DoneButton = vtkKWPushButton::New();
  this->MainFrame = vtkKWFrame::New();
  this->Count = 0;
  this->ViewProperties = NULL;
  this->MenuGroup = NULL;
  this->ApplyButton = vtkKWPushButton::New();
  this->DoUndoTree = NULL;
  this->CancelStatus = 0;
  strcpy(this->objectSelectionPrevious,"");
  this->EntryChangedCallback = vtkMimxEntryValueChangedCallback::New();
}

//----------------------------------------------------------------------------
vtkKWMimxGroupBase::~vtkKWMimxGroupBase()
{
  this->CancelButton->Delete();
  this->DoneButton->Delete();
  this->MainFrame->Delete();
  this->ApplyButton->Delete();
  this->EntryChangedCallback->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::CreateWidget()
{
	if(this->IsCreated())
	{
		vtkErrorMacro("class already created");
		return;
	}

	this->Superclass::CreateWidget();

}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::Update()
{
	this->UpdateEnableState();
}

//---------------------------------------------------------------------------
void vtkKWMimxGroupBase::UpdateEnableState()
{
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::AddMeshToDisplay( vtkUnstructuredGrid *mesh, const char *namePrefix,
                                          const char *foundationName, const char *elementSetName,
                                          vtkIdType Mod, const char *filePath)
{
  vtkMimxMeshActor *actor = vtkMimxMeshActor::New();
  actor->SetIsModified(Mod);
  actor->SetDataType( ACTOR_FE_MESH );
  actor->SetFoundationName(foundationName);
  this->FEMeshList->AppendItem( actor );
  actor->SetDataSet( mesh );
  actor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
  
  if (filePath)
    {
	  actor->SetFilePath(filePath);
    }
  
  bool useCounter = false;
  int offset = 1;
  std::string myName = foundationName;
  std::string myPrefix = namePrefix;
  if (myPrefix.length() > 0)
    {
    myName += "_";
    myName += myPrefix;
    offset = 0;
    }
  
  
  int maxIndex = 0;
  for (int i=0;i<this->FEMeshList->GetNumberOfItems()-1;i++)
    {
    const char *itemName = this->FEMeshList->GetItem(i)->GetFileName();
    const char *substr = strstr(itemName, myName.c_str());
    if ( substr == itemName)
      {
      char itemIndex[16];
      int startIndex = myName.length();
      int itemLength = strlen(itemName);
      int length = itemLength - startIndex;
      int currentIndex;
      
      useCounter = true;
      if (length == 0)
        {
        currentIndex = 0;
        }
      else
        {
        strncpy(itemIndex, &(itemName[startIndex+offset]), length-offset);
        itemIndex[length-offset] = '\0';
        currentIndex = atoi(itemIndex);
        }
      
      if (maxIndex < currentIndex) maxIndex = currentIndex;
      }
    }
  
  vtkIdType index = maxIndex+1;
  
  if ( (useCounter) || (strcmp(namePrefix, "") != 0) )
    {
    actor->SetObjectName(namePrefix,index);
    }
  
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  
  int itemIndex = this->FEMeshList->GetNumberOfItems()-1;
  this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->FEMeshList->GetItem( itemIndex ) );
  
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::AddSurfaceToDisplay(vtkPolyData *surface, const char *namePrefix,
                                             const char *foundationName, vtkIdType Mod, const char *filePath)
{
  
  vtkMimxSurfacePolyDataActor *actor = vtkMimxSurfacePolyDataActor::New();
  actor->SetIsModified(Mod);
  actor->SetDataType( ACTOR_POLYDATA_SURFACE );
  actor->SetFoundationName(foundationName);
  this->SurfaceList->AppendItem( actor );
  actor->GetDataSet()->DeepCopy( surface );
  actor->GetDataSet()->Modified();
  //Add SetRenderer and SetDataSet to Actor
  //actor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
  
  if(filePath)
    {
	  actor->SetFilePath(filePath);
    }
  
  bool useCounter = false;
  int offset = 1;
  std::string myName = foundationName;
  std::string myPrefix = namePrefix;
  if (myPrefix.length() > 0)
    {
    myName += "_";
    myName += myPrefix;
    offset = 0;
    }
  
  int maxIndex = 0;
  for (int i=0;i<this->SurfaceList->GetNumberOfItems()-1;i++)
    {
    const char *itemName = this->SurfaceList->GetItem(i)->GetFileName();
    const char *substr = strstr(itemName, myName.c_str());
    if ( substr == itemName)
      {
      char itemIndex[16];
      int startIndex = myName.length();
      int itemLength = strlen(itemName);
      int length = itemLength - startIndex;
      int currentIndex;
      
      useCounter = true;
      if (length == 0)
        {
        currentIndex = 0;
        }
      else
        {
        strncpy(itemIndex, &(itemName[startIndex+offset]), length-offset);
        itemIndex[length-offset] = '\0';
        currentIndex = atoi(itemIndex);
        }
      
      if (maxIndex < currentIndex) maxIndex = currentIndex;
      }
    }
  
  vtkIdType index = maxIndex + 1;
  if ( (useCounter) || (strcmp(namePrefix, "") != 0) )
    {
    actor->SetObjectName(namePrefix,index);
    }
  
  this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  
  int itemIndex = this->SurfaceList->GetNumberOfItems()-1;
  this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->SurfaceList->GetItem( itemIndex ) );
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::AddFeatureToDisplay(vtkPolyData *feature, const char *namePrefix,
                                             const char *foundationName, vtkIdType Mod, const char *filePath)
{
  
  vtkMimxFeaturePolyDataActor *actor = vtkMimxFeaturePolyDataActor::New();
  actor->SetIsModified(Mod);
  actor->SetDataType( ACTOR_POLYDATA_LINE );
  actor->SetFoundationName(foundationName);
  this->FeatureList->AppendItem( actor );
  actor->GetDataSet()->DeepCopy( feature );
  actor->GetDataSet()->Modified();
  //Add SetRenderer and SetDataSet to Actor
  //actor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
  
  if(filePath)
    {
	  actor->SetFilePath(filePath);
    }
  
  bool useCounter = false;
  int offset = 1;
  std::string myName = foundationName;
  std::string myPrefix = namePrefix;
  if (myPrefix.length() > 0)
    {
    myName += "_";
    myName += myPrefix;
    offset = 0;
    }
  
  int maxIndex = 0;
  for (int i=0;i<this->FeatureList->GetNumberOfItems()-1;i++)
    {
    const char *itemName = this->FeatureList->GetItem(i)->GetFileName();
    const char *substr = strstr(itemName, myName.c_str());
    if ( substr == itemName)
      {
      char itemIndex[16];
      int startIndex = myName.length();
      int itemLength = strlen(itemName);
      int length = itemLength - startIndex;
      int currentIndex;
      
      useCounter = true;
      if (length == 0)
        {
        currentIndex = 0;
        }
      else
        {
        strncpy(itemIndex, &(itemName[startIndex+offset]), length-offset);
        itemIndex[length-offset] = '\0';
        currentIndex = atoi(itemIndex);
        }
      
      if (maxIndex < currentIndex) maxIndex = currentIndex;
      }
    }
  
  vtkIdType index = maxIndex + 1;
  if ( (useCounter) || (strcmp(namePrefix, "") != 0) )
    {
    actor->SetObjectName(namePrefix,index);
    }
  
  this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor());
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  
  int itemIndex = this->FeatureList->GetNumberOfItems()-1;
  this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->FeatureList->GetItem( itemIndex ) );
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::AddBuildingBlockToDisplay(vtkUnstructuredGrid *ugrid, const char *namePrefix,
                                                   const char *foundationName, vtkIdType Mod, const char *filePath)
{
  /* Create the New Display Node */
  vtkMimxUnstructuredGridActor *actor = vtkMimxUnstructuredGridActor::New();
	actor->SetIsModified(Mod);
	actor->SetDataType(ACTOR_BUILDING_BLOCK);
  actor->SetFoundationName(foundationName);
	actor->GetDataSet()->DeepCopy( ugrid );
  actor->GetDataSet()->Modified();
  
  this->BBoxList->AppendItem(actor);
  
	if(filePath)
    {
		actor->SetFilePath(filePath);
    }
  
	/* Create the Redo/Undo tree */
  int currentitem = this->BBoxList->GetNumberOfItems()-1;
  this->DoUndoTree->AppendItem(new Node);
  this->DoUndoTree->GetItem(currentitem)->Parent = NULL;
  this->DoUndoTree->GetItem(currentitem)->Child = NULL;
  this->DoUndoTree->GetItem(currentitem)->Data = actor;
	
	bool useCounter = false;
	int offset = 1;
	std::string myName = foundationName;
  std::string myPrefix = namePrefix;
  if (myPrefix.length() > 0)
    {
    myName += "_";
    myName += myPrefix;
    offset = 0;
    }
  
	int maxIndex = 0;
  for (int i=0;i<this->BBoxList->GetNumberOfItems()-1;i++)
    {
    const char *itemName = this->BBoxList->GetItem(i)->GetFileName();
    const char *substr = strstr(itemName, myName.c_str());
    if ( substr == itemName)
      {
      char itemIndex[16];
      int startIndex = myName.length();
      int itemLength = strlen(itemName);
      int length = itemLength - startIndex;
      int currentIndex;
      
      useCounter = true;
      if (length == 0)
        {
        currentIndex = 0;
        }
      else
        {
        strncpy(itemIndex, &(itemName[startIndex+offset]), length-offset);
        itemIndex[length-offset] = '\0';
        currentIndex = atoi(itemIndex);
        }
      
      if (maxIndex < currentIndex) maxIndex = currentIndex;
      }
    }
  
  vtkIdType index = maxIndex+1;
  
  if ( (useCounter) || (strcmp(namePrefix, "") != 0) )
    {
    actor->SetObjectName(namePrefix,index);
    }
  
  /* Assign Mesh Seeds - The initial values are based on Application Settings */
  vtkKWMimxApplication *app = vtkKWMimxApplication::SafeDownCast(this->GetMimxMainWindow()->GetApplication());
  double edgeLength = app->GetAverageElementLength();
  // check if the mesh seeds are present, if not present calculate the mesh seed
  if(!actor->GetDataSet()->GetCellData()->GetArray("Mesh_Seed"))
    {
	  actor->MeshSeedFromAverageElementLength( edgeLength, edgeLength, edgeLength );
    }
  
  actor->GetDataSet()->Modified();
  actor->SetFillColor(0.5, 0.5, 0.5);
  actor->SetOutlineColor(1.0, 1.0, 1.0);
  
  //actor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
  this->GetMimxMainWindow()->GetRenderWidget()->AddViewProp(actor->GetActor()); 
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->BBoxList->GetItem(currentitem));
}

//----------------------------------------------------------------------------
void vtkKWMimxGroupBase::AddImageToDisplay(ImageType::Pointer image, const char *namePrefix,
                                           const char *foundationName, vtkIdType Mod, const char *filePath)
{
  vtkMimxImageActor *imageActor = vtkMimxImageActor::New();
  imageActor->SetIsModified(Mod);
  imageActor->SetDataType(ACTOR_IMAGE);
  this->ImageList->AppendItem( imageActor );
  //double resolution = this->GetMimxMainWindow()->GetImageResolution();
  //imageActor->SetImageResolution( resolution );
  imageActor->SetInteractor(this->GetMimxMainWindow()->GetRenderWidget()->GetRenderWindowInteractor());
  imageActor->SetITKImage( image );
  imageActor->SetFoundationName( foundationName );
  if(filePath)
    {
	  imageActor->SetFilePath(filePath);
    }
  
  bool useCounter = false;
  int offset = 1;
  std::string myName = foundationName;
  std::string myPrefix = namePrefix;
  if (myPrefix.length() > 0)
    {
    myName += "_";
    myName += myPrefix;
    offset = 0;
    }
  
  int maxIndex = 0;
  for (int i=0;i<this->ImageList->GetNumberOfItems()-1;i++)
    {
    const char *itemName = this->ImageList->GetItem(i)->GetFileName();
    const char *substr = strstr(itemName, myName.c_str());
    
    if ( substr == itemName)
      {
      char itemIndex[16];
      int startIndex = myName.length();
      int itemLength = strlen(itemName);
      int length = itemLength - startIndex;
      int currentIndex;
      
      useCounter = true;
      if (length == 0)
        {
        currentIndex = 0;
        }
      else
        {
        strncpy(itemIndex, &(itemName[startIndex+offset]), length-offset);
        itemIndex[length-offset] = '\0';
        currentIndex = atoi(itemIndex);
        }
      
      if (maxIndex < currentIndex) maxIndex = currentIndex;
      }
    }
  
  vtkIdType index = maxIndex + 1;
  if ( (useCounter) || (strcmp(namePrefix, "") != 0) )
    {
    imageActor->SetObjectName(namePrefix,index);
    }
  
  
  
  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( imageActor );
  //this->GetMimxMainWindow()->GetViewProperties()->UpdateVisibility();
}

//----------------------------------------------------------------------------
int vtkKWMimxGroupBase::UpdateSurfaceComboBox(vtkKWComboBox *combobox)
{
  combobox->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->SurfaceList->GetNumberOfItems(); i++)
    {
    combobox->AddValue( this->SurfaceList->GetItem(i)->GetFileName() );
    vtkMimxSurfacePolyDataActor *surfaceActor = vtkMimxSurfacePolyDataActor::SafeDownCast(this->SurfaceList->GetItem(i));
    int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(
                         this->SurfaceList->GetItem(i)->GetActor());
    if ( viewedItem )
      {
		  defaultItem = i;
      }
    }
  
  if ((this->SurfaceList->GetNumberOfItems() > 0) && (defaultItem == -1))
    {
    defaultItem = this->SurfaceList->GetNumberOfItems()-1;
    }
  
  if (defaultItem != -1)
    {
    combobox->SetValue(this->SurfaceList->GetItem(defaultItem)->GetFileName());
    }
  
  return defaultItem;
}

//----------------------------------------------------------------------------
int vtkKWMimxGroupBase::UpdateFeatureComboBox(vtkKWComboBox *combobox)
{
  combobox->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->FeatureList->GetNumberOfItems(); i++)
    {
    combobox->AddValue( this->FeatureList->GetItem(i)->GetFileName() );
    int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(this->FeatureList->GetItem(i)->GetActor());
    if ( viewedItem )
      {
		  defaultItem = i;
      }
    }
  
  if ((this->FeatureList->GetNumberOfItems() > 0) && (defaultItem == -1))
    {
    defaultItem = this->FeatureList->GetNumberOfItems()-1;
    }
  
  if (defaultItem != -1)
    {
    combobox->SetValue(this->FeatureList->GetItem(defaultItem)->GetFileName());
    }
  
  return defaultItem;
}

//----------------------------------------------------------------------------
int vtkKWMimxGroupBase::UpdateMeshComboBox(vtkKWComboBox *combobox)
{
  combobox->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
    {
    combobox->AddValue( this->FEMeshList->GetItem(i)->GetFileName() );
    vtkMimxMeshActor *meshActor = vtkMimxMeshActor::SafeDownCast(this->FEMeshList->GetItem(i));
    bool viewedItem = meshActor->GetMeshVisibility();
    if ( viewedItem )
      {
		  defaultItem = i;
      }
    }
  
  if ((this->FEMeshList->GetNumberOfItems() > 0) && (defaultItem == -1))
    {
    defaultItem = this->FEMeshList->GetNumberOfItems()-1;
    }
  
  if (defaultItem != -1)
    {
    combobox->SetValue(this->FEMeshList->GetItem(defaultItem)->GetFileName());
    }
  
  return defaultItem;
}

//----------------------------------------------------------------------------
int vtkKWMimxGroupBase::UpdateBuildingBlockComboBox(vtkKWComboBox *combobox)
{
  combobox->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->BBoxList->GetNumberOfItems(); i++)
    {
    combobox->AddValue(this->BBoxList->GetItem(i)->GetFileName() );
    vtkMimxUnstructuredGridActor *bbActor = vtkMimxUnstructuredGridActor::SafeDownCast(this->BBoxList->GetItem(i));
    int viewedItem = this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer()->HasViewProp(bbActor->GetActor());
    if ( viewedItem )
      {
		  defaultItem = i;
      }
    }
  
  if ((this->BBoxList->GetNumberOfItems() > 0) && (defaultItem == -1))
    {
    defaultItem = this->BBoxList->GetNumberOfItems()-1;
    }
  
  if (defaultItem != -1)
    {
    combobox->SetValue(
                       this->BBoxList->GetItem(defaultItem)->GetFileName());
    }
  
  return defaultItem;
}

//----------------------------------------------------------------------------
int vtkKWMimxGroupBase::UpdateImageComboBox(vtkKWComboBox *combobox)
{
  combobox->DeleteAllValues();
  
  int defaultItem = -1;
  for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
    {
    combobox->AddValue(this->ImageList->GetItem(i)->GetFileName());
    int viewedItem = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(i))->GetActorVisibility();
    if ( viewedItem ) 
      {
      defaultItem = i;
      }
    }
  
  if ((this->ImageList->GetNumberOfItems() > 0) && (defaultItem == -1))
    {
    defaultItem = this->ImageList->GetNumberOfItems()-1;
    }
  
  if (defaultItem != -1)
    {
    combobox->SetValue(this->ImageList->GetItem(defaultItem)->GetFileName());
    }
  
  return defaultItem;
}

