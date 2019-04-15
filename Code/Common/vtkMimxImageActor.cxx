/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxImageActor.cxx,v $
Language:  C++
Date:      $Date: 2013/03/16 21:24:42 $
Version:   $Revision: 1.12 $

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

#include "vtkMimxImageActor.h"

#include "vtkActor.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkMimxImagePlaneWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkImageReslice.h"
#include "vtkTransform.h"
#include "vtkImageChangeInformation.h"


vtkCxxRevisionMacro(vtkMimxImageActor, "$Revision: 1.12 $");
vtkStandardNewMacro(vtkMimxImageActor);

//--------------------------------------------------------------------------------
vtkMimxImageActor::vtkMimxImageActor()
{
	this->PlaneX = vtkMimxImagePlaneWidget::New();
	this->PlaneY = vtkMimxImagePlaneWidget::New();
	this->PlaneZ = vtkMimxImagePlaneWidget::New();
	this->RASToIJKTransform = vtkTransform::New();
  this->IJKToRASTransform = vtkTransform::New();
	this->LPSToIJKTransform = vtkTransform::New();
  this->IJKToLPSTransform = vtkTransform::New();
  this->ResliceFilter = vtkImageReslice::New();
  this->ItkToVtkImageFilter = ItkToVtkImageFilterType::New();
  
	this->Interactor = NULL;
  this->ItkImage = NULL;
  this->ResliceImage = NULL;
  this->InteractionFlag = 1;
  
  this->ImageResolution = 1.0;
  this->ImageDataType = MIMX_IMAGE_SIGNED_16BIT;
  this->SetDataType( ACTOR_IMAGE );
}

//--------------------------------------------------------------------------------
vtkMimxImageActor::~vtkMimxImageActor()
{
	this->PlaneX->Delete();
	this->PlaneY->Delete();
	this->PlaneZ->Delete();
	this->ItkToVtkImageFilter->Delete();
  this->RASToIJKTransform->Delete();
  this->IJKToRASTransform->Delete();
	this->LPSToIJKTransform->Delete();
  this->IJKToLPSTransform->Delete();
  this->ResliceFilter->Delete();
  if (this->ResliceImage)
    {
    this->ResliceImage->Delete();
    }
  this->ItkImage = NULL;
  
}

//--------------------------------------------------------------------------------
vtkImageData* vtkMimxImageActor::GetDataSet()
{
	return this->ItkToVtkImageFilter->GetOutput();
}

//--------------------------------------------------------------------------------
vtkImageData* vtkMimxImageActor::GetOrientedDataSet()
{
	return this->ResliceImage;
}

