/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateVoxelMesh.cxx,v $
Language:  C++
Date:      $Date: 2013/04/04 02:44:10 $
Version:   $Revision: 1.2 $

 Musculoskeletal Imaging, Modelling and Experimentation (MIMX)
 Center for Computer Aided Design
 The University of Iowa
 Iowa City, IA 52242
 http://www.ccad.uiowa.edu/mimx/
 
Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWMimxCreateVoxelMesh.h"

//VTK Headers
#include <vtkMimxImageActor.h>
#include <vtkMimxMeshActor.h>
#include <vtkMimxErrorCallback.h>
#include <vtkObjectFactory.h>
#include <vtkMimxApplyNodeElementNumbers.h>
#include <vtkStringArray.h>
#include <vtkCellTypes.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkPointSet.h>
#include <vtkCellData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>

//KWWidgets Headers
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWPushButton.h>
#include <vtkKWRenderWidget.h>

//MIMX Headers
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxNodeElementNumbersGroup.h"
#include "vtkKWMimxMainUserInterfacePanel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxCreateVoxelMesh);
vtkCxxRevisionMacro(vtkKWMimxCreateVoxelMesh, "$Revision: 1.2 $");

//----------------------------------------------------------------------------
vtkKWMimxCreateVoxelMesh::vtkKWMimxCreateVoxelMesh()
{
  this->ResampleFactorEntry = NULL;
  this->ComponentFrame = NULL;
  this->ImageListComboBox = NULL;
  this->LabelMapButton = NULL;
  this->LabelMapValueEntry = NULL;
  this->NodeElementNumbersGroup=NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxCreateVoxelMesh::~vtkKWMimxCreateVoxelMesh()
{
  if(this->ResampleFactorEntry)
     this->ResampleFactorEntry->Delete();
  if (this->ComponentFrame)
	  this->ComponentFrame->Delete();
  if(this->ImageListComboBox)
	  this->ImageListComboBox->Delete();
	if(this->LabelMapButton)
	  this->LabelMapButton->Delete();
	if(this->LabelMapValueEntry)
	  this->LabelMapValueEntry->Delete();
  if(this->NodeElementNumbersGroup)
		this->NodeElementNumbersGroup->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::CreateWidget()
{
  if(this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->Superclass::CreateWidget();
  

  if ( !this->ComponentFrame )
    this->ComponentFrame = vtkKWFrameWithLabel::New();
  this->ComponentFrame->SetParent( this->MainFrame );
  this->ComponentFrame->Create();
  this->ComponentFrame->SetLabelText("Mask / Label Map");
  this->ComponentFrame->CollapseFrame();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ComponentFrame->GetWidgetName());
  
  //Image File Combobox
  if (!this->ImageListComboBox)	
      this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  ImageListComboBox->SetParent(this->ComponentFrame->GetFrame());
  ImageListComboBox->Create();
  ImageListComboBox->SetLabelText("Image : ");
  ImageListComboBox->SetLabelWidth(15);
  ImageListComboBox->GetWidget()->ReadOnlyOn();
  ImageListComboBox->GetWidget()->SetBalloonHelpString(
	  "Enter the image for voxel meshing");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    ImageListComboBox->GetWidgetName());

  //Resample Factor Entry
  if (!this->ResampleFactorEntry)
	  this->ResampleFactorEntry = vtkKWEntryWithLabel::New();
  this->ResampleFactorEntry->SetParent(this->MainFrame);
  this->ResampleFactorEntry->Create();
  this->ResampleFactorEntry->SetLabelWidth(30);
  this->ResampleFactorEntry->SetLabelText("Downsampling Factor: ");
  this->ResampleFactorEntry->GetWidget()->SetValue("1");
  this->ResampleFactorEntry->GetWidget()->SetBalloonHelpString(
	  "Downsampling for mesh (1-10)");
  this->ResampleFactorEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand y -padx 2 -pady 2 -fill x", 
	  this->ResampleFactorEntry->GetWidgetName());
  
  // Check button for Specifying label map value  
  if (!this->LabelMapButton)
	  this->LabelMapButton = vtkKWCheckButtonWithLabel::New();
	this->LabelMapButton->SetParent(this->MainFrame);
  this->LabelMapButton->Create();
  this->LabelMapButton->SetLabelText("Label Map");
  this->LabelMapButton->SetLabelPositionToLeft();
  this->LabelMapButton->GetWidget()->SetCommand(this, "LabelMapCallback");
  this->LabelMapButton->GetWidget()->SelectedStateOff();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6", 
	  this->LabelMapButton->GetWidgetName());
  
  // Label map value for mesh generation   
  if (!this->LabelMapValueEntry)
	  this->LabelMapValueEntry = vtkKWEntryWithLabel::New();
	this->LabelMapValueEntry->SetParent( this->MainFrame );
	this->LabelMapValueEntry->Create();
	this->LabelMapValueEntry->SetLabelWidth(30);
	this->LabelMapValueEntry->SetLabelText("Label Map Value :");
	this->LabelMapValueEntry->GetWidget()->SetValue("1");
	this->LabelMapValueEntry->GetWidget()->SetBalloonHelpString(
	  "Value in label map for mesh generation");
	this->LabelMapValueEntry->GetWidget()->SetRestrictValueToInteger();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -fill x", 
    this->LabelMapValueEntry->GetWidgetName());  
	this->LabelMapValueEntry->SetEnabled( 0 );
	
  
  if (!this->NodeElementNumbersGroup)
    this->NodeElementNumbersGroup = vtkKWMimxNodeElementNumbersGroup::New();
  this->NodeElementNumbersGroup->SetParent(this->MainFrame);
  //this->NodeElementNumbersGroup->SetUseFrameWithSlider( 0 );
  //this->NodeElementNumbersGroup->SetUseApplyCancelButtons( 0 );
  this->NodeElementNumbersGroup->Create();
  this->GetApplication()->Script(
	  "pack %s -side top -anchor nw -expand n -padx 2 -pady 6", 
	  this->NodeElementNumbersGroup->GetWidgetName());

  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "VoxelMeshApplyCallback");
  this->CancelButton->SetCommand(this, "VoxelMeshCancelCallback");
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::Update()
{
	this->UpdateEnableState();
}
//---------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}
//----------------------------------------------------------------------------
int vtkKWMimxCreateVoxelMesh::VoxelMeshApplyCallback()
{
	vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();
	if(!strcmp(this->ImageListComboBox->GetWidget()->GetValue(),"") )
	{
		callback->ErrorMessage("Please select an image for mesh generation");
		return 0;
	}

	vtkKWComboBox *combobox = this->ImageListComboBox->GetWidget();
	std::string imageName = combobox->GetValue();
	

	int num = combobox->GetValueIndex(imageName.c_str());
	if(num < 0 || num > combobox->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Please choose a valid image");
		combobox->SetValue("");
		return 0;
	}

	//Get combobox
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(
	this->ImageList->GetItem(combobox->GetValueIndex(imageName.c_str())));

	//Get selected values
	int resampleSize = this->ResampleFactorEntry->GetWidget()->GetValueAsInt();
	if ((resampleSize < 1) || (resampleSize > 10))
	{
		callback->ErrorMessage("The resample facotr must be between 1 and 10");
		return 0;
	}

	int nodeNumber = this->NodeElementNumbersGroup->GetNodeNumberEntry()->GetValueAsInt();
	if (nodeNumber < 1 )
	{
		callback->ErrorMessage("Node numbers must be greater than 0");
		return 0;
	}
  
	int elementNumber = this->NodeElementNumbersGroup->GetElementNumberEntry()->GetValueAsInt();
	if (elementNumber < 1 )
	{
		callback->ErrorMessage("Element numbers must be greater than 0");
		return 0;
	}
  
	std::string nodeSetName = this->NodeElementNumbersGroup->GetNodeSetNameEntry()->GetValue();
	std::string elementSetName = this->NodeElementNumbersGroup->GetElementSetNameEntry()->GetValue();

	if ((nodeSetName.length() == 0) || (elementSetName.length() == 0))
	{
		callback->ErrorMessage("Please provide Node and Element Set Labels");
		return 0;
	} 

	//Typedefs used throughout the program
	typedef    signed short                               InputPixelType;
	typedef    itk::Image< InputPixelType,  3 >			      ImageType;
	typedef    ImageType::DirectionType                   DirectionType;
	typedef    ImageType::PointType                       PointType;


	/** Now Generate the Mesh */
	double cosineArray[9];
	double origin[3];
	double spacing[3];
	int k = 0;
	for (int i = 0; i < 3; i++)
  {
    spacing[i] = imageActor->GetITKImage()->GetSpacing()[i];
    origin[i] = imageActor->GetITKImage()->GetOrigin()[i];

    // Get IJK to RAS direction vector
    for ( int j=0; j < 3; j++ )
    {
      cosineArray[k] = imageActor->GetITKImage()->GetDirection()[j][i];
      k++;
    }
  }
  
	ImageType::SpacingType tmpSpacing;
	tmpSpacing[0] = tmpSpacing[1] = tmpSpacing[2] = 1.0;
	ImageType::PointType tmpOrigin;
	tmpOrigin[0] = tmpOrigin[1] = tmpOrigin[2] = 0.0;
	imageActor->GetITKImage()->SetSpacing( tmpSpacing );
	imageActor->GetITKImage()->SetOrigin( tmpOrigin );

#if 0
	typedef itk::MimxImageToVtkUnstructuredGridFilter< ImageType> ImageToMeshFilterType;
	ImageToMeshFilterType::Pointer imageToHexMeshFilter = ImageToMeshFilterType::New( );
	imageToHexMeshFilter->SetInput( imageActor->GetITKImage() );
	imageToHexMeshFilter->SetComputeMeshNodeNumberingOff( );  
	imageToHexMeshFilter->SetMeshResampleSize( resampleSize );
	imageToHexMeshFilter->Update( );
  imageToHexMeshFilter->GetOutput( );
#endif
  
	vtkUnstructuredGrid* finalMesh = vtkUnstructuredGrid::New();
	
	tmpSpacing[0] = spacing[0]; tmpSpacing[1] = spacing[1]; tmpSpacing[2] = spacing[2];
	tmpOrigin[0] = origin[0]; tmpOrigin[1] = origin[1]; tmpOrigin[2] = origin[2];
	imageActor->GetITKImage()->SetSpacing( tmpSpacing );
	imageActor->GetITKImage()->SetOrigin( tmpOrigin );
  
	vtkTransform *orientTransform = imageActor->GetIJKToRASTransform( );
  vtkTransformFilter *transformFilter = vtkTransformFilter::New();
  transformFilter->SetInput( finalMesh );
  transformFilter->SetTransform( orientTransform );
  transformFilter->Update( );
  
  vtkPointSet *transformedPoints = transformFilter->GetOutput( );
  
	//vtkUnstructuredGrid* finalMesh = vtkUnstructuredGrid::New();
	//finalMesh->DeepCopy( imageToHexMeshFilter->GetOutput( ) );
	finalMesh->SetPoints( transformedPoints->GetPoints( ) );

	//--> Stuff to save file with auto name
	callback->SetState(0);
	if ( imageName.length() > 0 )
	{
		std::string foundationname = this->ImageList->GetItem(combobox->GetValueIndex(imageName.c_str()))->GetFileName();
		this->Count++;
		this->InitializeMeshFieldData(finalMesh, nodeSetName.c_str(), elementSetName.c_str(), nodeNumber, elementNumber);
		AddMeshToDisplay(finalMesh, "VoxelMesh-", foundationname.c_str(), this->Count);
	}
	
	this->GetMimxMainWindow()->SetStatusText("Created Voxel Mesh");
	this->UpdateObjectLists();

  return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Resample Factor Entry" << std::endl;
  this->ResampleFactorEntry->PrintSelf(os, indent.GetNextIndent() );
  
  os << indent << "Component Frame" << std::endl;
  this->ComponentFrame->PrintSelf(os, indent.GetNextIndent() );
  
  os << indent << "Image List ComboBox" << std::endl;
  this->ImageListComboBox->PrintSelf(os, indent.GetNextIndent() );
  
  os << indent << "Label Map Button" << std::endl;
  this->LabelMapButton->PrintSelf(os, indent.GetNextIndent() );
  
  os << indent << "Lap Map Value Entry" << std::endl;
  this->LabelMapValueEntry->PrintSelf(os, indent.GetNextIndent() );
  
  os << indent << "Node/Element Number Group" << std::endl;
  this->NodeElementNumbersGroup->PrintSelf(os, indent.GetNextIndent() );
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::VoxelMeshCancelCallback()
{
	this->CancelStatus = 1;
  this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->SetEnabled(1);
	this->CancelStatus = 0;
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::LabelMapCallback( int state )
{
	if ( state )
	  this->LabelMapValueEntry->SetEnabled(1);
	else
	  this->LabelMapValueEntry->SetEnabled(0);
}
//-----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::UpdateObjectLists()
{
	this->UpdateImageComboBox( this->ImageListComboBox->GetWidget() );
	
	/* Set the Deafault Node and Element Numbers */
	int maxNodeNumber = 0;
	int maxElementNumber = 0;
	vtkUnstructuredGrid *grid = NULL;
	for (int i = 0; i < this->FEMeshList->GetNumberOfItems(); i++)
	{
		grid = vtkMimxMeshActor::SafeDownCast(this->FEMeshList->GetItem(i))->GetDataSet();
		vtkIntArray *nodeArray = vtkIntArray::SafeDownCast(grid->GetPointData()->GetArray("Node_Numbers"));
    vtkIntArray *elementArray = vtkIntArray::SafeDownCast(grid->GetCellData()->GetArray("Element_Numbers"));
    if ( nodeArray )
    {
      for (int j=0; j<nodeArray->GetNumberOfTuples(); j++)
      {
        if(nodeArray->GetValue(j) > maxNodeNumber)	maxNodeNumber = nodeArray->GetValue(j);
      }
    }
    if ( elementArray )
    {
      for (int j=0; j<elementArray->GetNumberOfTuples(); j++)
      {
        if(elementArray->GetValue(j) > maxElementNumber)	maxElementNumber = elementArray->GetValue(j);
      }
    }
	}
	
  maxNodeNumber++;
  maxElementNumber++;
  this->NodeElementNumbersGroup->GetNodeNumberEntry()->SetValueAsInt( maxNodeNumber );
	this->NodeElementNumbersGroup->GetElementNumberEntry()->SetValueAsInt( maxElementNumber );
	this->NodeElementNumbersGroup->GetNodeSetNameEntry()->SetValue( "" );
	this->NodeElementNumbersGroup->GetElementSetNameEntry()->SetValue( "" );
	
}
//----------------------------------------------------------------------------------------
	//Create new mesh file
void vtkKWMimxCreateVoxelMesh::AddMeshToDisplay(
	vtkUnstructuredGrid *mesh, const char *namePrefix, const char *FoundationName, int index)

{
  vtkMimxMeshActor *meshActor = vtkMimxMeshActor::New();
  meshActor->SetFoundationName(FoundationName);
  this->FEMeshList->AppendItem( meshActor );
	meshActor->SetDataSet( mesh );
	meshActor->SetRenderer( this->GetMimxMainWindow()->GetRenderWidget()->GetRenderer() );
  
  //this->Count++;
  vtkIdType idNumber = index;
  meshActor->SetObjectName( namePrefix, idNumber);
  // check if triangle or quad elements exist
  vtkCellTypes *cellTypes = vtkCellTypes::New();
  meshActor->GetDataSet()->GetCellTypes(cellTypes);

  this->GetMimxMainWindow()->GetRenderWidget()->Render();
  this->GetMimxMainWindow()->GetRenderWidget()->ResetCamera();
  
  int itemIndex = this->FEMeshList->GetNumberOfItems()-1;
	this->GetMimxMainWindow()->GetViewProperties()->AddObjectList( this->FEMeshList->GetItem(itemIndex) );    
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateVoxelMesh::InitializeMeshFieldData(vtkUnstructuredGrid *mesh, 
                const char *nodeSetName, const char *elementSetName, int nodeNumber, int elementNumber)
{
  vtkMimxApplyNodeElementNumbers *apply = new vtkMimxApplyNodeElementNumbers;
	apply->SetUnstructuredGrid( mesh );
	apply->SetNodeSetName( nodeSetName );
	apply->SetStartingNodeNumber( nodeNumber );
	apply->ApplyNodeNumbers();
	apply->SetElementSetName( elementSetName );
	apply->SetStartingElementNumber( elementNumber );
	apply->ApplyElementNumbers();
	delete apply;
  
  // creation of a new field data for element set storage
  vtkStringArray *elsetarray = vtkStringArray::New();
  elsetarray->SetName("Element_Set_Names");
  elsetarray->InsertNextValue( elementSetName );

  vtkStringArray *nodesetarray = vtkStringArray::New();
  nodesetarray->SetName("Node_Set_Names");
  nodesetarray->InsertNextValue( nodeSetName );

  mesh->GetFieldData()->AddArray(elsetarray);
  elsetarray->Delete();

  mesh->GetFieldData()->AddArray(nodesetarray);
  nodesetarray->Delete();

  vtkIntArray *BoundCond = vtkIntArray::New();
  BoundCond->SetNumberOfValues(1);
  BoundCond->SetValue(0,1);
  BoundCond->SetName("Boundary_Condition_Number_Of_Steps");
  mesh->GetFieldData()->AddArray(BoundCond);
  BoundCond->Delete();
}
