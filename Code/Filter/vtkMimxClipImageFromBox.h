/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxClipImageFromBox.h,v $
Language:  C++
Date:      $Date: 2013/01/02 20:39:31 $
Version:   $Revision: 1.2 $

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


#ifndef _vtkMimxClipImageFromBox_h
#define _vtkMimxClipImageFromBox_h

//VTK Headers
#include "vtkBoxWidget.h"
#include "vtkImagePlaneWidget.h"
#include "vtkRenderWindowInteractor.h"

//ITK Headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkResampleImageFilter.h"

//MIMX Headers
#include "vtkMimxFilterWin32Header.h"

typedef itk::Image<signed short, 3>  ImageType;

//	class for storing polydata generated from Tracing
class VTK_MIMXFILTER_EXPORT vtkMimxClipImageFromBox 
{
public:

	typedef itk::ImageFileReader<ImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter<ImageType> FilterType;
	ReaderType::Pointer Reader;
	FilterType::Pointer	Filter;
	typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
	ResampleFilterType::Pointer ResampleFilter;

	vtkMimxClipImageFromBox();
	~vtkMimxClipImageFromBox();
	void SetInteractor(vtkRenderWindowInteractor *Iren);
	void SetInputModeToFileName(int InputMode);
	void SetFileName(const char* FileName);
	void SetInput(ImageType* InputImage);
	ImageType* GetitkImageOutput();
	void EnableImagePlaneWidget(int PlaneWidgetMode);
	void EnableBoxWidget(int BoxWidgetMode);
	void DisplayWidgets();
	void ClipImage();
	void ShowWidget(int VisibleMode);
	ImageType* GetitkImageInput();

private:
	vtkBoxWidget *BoxWidget;
	vtkImagePlaneWidget *PlaneX, *PlaneY, *PlaneZ;
	vtkCellPicker *CellPicker;
	vtkRenderWindowInteractor *Interactor;
	int BWMode;
	int PWmode;
	int IpMode;
	int VisMode;
	const char *FName;
	ImageType* IpImage;
	ImageType* OpImage;
protected:
};

#endif
