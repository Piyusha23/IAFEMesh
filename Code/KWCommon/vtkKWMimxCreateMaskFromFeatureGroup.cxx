/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxCreateMaskFromFeatureGroup.cxx,v $
Language:  C++
Date:      $Date: 2013/04/04 02:25:56 $
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
//For surface generation program
#include "vtkKWMimxCreateMaskFromFeatureGroup.h"

// ITK Headers
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkRigid3DTransform.h>
#include <itkResampleImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkIdentityTransform.h>

// VTK Headers
#include <vtkFieldData.h>
#include <vtkStringArray.h>
#include <vtkAbstractArray.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkTransformPolyDataFilter.h>

// KWWidget Headers
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWComboBoxWithLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWSpinBoxWithLabel.h>
#include <vtkKWSpinBox.h>

// MIMX Headers
#include "mimxCommonDefine.h"
#include "vtkMimxImageActor.h"
#include "vtkMimxFeaturePolyDataActor.h"
#include "vtkMimxErrorCallback.h"
#include "vtkKWMimxMainNotebook.h"
#include "vtkKWMimxMainUserInterfacePanel.h"
#include "PolygonFill.h"
#include "MyLineDraw.h"
#include "ManualTrace.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkKWMimxCreateMaskFromFeatureGroup);
vtkCxxRevisionMacro(vtkKWMimxCreateMaskFromFeatureGroup, "$Revision: 1.3 $");

//----------------------------------------------------------------------------
vtkKWMimxCreateMaskFromFeatureGroup::vtkKWMimxCreateMaskFromFeatureGroup()
{
  this->ImageListComboBox = NULL;
  this->FeatureListComboBox = NULL;
  this->ComponentFrame = NULL;
  this->ParameterFrame = NULL;
  this->FillValueEntry = NULL;
}

//----------------------------------------------------------------------------
vtkKWMimxCreateMaskFromFeatureGroup::~vtkKWMimxCreateMaskFromFeatureGroup()
{
  if(this->ImageListComboBox)
	  this->ImageListComboBox->Delete();
  if(this->FeatureListComboBox)
     this->FeatureListComboBox->Delete();
  if(this->ComponentFrame)
	  this->ComponentFrame->Delete();
  if(this->ParameterFrame)
	  this->ParameterFrame->Delete();
  if(this->FillValueEntry)
	  this->FillValueEntry->Delete();
}
//----------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::CreateWidget()
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
  this->ComponentFrame->SetLabelText("Image & Feature");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x", 
    this->ComponentFrame->GetWidgetName());
  this->ComponentFrame->CollapseFrame();

  if (!this->ImageListComboBox)	
      this->ImageListComboBox = vtkKWComboBoxWithLabel::New();
  this->ImageListComboBox->SetParent(this->ComponentFrame->GetFrame());
  this->ImageListComboBox->Create();
  this->ImageListComboBox->SetLabelText("Image : ");
  this->ImageListComboBox->SetLabelWidth(15);
  this->ImageListComboBox->GetWidget()->ReadOnlyOn();
  this->ImageListComboBox->GetWidget()->SetBalloonHelpString(
	  "Example image defining the resulting image space");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x", 
    this->ImageListComboBox->GetWidgetName());
  
  if (!this->FeatureListComboBox)
    this->FeatureListComboBox = vtkKWComboBoxWithLabel::New();
  this->FeatureListComboBox->SetParent(this->ComponentFrame->GetFrame());
  this->FeatureListComboBox->Create();
  this->FeatureListComboBox->SetLabelText("Feature : ");
  this->FeatureListComboBox->SetLabelWidth(15);
  this->FeatureListComboBox->GetWidget()->ReadOnlyOn();
  this->FeatureListComboBox->GetWidget()->SetBalloonHelpString(
    "Feature to convert to a mask");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
    this->FeatureListComboBox->GetWidgetName());
  

  //Frame for User Parameters
  if(!this->ParameterFrame)
	  this->ParameterFrame = vtkKWFrame::New();
  this->ParameterFrame->SetParent( this->MainFrame );
  this->ParameterFrame->Create();
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 6 -fill x",
    this->ParameterFrame->GetWidgetName());
  
  
  //Fill Value
  if (!this->FillValueEntry)
	  this->FillValueEntry = vtkKWSpinBoxWithLabel::New();
  this->FillValueEntry->SetParent(this->ParameterFrame);
  this->FillValueEntry->Create();
  this->FillValueEntry->SetLabelWidth(30);
  this->FillValueEntry->SetLabelText("Inside Value : ");
  this->FillValueEntry->GetWidget()->SetRestrictValueToInteger();
  this->FillValueEntry->GetWidget()->SetValue(1.0);
  this->FillValueEntry->GetWidget()->SetRange(1, 4095);
  this->FillValueEntry->GetWidget()->SetIncrement(1);
  this->FillValueEntry->GetWidget()->SetBalloonHelpString(
    "Image value for inside the ROI");
  this->GetApplication()->Script(
    "pack %s -side top -anchor nw -expand y -padx 2 -pady 6 -fill x",
     this->FillValueEntry->GetWidgetName());
  
  // Callbacks for Apply and Cancel Buttons
  this->ApplyButton->SetCommand(this, "CreateMaskFromFeatureApplyCallback");
  this->CancelButton->SetCommand(this, "CreateMaskFromFeatureCancelCallback");

}

