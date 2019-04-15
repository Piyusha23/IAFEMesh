/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkPatchSurfaceMeshUsingTPS.h,v $

  Copyright (c) University of Iowa
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPatchSurfaceMeshUsingTPSUsingTPS - patch-up holes in a surface data using 
// TPS transform
//
// .SECTION Description
// vtkPatchSurfaceMeshUsingTPSUsingTPS is a filter to patch holes in a surface data. 
// input and output is polydata. the hole being patched should be convex. the concave 
// hole might not be patched accurately.

#ifndef __vtkPatchSurfaceMeshUsingTPSUsingTPS_h
#define __vtkPatchSurfaceMeshUsingTPSUsingTPS_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkMimxFilterWin32Header.h"

class vtkPointLocator;
class vtkPolyData;
class vtkPolyDataCollection;
class vtkPoints ;
class vtkAppendFilter ;


class VTK_MIMXFILTER_EXPORT vtkPatchSurfaceMeshUsingTPS : public vtkPolyDataAlgorithm
{

public:
  static vtkPatchSurfaceMeshUsingTPS *New();
  vtkTypeRevisionMacro(vtkPatchSurfaceMeshUsingTPS,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
protected:
  vtkPatchSurfaceMeshUsingTPS();
  ~vtkPatchSurfaceMeshUsingTPS();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
  vtkPatchSurfaceMeshUsingTPS(const vtkPatchSurfaceMeshUsingTPS&);  // Not implemented.
  void operator=(const vtkPatchSurfaceMeshUsingTPS&);  // Not implemented.
};

#endif


