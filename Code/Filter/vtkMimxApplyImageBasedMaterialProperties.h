/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxApplyImageBasedMaterialProperties.h,v $
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

// .NAME vtkMimxApplyImageBasedMaterialProperties - Apply material properties based on 
// image intensity
// .SECTION Description
// vtkMimxApplyImageBasedMaterialProperties is a filter that takes an unstructured grid 
// and itkImage as inputs. The filter calculates material property value based on image
// intensity values.
// Cell Data scalar named Material_Properties is attached to the output.

// .SECTION See Also

#ifndef __vtkMimxApplyImageBasedMaterialProperties_h
#define __vtkMimxApplyImageBasedMaterialProperties_h

//VTK Headers
#include "vtkUnstructuredGridAlgorithm.h"

//ITK Headers
#include "itkImage.h"
#include "itkOrientedImage.h"
#include "itkIndex.h"

//MIMX Headers
#include "vtkMimxFilterWin32Header.h"


class vtkUnstructuredGrid;
typedef itk::Image<signed short, 3>  ImageType;
typedef itk::OrientedImage<signed short, 3> OrientImageType;
typedef itk::Index<3> IntegerType;


class VTK_MIMXFILTER_EXPORT vtkMimxApplyImageBasedMaterialProperties : public vtkUnstructuredGridAlgorithm
{
public:
  enum { AVERAGE=1, MEDIAN=2, MAXIMUM=3 };
  static vtkMimxApplyImageBasedMaterialProperties *New();
  vtkTypeRevisionMacro(vtkMimxApplyImageBasedMaterialProperties,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetITKImage(ImageType::Pointer);
  void SetITKOrientedImage(OrientImageType::Pointer);
  vtkSetMacro(ElementSetName, const char*);
  vtkSetMacro(ConstantA, double);
  vtkGetMacro(ConstantA, double);
  vtkSetMacro(ConstantB, double);
  vtkGetMacro(ConstantB, double);
  vtkSetMacro(ConstantC, double);
  vtkGetMacro(ConstantC, double);
  vtkSetMacro(IntensityCalculationMode, int);
  vtkGetMacro(IntensityCalculationMode, int);
protected:
  vtkMimxApplyImageBasedMaterialProperties();
  ~vtkMimxApplyImageBasedMaterialProperties();
  ImageType::Pointer ITKImage;
  OrientImageType::Pointer ITKOrientImage;
  
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int , vtkInformation *);
  double GetAverageSubRegionIntensityValue(IntegerType StartIndex, IntegerType EndIndex);
  double GetMedianSubRegionIntensityValue(IntegerType StartIndex, IntegerType EndIndex);
  double GetMaximumSubRegionIntensityValue(IntegerType StartIndex, IntegerType EndIndex);
  double CalculateMaterialProperties(double Value);
  const char *ElementSetName;
  double ConstantA;
  double ConstantB;
  double ConstantC;
  int IntensityCalculationMode;
private:
  vtkMimxApplyImageBasedMaterialProperties(const vtkMimxApplyImageBasedMaterialProperties&);  // Not implemented.
  void operator=(const vtkMimxApplyImageBasedMaterialProperties&);  // Not implemented.
};

#endif