//--------------------------------------------------------------------------------
ImageType::Pointer vtkMimxImageActor::GetITKImage()
{
	return this->ItkImage;
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::SetITKImage(ImageType::Pointer image)
{
	this->ItkImage = image;
	this->ItkToVtkImageFilter->SetInput( this->ItkImage );
	this->ItkToVtkImageFilter->Update();
  
  this->InitializeDisplayWithImage();
}

//--------------------------------------------------------------------------------
int vtkMimxImageActor::GetActorVisibility()
{
	bool visible = false;
  if (this->PlaneX->GetEnabled()) visible = true;
  if (this->PlaneY->GetEnabled()) visible = true;
  if (this->PlaneZ->GetEnabled()) visible = true;
  return visible;
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::DisplayActor(int Display)
{
	this->PlaneX->SetEnabled(Display);
  this->PlaneX->SetInteraction(this->InteractionFlag);
  
	this->PlaneY->SetEnabled(Display);
  this->PlaneY->SetInteraction(this->InteractionFlag);
  
	this->PlaneZ->SetEnabled(Display);
  this->PlaneZ->SetInteraction(this->InteractionFlag);
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "PlaneX: " << this->PlaneX << std::endl;
  os << indent << "PlaneY: " << this->PlaneY << std::endl;
  os << indent << "PlaneZ: " << this->PlaneZ << std::endl;
  os << indent << "ITK Image: " << this->ItkImage << std::endl;
  os << indent << "Image Data Type: " << this->ImageDataType << std::endl;
  os << indent << "ItkToVtkImageFilter: " << this->ItkToVtkImageFilter << std::endl;
  os << indent << "Interactor: " << this->Interactor << std::endl;
  os << indent << "RASToIJKTransform: " << this->RASToIJKTransform << std::endl;
  os << indent << "IJKToRASTransform: " << this->IJKToRASTransform << std::endl;
  os << indent << "LPSToIJKTransform: " << this->LPSToIJKTransform << std::endl;
  os << indent << "IJKToLPSTransform: " << this->IJKToLPSTransform << std::endl;
  os << indent << "Resolution: " << this->ImageResolution << std::endl;
  os << indent << "InteractionFlag: " << this->InteractionFlag << std::endl;
  
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::SetInteraction(int Mode)
{
	this->InteractionFlag = Mode;
  
  if ( this->PlaneX->GetEnabled() )
    this->PlaneX->SetInteraction(Mode);
  
  if ( this->PlaneY->GetEnabled() )
	  this->PlaneY->SetInteraction(Mode);
  
  if ( this->PlaneZ->GetEnabled() )
	  this->PlaneZ->SetInteraction(Mode);
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::InitializeDisplayWithImage( )
{
  
  ////////////////////////////////////////////////
	// Handle Image Orientation
  ////////////////////////////////////////////////
	this->CreateCordinateSystemTransforms();
  
  vtkImageChangeInformation *changeInformation = vtkImageChangeInformation::New();
  changeInformation->SetOutputOrigin( 0.0, 0.0, 0.0 );
  changeInformation->SetOutputSpacing( 1.0, 1.0, 1.0 );
  changeInformation->SetInput( ItkToVtkImageFilter->GetOutput() );
  changeInformation->Update();
  
  this->ResliceFilter->SetInput( changeInformation->GetOutput() );
  this->ResliceFilter->SetOutputSpacing( this->ImageResolution, this->ImageResolution, this->ImageResolution );
  vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();
  this->RASToIJKTransform->GetMatrix(resliceAxes);
  this->ResliceFilter->SetResliceAxes( resliceAxes );
  this->ResliceFilter->Update();
  
  this->ResliceImage = this->ResliceFilter->GetOutput();
  
	int *extent = this->ResliceFilter->GetOutput()->GetExtent();
  
	////////////////////////////////////////////////
	// setting up the Image Viewers
  ////////////////////////////////////////////////
  
	// Sagittal Viewer
	PlaneX->SetInput(this->ResliceFilter->GetOutput());
	PlaneX->DisplayTextOn();
	PlaneX->SetPlaneOrientationToXAxes();
	PlaneX->RestrictPlaneToVolumeOn();
	PlaneX->SetSliceIndex( (extent[0]+extent[1])/2 );
  //	PlaneX->SetPicker(CellPicker);
	PlaneX->KeyPressActivationOn();
  PlaneX->SetKeyPressActivationValue('x');
  PlaneX->SetInteractor(NULL);
  PlaneX->SetInteractor(Interactor);
	PlaneX->GetPlaneProperty()->SetColor(1,0,0);
	PlaneX->On();
  PlaneX->SetIJKToLPSTransform( this->IJKToLPSTransform );
  
	// Coronal Viewer
	PlaneY->SetInput(this->ResliceFilter->GetOutput());
	PlaneY->DisplayTextOn();
	PlaneY->SetPlaneOrientationToYAxes();
	PlaneY->RestrictPlaneToVolumeOn();
	PlaneY->SetSliceIndex( (extent[2]+extent[3])/2 );
  //	PlaneY->SetPicker(CellPicker);
  PlaneY->KeyPressActivationOn();
	PlaneY->SetKeyPressActivationValue('y');
	PlaneY->GetPlaneProperty()->SetColor(0,1,0);
	PlaneY->SetInteractor(Interactor);
	PlaneY->On();
  PlaneY->SetIJKToLPSTransform( this->IJKToLPSTransform );
  
  // Axial Viewer
	PlaneZ->SetInput(this->ResliceFilter->GetOutput());
	PlaneZ->DisplayTextOn();
	PlaneZ->SetPlaneOrientationToZAxes();
	PlaneZ->RestrictPlaneToVolumeOn();
	PlaneZ->SetSliceIndex( (extent[4]+extent[5])/2 );
  //	PlaneZ->SetPicker(CellPicker);
  PlaneZ->KeyPressActivationOn();
	PlaneZ->SetKeyPressActivationValue('z');
	PlaneZ->GetPlaneProperty()->SetColor(0,0,1);
	PlaneZ->SetInteractor(Interactor);
	PlaneZ->On();
  PlaneZ->SetIJKToLPSTransform( this->IJKToLPSTransform );
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::CreateCordinateSystemTransforms()
{
  this->CreateRASTransforms();
  this->CreateLPSTransforms();
}

//--------------------------------------------------------------------------------
// Create the transforms that map from RAS To IJK and IJK to RAS
// This code was based on the Slicer3 ArchTypeSeriesReader
void vtkMimxImageActor::CreateRASTransforms()
{
  vtkMatrix4x4* IjkToLpsMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4* RasToIjkMatrix = vtkMatrix4x4::New();
  
  RasToIjkMatrix->Identity();
  IjkToLpsMatrix->Identity();
  
  double spacing[3];
  double origin[3];
  
  for (int i = 0; i < 3; i++)
    {
    spacing[i] = this->ItkImage->GetSpacing()[i];
    origin[i] = this->ItkImage->GetOrigin()[i];
    // Get IJK to RAS direction vector
    for ( unsigned int j=0; j < 3; j++ )
      {
      IjkToLpsMatrix->SetElement(j, i, this->ItkImage->GetSpacing()[i]*this->ItkImage->GetDirection()[j][i]);
      }
    }
  
  vtkMatrix4x4* LpsToRasMatrix = vtkMatrix4x4::New();
  LpsToRasMatrix->Identity();
  LpsToRasMatrix->SetElement(0,0,-1);
  LpsToRasMatrix->SetElement(1,1,-1);
  
  vtkMatrix4x4::Multiply4x4(LpsToRasMatrix,IjkToLpsMatrix, RasToIjkMatrix);
  
  origin[0] *= -1;   // L -> R
  origin[1] *= -1;   // P -> A
  
  
  for (int j = 0; j < 3; j++)
    {
    RasToIjkMatrix->SetElement(j, 3, origin[j]);
    }
  RasToIjkMatrix->Invert();
  
  this->RASToIJKTransform->Identity ();
  this->RASToIJKTransform->SetMatrix( RasToIjkMatrix );
  this->IJKToRASTransform = vtkTransform::SafeDownCast( this->RASToIJKTransform->GetInverse() );
}

//--------------------------------------------------------------------------------
// Create the transforms that map from LPS To IJK and IJK to LPS
void vtkMimxImageActor::CreateLPSTransforms()
{
  vtkMatrix4x4* IjkToLpsMatrix = vtkMatrix4x4::New();
  IjkToLpsMatrix->Identity();
  
  double spacing[3];
  double origin[3];
  
  for (int i = 0; i < 3; i++)
    {
    spacing[i] = this->ItkImage->GetSpacing()[i];
    origin[i] = this->ItkImage->GetOrigin()[i];

    // Get IJK to RAS direction vector
    for ( unsigned int j=0; j < 3; j++ )
      {
      IjkToLpsMatrix->SetElement(j, i, this->ItkImage->GetSpacing()[i]*this->ItkImage->GetDirection()[j][i]);
      }
    }
  
  for (int j = 0; j < 3; j++)
    {
    IjkToLpsMatrix->SetElement(j, 3, origin[j]);
    }
  
  this->IJKToLPSTransform->Identity ();
  this->IJKToLPSTransform->SetMatrix( IjkToLpsMatrix );
  this->LPSToIJKTransform = vtkTransform::SafeDownCast( this->IJKToLPSTransform->GetInverse() );
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::RasToIjkTransformPoint(double rasPoint[3], double ijkPoint[3])
{
  this->RASToIJKTransform->TransformPoint(rasPoint, ijkPoint);
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::IjkToRasTransformPoint(double ijkPoint[3], double rasPoint[3])
{
  this->IJKToRASTransform->TransformPoint(ijkPoint, rasPoint);
}
//--------------------------------------------------------------------------------
void vtkMimxImageActor::LpsToIjkTransformPoint(double lpsPoint[3], double ijkPoint[3])
{
  this->LPSToIJKTransform->TransformPoint(lpsPoint, ijkPoint);
}
//--------------------------------------------------------------------------------
void vtkMimxImageActor::IjkToLpsTransformPoint(double ijkPoint[3], double lpsPoint[3])
{
  this->IJKToLPSTransform->TransformPoint(ijkPoint, lpsPoint);
}
//--------------------------------------------------------------------------------
vtkMimxImagePlaneWidget* vtkMimxImageActor::GetXaxisImageActorWidget()
{
	return this->PlaneX;
}
//--------------------------------------------------------------------------------
vtkMimxImagePlaneWidget* vtkMimxImageActor::GetYaxisImageActorWidget()
{
	return this->PlaneY;
}
//--------------------------------------------------------------------------------
vtkMimxImagePlaneWidget* vtkMimxImageActor::GetZaxisImageActorWidget()
{
	return this->PlaneZ;
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::SetImageResolution(double resolution)
{
	this->ImageResolution = resolution;
  if (this->ResliceImage != NULL)
    {
    this->ResliceFilter->SetOutputSpacing( this->ImageResolution, this->ImageResolution, this->ImageResolution );
    this->ResliceFilter->UpdateWholeExtent();
    this->ResliceFilter->Update();
    
    int *extent = this->ResliceFilter->GetOutput()->GetExtent();
    
    this->PlaneX->SetSliceIndex( extent[1]/2 );
    this->PlaneY->SetSliceIndex( extent[3]/2 );
    this->PlaneZ->SetSliceIndex( extent[5]/2 );
    }
}

//--------------------------------------------------------------------------------
int vtkMimxImageActor::GetAxisActorVisibility(int Axis)
{
	switch(Axis)
	{
		case 0:
      if (this->PlaneX->GetEnabled()) return 1;
      break;
		case 1:
      if (this->PlaneY->GetEnabled()) return 1;
      break;
		case 2:
      if (this->PlaneZ->GetEnabled()) return 1;
      break;
	}
  return 0;
}

//--------------------------------------------------------------------------------
void vtkMimxImageActor::DisplayAxisActor(int Axis, int Display)
{
	//Allows control over individual planes
	switch(Axis)
	{
		case 0:
      this->PlaneX->SetEnabled(Display);
      break;
		case 1:
      this->PlaneY->SetEnabled(Display);
      break;
		case 2:
      this->PlaneZ->SetEnabled(Display);
      break;
	}
}
