/*=========================================================================
 
 Program:   MIMX Meshing Toolkit
 Module:    $RCSfile: vtkMimxCreateVoxelMesh.h,v $
 Language:  C++
 Date:      $Date: 2013/04/04 02:25:56 $
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

// .NAME vtkMimxCreateVoxelMesh - Create a Hexadral Mesh from a VTK image
// .SECTION Description
//  vtkMimxCreateVoxelMesh generates a Hexahedron mesh from an image. 
//  It is assumed that all voxels greater than 0 will be included in
//  the mesh. The user can subsample the image to generate a less 
//  dense mesh. In this case the number of voxels in the image above 0
//  is counted. If the number exceeds the user defined threshold, then
//  the hex is included in the mesh.
//  Some of the ideas imnplemented here are based on the work 
//  in the following paper.
//
//    Grosland NM, Brown TD
//    A voxel-based formulation for contact finite element analysis.
//    Comput Methods Biomech Biomed Engin. 2002 Feb;5(1):21-32
//
//  The implementation in this class is based in part by vtkMarchingCubes
//
// .SECTION See Also 
//  vtkMarchingCubes vtkUnstructuredGridAlgorithm

#ifndef __vtkMimxCreateVoxelMesh_h
#define __vtkMimxCreateVoxelMesh_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkMimxFilterWin32Header.h"

class vtkPointLocator;


class VTK_MIMXFILTER_EXPORT vtkMimxCreateVoxelMesh : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkMimxCreateVoxelMesh *New();
  vtkTypeRevisionMacro(vtkMimxCreateVoxelMesh,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Manual bounds setting
  void SetResampleSize(int index, int size);
  int GetResampleSize(int index);

  // Description:
  // Get/Set the number of voxels required to be included in the mesh.
  // This only makes sense for subsampled data where we count number 
  // of voxels in the susampled region.
  vtkSetMacro(InclusionVoxelCount, int);
  vtkGetMacro(InclusionVoxelCount, int);
  
  // Description:
  // Get/Set the staring node number for the mesh
  vtkSetMacro(StartingNodeNumber, int);
  vtkGetMacro(StartingNodeNumber, int);
  
  // Description:
  // Get/Set the staring element number for the mesh
  vtkSetMacro(StartingElementNumber, int);
  vtkGetMacro(StartingElementNumber, int);
  
  // Description:
  // Get/Set the default Element set name.
  void SetElementSetName(const char *setName);
  void GetElementSetName(char *setName);
  
  // Description:
  // Get/Set the default Element set name.
  void SetNodeSetName(const char *setName);
  void GetNodeSetName(char *setName);
  
  // Description:
  // Overide the default locator.  Useful for changing the number of
  // bins for performance or specifying a more aggressive locator.
  void SetLocator(vtkPointLocator *locator);
  vtkGetObjectMacro(Locator,vtkPointLocator);

  // Description:
  // Create default locator. Used to create one when none is
  // specified. The locator is used to merge coincident points.
  void CreateDefaultLocator();
  
protected:
  vtkMimxCreateVoxelMesh();
  ~vtkMimxCreateVoxelMesh();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  int ResampleSize[3];
  int InclusionVoxelCount;
  vtkPointLocator *Locator;
  int StartingNodeNumber;
  int StartingElementNumber;
  std::string ElementSetName;
  std::string NodeSetName;

private:
  vtkMimxCreateVoxelMesh(const vtkMimxCreateVoxelMesh&);  // Not implemented.
  void operator=(const vtkMimxCreateVoxelMesh&);  // Not implemented.
  
  
};

#endif