//----------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::Update()
{
	this->UpdateEnableState();
}

//---------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::UpdateEnableState()
{
	this->UpdateObjectLists();
	this->Superclass::UpdateEnableState();
}

//----------------------------------------------------------------------------
int vtkKWMimxCreateMaskFromFeatureGroup::CreateMaskFromFeatureApplyCallback()
{
  vtkMimxErrorCallback *callback = this->GetMimxMainWindow()->GetErrorCallback();

  std::string imageName = this->ImageListComboBox->GetWidget()->GetValue();
  std::string featureName = this->FeatureListComboBox->GetWidget()->GetValue();
  std::cout << "Image Name: " << imageName << std::endl;
  std::cout << "Feature Name: " << featureName << std::endl;
  
	if (imageName == "" )
	{
		callback->ErrorMessage("Image selection required");
		return 0;
	}
  
  if (featureName == "" )
    {
		callback->ErrorMessage("Feature selection required");
		return 0;
    }

	int imageNumber = this->ImageListComboBox->GetWidget()->GetValueIndex(imageName.c_str());
  std::cout << "Image Number: " << imageNumber << std::endl;
	if (imageNumber < 0 || imageNumber > this->ImageListComboBox->GetWidget()->GetNumberOfValues()-1)
	{
		callback->ErrorMessage("Choose a valid image");
		return 0;
	}
  
  int featureNumber = this->FeatureListComboBox->GetWidget()->GetValueIndex(featureName.c_str());
  std::cout << "Image Number: " << featureNumber << std::endl;
	if (featureNumber < 0 || featureNumber > this->FeatureListComboBox->GetWidget()->GetNumberOfValues()-1)
    {
		callback->ErrorMessage("Choose a valid feature");
		return 0;
    }

	//Get combobox
	vtkMimxImageActor *imageActor = vtkMimxImageActor::SafeDownCast(this->ImageList->GetItem(imageNumber));
  vtkMimxFeaturePolyDataActor *featureActor = vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(imageNumber));
  vtkImageData *imageData = imageActor->GetOrientedDataSet();
  vtkPolyData *contours = featureActor->GetDataSet();
  
	//Get selected values
	ImagePixelType fillValue = static_cast<ImagePixelType>(this->FillValueEntry->GetWidget()->GetValue());
  
  
  //Check Input Parameter values
	if ( (fillValue > 4095) || (fillValue < 0) )
    {
	  callback->ErrorMessage("Fill value must be between 1 and 4095");
		return 0;
    }
	
  
  //Reassign axes for polygon fill (2D) based on coronal/sagittal/axial views
	int xAxis = 0;
	int yAxis = 2;
	int zAxis = 1;
	bool neg = false;
  
  int featurePlane = this->GetFeaturePlane(contours);
  
  switch (featurePlane) {
    case ManualTrace::Axial:
      xAxis = 0;
      yAxis = 1;
      zAxis = 2;
      neg = false;
      break;
    case ManualTrace::Coronal:
      xAxis = 0;
      yAxis = 2;
      zAxis = 1;
      neg = true;
      break;
    case ManualTrace::Sagittal:
      xAxis = 2;
      yAxis = 1;
      zAxis = 0;
      neg = true;
      break;
    default:
      xAxis = 0;
      yAxis = 1;
      zAxis = 2;
      neg = false;
      break;
  }
  
  
  
  // Get Resampled VTK Image Dimensions, Spacing, and Origin
  int dims[3];
  double origin[3];
  double spacing[3];
  imageData->GetDimensions( dims );
  imageData->GetOrigin( origin );
  imageData->GetSpacing( spacing );
  
  // Allocated ITK image of the same size
  ImageType::Pointer ijkImage = ImageType::New();
  ImageType::IndexType start;
  start[0] = 0; 
  start[1] = 0;
  start[2] = 0;
  
  ImageType::SizeType  size;
  size[0]  = dims[0];
  size[1]  = dims[1];
  size[2]  = dims[2];
  
  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
  
  ijkImage->SetRegions( region );
  
  ImageType::PointType displayOrigin;
  displayOrigin[0] = -1.0*origin[0];
  displayOrigin[1] = -1.0*origin[1];
  displayOrigin[2] = origin[2];
  ijkImage->SetOrigin( displayOrigin );
  
  ImageType::DirectionType direction;
  direction[0][0]=-1.0; direction[0][1]=0.0; direction[0][2]=0.0;
  direction[1][0]= 0.0; direction[1][1]=-1.0; direction[1][2]=0.0;
  direction[2][0]= 0.0; direction[2][1]=0.0; direction[2][2]=1.0;
  ijkImage->SetDirection(direction);
  
  ImageType::SpacingType displaySpacing;
  displaySpacing[0] = spacing[0];
  displaySpacing[1] = spacing[1];
  displaySpacing[2] = spacing[2];
  ijkImage->SetSpacing(displaySpacing);
  
  ijkImage->Allocate();
  ijkImage->FillBuffer( 0 );
  
  //Consider each individual contour separately
  for (int i = 0; i<contours->GetNumberOfCells(); i++)
		{
    vtkCell *cell = contours->GetCell(i);
    unsigned numPts = cell->GetNumberOfPoints();
    
    PolygonFill::PointListHeader *vList = new PolygonFill::PointListHeader(numPts);
    
    // The round the points to the nearest voxel index.
    int slicenum;
    int numberOfVoxels = 0;
    int j(0),lastX(99999),lastY(99999);
    for(unsigned k = 0; k < numPts; k++)
			{
      vtkIdType pointId = cell->GetPointId(k);
      
      double point[3] = {0,0,0};
      contours->GetPoints()->GetPoint(pointId,point);
      
      std::cout << "RAS Point (" << pointId << ") : " << point[0]<< " " << point[1] << " " << point[2] << endl;
      
      slicenum = vnl_math_rnd(abs(point[zAxis] - origin[zAxis]) / spacing[zAxis]);
      int x = vnl_math_rnd((point[xAxis] - origin[xAxis]) / spacing[xAxis]);
      int y = vnl_math_rnd((point[yAxis] - origin[yAxis]) / spacing[yAxis]);
      
      std::cout << "IJK Point: " << x << " " << y << " " << slicenum << endl;
      
      // polygon is in the plane of the slice
      
      if(x == lastX && y == lastY)
				{
        continue;
				}
      lastX =	vList->PointPtr[j].X = x;
      lastY = vList->PointPtr[j].Y = y;
      j++;
			}
    
    std::cout << "Number of POints: " << j << std::endl;
    vList->Length = j;
    
    MyLineDraw drawer(ijkImage,xAxis,yAxis,zAxis,slicenum,fillValue,&numberOfVoxels);
    PolygonFill::Fill(vList,&drawer);
    
		}
  
  std::cout << "Display Origin: " << ijkImage->GetOrigin() << std::endl;
  std::cout << "Display Direction: " << ijkImage->GetDirection() << std::endl;
  std::cout << "Display Spacing: " << ijkImage->GetSpacing() << std::endl;
  
  std::cout << "Image Origin: " << imageActor->GetITKImage()->GetOrigin() << std::endl;
  std::cout << "Image Direction: " << imageActor->GetITKImage()->GetDirection() << std::endl;
  std::cout << "Image Spacing: " << imageActor->GetITKImage()->GetSpacing() << std::endl;
  
  typedef itk::IdentityTransform<double, 3> IdentityTransformType;
  IdentityTransformType::Pointer identifyTransform = IdentityTransformType::New();
  identifyTransform->SetIdentity();
  
  typedef itk::ResampleImageFilter< ImageType, ImageType, double > ResampleImageFilterType;
  ResampleImageFilterType::Pointer resampleFilter = ResampleImageFilterType::New();
  resampleFilter->SetInput(ijkImage);
  resampleFilter->SetOutputSpacing( imageActor->GetITKImage()->GetSpacing() );
  resampleFilter->SetSize(imageActor->GetITKImage()->GetLargestPossibleRegion().GetSize());
  resampleFilter->SetOutputDirection(imageActor->GetITKImage()->GetDirection());
  resampleFilter->SetOutputOrigin(imageActor->GetITKImage()->GetOrigin());
  
  typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double > NearestNeighborInterpolatorType;
  NearestNeighborInterpolatorType::Pointer interpolator = NearestNeighborInterpolatorType::New();
  resampleFilter->SetInterpolator( interpolator );
  resampleFilter->SetTransform( identifyTransform );
  resampleFilter->Update();
  
  
  imageActor->DisplayActor(0);
  this->AddImageToDisplay(resampleFilter->GetOutput(), "Mask-", featureName.c_str(), 1, NULL);
  this->GetMimxMainWindow()->SetStatusText("Created Mask");
  
  this->UpdateObjectLists();
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::CreateMaskFromFeatureCancelCallback()
{
	this->UnpackGroup();
  this->MenuGroup->SetMenuButtonsEnabled(1);
  this->GetMimxMainWindow()->GetMainUserInterfacePanel()->GetMimxMainNotebook()->SetEnabled(1);
  
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::UpdateImageList()
{
  this->ImageListComboBox->GetWidget()->DeleteAllValues();
	
	int defaultItem = -1;
	for (int i = 0; i < this->ImageList->GetNumberOfItems(); i++)
    {
		this->ImageListComboBox->GetWidget()->AddValue(this->ImageList->GetItem(i)->GetFileName());
    
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
    this->ImageListComboBox->GetWidget()->SetValue(this->ImageList->GetItem(defaultItem)->GetFileName());
    }
}

//----------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::UpdateFeatureList()
{
  this->FeatureListComboBox->GetWidget()->DeleteAllValues();
	
  int defaultItem = -1;
  for (int i = 0; i < this->FeatureList->GetNumberOfItems(); i++)
    {
		vtkMimxFeaturePolyDataActor *actor = vtkMimxFeaturePolyDataActor::SafeDownCast(this->FeatureList->GetItem(i));
    vtkPolyData *feature = actor->GetDataSet();
    int featurePlane = this->GetFeaturePlane(feature);
    
    if (featurePlane != ManualTrace::Unknown )
      {
      this->FeatureListComboBox->GetWidget()->AddValue(this->FeatureList->GetItem(i)->GetFileName());
      int viewedItem = actor->GetVisibility();
      if ( (defaultItem == -1) && ( viewedItem ) )
        {
        defaultItem = i;
        }
      }
    }
  
  if (defaultItem != -1)
    {
    this->FeatureListComboBox->GetWidget()->SetValue(this->FeatureList->GetItem(defaultItem)->GetFileName());
    }
}

//----------------------------------------------------------------------------
int vtkKWMimxCreateMaskFromFeatureGroup::GetFeaturePlane(vtkPolyData *feature)
{
  int plane = ManualTrace::Unknown;
  
  vtkStringArray *stringArray = vtkStringArray::SafeDownCast(feature->GetFieldData()->GetAbstractArray("FEATURE-PLANE"));

  if (stringArray)
    {
    vtkStdString planeStr = stringArray->GetValue(0);
    
    if (planeStr == "AXIAL")
      {
      plane = ManualTrace::Axial;
      }
    if (planeStr == "CORONAL")
      {
      plane = ManualTrace::Coronal;
      }
    if (planeStr == "SAGITTAL")
      {
      plane = ManualTrace::Sagittal;
      }
    }
  
  return plane;
}

//------------------------------------------------------------------------------
void vtkKWMimxCreateMaskFromFeatureGroup::UpdateObjectLists()
{
	this->UpdateImageList();
  this->UpdateFeatureList();
}

