/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxImageActor.h,v $
Language:  C++
Date:      $Date: 2013/03/16 21:24:42 $
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
// .NAME vtkMimxImageActor - Actor representation for the image loaded.
// .SECTION Description
// vtkMimxImageActor is the pipeline setup for image display. The Actor
// will reorient the images and resample it to be isotropic before display.

#ifndef __vtkMimxImageActor_h
#define __vtkMimxImageActor_h

//ITK Headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

//MIMX Headers
#include "vtkMimxCommonWin32Header.h"
#include "vtkMimxActorBase.h"
#include "mimxCommonDefine.h"


class vtkImagePlaneWidget;
class vtkMimxImagePlaneWidget;
class vtkRenderWindowInteractor;
class vtkImageReslice;
class vtkTransform;

class VTK_MIMXCOMMON_EXPORT vtkMimxImageActor : public vtkMimxActorBase
{
public:
  static vtkMimxImageActor *New();
  vtkTypeRevisionMacro(vtkMimxImageActor,vtkMimxActorBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Get the image dataset. The dataset will be in VTK format.
  vtkImageData* GetDataSet();
  vtkImageData* GetOrientedDataSet();
  
  // Description:
  // Get the ITK image. This image will have the original image
  // orientation and spacing.
  ImageType::Pointer GetITKImage();
  
  // Description:
  // Set the ITK image. This image will be resampled to isotropic
  // resolution before display
  void SetITKImage(ImageType::Pointer image);
  
  // Description:
  // Set file path. This is used to name the dataset
  void SetITKImageFilePath(const char* FPath);
  
  // Description:
  // Interactor because vtkBoxWidget is being used for display
  void SetInteractor(vtkRenderWindowInteractor *Int)
  {
    this->Interactor = Int;
  }
  
  // Description:
  // Get/Set the Image actor visibility
  void DisplayActor(int Display);
  int GetActorVisibility();
  void DisplayAxisActor(int Axis, int Display);
  int GetAxisActorVisibility(int Axis);
  
  // Description:
  // Get/Set the Image actor visibility
  void SetInteraction (int Mode);
  
  // Description:
  // Get/Set the resolution of the images in the Image Viewer
  void SetImageResolution(double);
  vtkGetMacro(ImageResolution, double);
  
  // Description:
  // Get VTK Transform mapping from RAS position to IJK coordinates
  vtkTransform* GetRASToIJKTransform() {return RASToIJKTransform;};
  
  // Description:
  // Get VTK Transform mapping from IJK position to RAS coordinates
  vtkTransform* GetIJKToRASTransform() {return IJKToRASTransform;};
  
  // Description:
  // Get VTK Transform mapping from LPS position to IJK coordinates
  vtkTransform* GetLPSToIJKTransform() {return LPSToIJKTransform;};
  
  // Description:
  // Get VTK Transform mapping from IJK position to LPS coordinates
  vtkTransform* GetIJKToLPSTransform() {return IJKToLPSTransform;};
  
  // Description:
  // Transform RAS position to IJK coordinates
  void RasToIjkTransformPoint(double rasPoint[3], double ijkPoint[3]);
  
  // Description:
  // Transform IJK position to RAS coordinates
  void IjkToRasTransformPoint(double ijkPoint[3], double rasPoint[3]);
  
  // Description:
  // Transform LPS position to IJK coordinates
  void LpsToIjkTransformPoint(double lpsPoint[3], double ijkPoint[3]);
  
  // Description:
  // Transform IJK position to LPS coordinates
  void IjkToLpsTransformPoint(double ijkPoint[3], double lpsPoint[3]);
  
  // Description:
  // Transform RAS position to LPS coordinates
  void RasToLpsTransformPoint(double rasPoint[3], double lpsPoint[3]);
  
  // Description:
  // Get the image plane widget for each axis
  vtkMimxImagePlaneWidget* GetXaxisImageActorWidget();
  vtkMimxImagePlaneWidget* GetYaxisImageActorWidget();
  vtkMimxImagePlaneWidget* GetZaxisImageActorWidget();
  
protected:
  vtkMimxImageActor();
  ~vtkMimxImageActor();
  
  typedef itk::ImageToVTKImageFilter<ImageType> ItkToVtkImageFilterType;
  
  // Description:
  // This method is called internally to initialize the display
  // after an image is passed to the actor.
  void InitializeDisplayWithImage( );
  
  // Description:
  // This method is called internally to create all of the
  // coordinate system transformations based on the image
  // direction cosines. The image display is in RAS and the
  // ITK image is specified in LPS space.
  void CreateCordinateSystemTransforms();
  void CreateLPSTransforms();
  void CreateRASTransforms();
  
  // Description:
  // Internally used VTK filters used to display the image
  ItkToVtkImageFilterType::Pointer	ItkToVtkImageFilter;
  vtkMimxImagePlaneWidget *PlaneX;
  vtkMimxImagePlaneWidget *PlaneY;
  vtkMimxImagePlaneWidget *PlaneZ;
  vtkImageData *ResliceImage;
  vtkTransform *RASToIJKTransform;
  vtkTransform *IJKToRASTransform;
  vtkTransform *LPSToIJKTransform;
  vtkTransform *IJKToLPSTransform;
  vtkImageReslice *ResliceFilter;
  vtkRenderWindowInteractor *Interactor;
  
  // Description:
  // Internally used class variables
  double ImageResolution;
  ImageType::Pointer ItkImage;
  MIMXImageType ImageDataType;
  int InteractionFlag;

private:
  vtkMimxImageActor(const vtkMimxImageActor&);  // Not implemented.
  void operator=(const vtkMimxImageActor&);  // Not implemented.
};

#endif

