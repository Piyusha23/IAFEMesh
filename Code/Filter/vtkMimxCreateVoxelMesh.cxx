/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxCreateVoxelMesh.cxx,v $
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

#include "vtkMimxCreateVoxelMesh.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkAlgorithm.h>
#include <vtkPointLocator.h>
#include <vtkCellArray.h>
#include <vtkIntArray.h>
#include <vtkStringArray.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkMergePoints.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>


//-------------------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMimxCreateVoxelMesh, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMimxCreateVoxelMesh);

//-------------------------------------------------------------------------------------
vtkMimxCreateVoxelMesh::vtkMimxCreateVoxelMesh()
{
  this->ResampleSize[0]     = 1;
  this->ResampleSize[1]     = 1;
  this->ResampleSize[2]     = 1;
  this->InclusionVoxelCount = 1;
  this->Locator             = NULL;
  this->StartingNodeNumber  = 1;
  this->StartingElementNumber = 1;
  this->ElementSetName      = "Default";
  this->NodeSetName         = "Default";
}

//-------------------------------------------------------------------------------------
vtkMimxCreateVoxelMesh::~vtkMimxCreateVoxelMesh()
{
  if ( this->Locator )
  {
    this->Locator->UnRegister(this);
    this->Locator = NULL;
  }
}

//-------------------------------------------------------------------------------------
int vtkMimxCreateVoxelMesh::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}

//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Inclusion Voxel Count: " << this->InclusionVoxelCount << std::endl;
  os << indent << "Resample Size: " << this->ResampleSize[0];
  os << " " << this->ResampleSize[1] << " " << this->ResampleSize[2] << std::endl;
  os << indent << "Starting Node Number: " << this->StartingNodeNumber;
  os << indent << "Starting Element Number: " << this->StartingElementNumber;
  os << indent << "Node Set Name: " << this->NodeSetName;
  os << indent << "Element Set Name: " << this->ElementSetName;
  
  if ( this->Locator )
  {
    os << indent << "Locator:" << this->Locator << "\n";
    this->Locator->PrintSelf(os,indent.GetNextIndent());
  }
  else
  {
    os << indent << "Locator: (none)\n";
  }
    
}

//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::SetResampleSize(int index, int size)
{
  if (index < 3)
  {
    this->ResampleSize[index] = size;
  }
}

//-------------------------------------------------------------------------------------
int vtkMimxCreateVoxelMesh::GetResampleSize(int index)
{
  if (index < 3)
    return this->ResampleSize[index];
  else
    return 0;
}


//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::SetElementSetName(const char *setName)
{
  this->ElementSetName = setName;
}

//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::GetElementSetName(char *setName)
{
  strcpy(setName, this->ElementSetName.c_str() );
}

//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::SetNodeSetName(const char *setName)
{
  this->NodeSetName = setName;
}

//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::GetNodeSetName(char *setName)
{
  strcpy(setName, this->NodeSetName.c_str() );
}

//-------------------------------------------------------------------------------------
template <class T>
void vtkGenerateVoxelMesh(vtkMimxCreateVoxelMesh *self,T *scalars, int dims[3], 
                                     double origin[3], double spacing[3], int resample[3],
                                     vtkPointLocator *locator, 
                                     vtkCellArray *newCells, int requiredCount)
{
  
  int sliceSize = dims[0] * dims[1];
  vtkIdType ptIds[8];
  int extent[6];
  
  vtkInformation *inInfo = self->GetExecutive()->GetInputInformation(0, 0);
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent);
  
  self->UpdateProgress(0.0);
  
  double numSteps = static_cast<double> ( (dims[2]-resample[2])/resample[2] );
  
  for (int z=0; z < (dims[2]-resample[2]-1); z+=resample[2])
  {
    self->UpdateProgress (z / static_cast<double>(dims[2] - resample[2]-1));
    if (self->GetAbortExecute())
    {
      break;
    }
    
    for (int y=0;y<dims[1]-resample[1]-1;y+=resample[1])
    {
      for (int x=0;x<dims[0]-resample[0]-1;x+=resample[0])
      {
        int zmin=z;
        int zmax=z+resample[2];
        int count=0;
        for (int k=zmin;k<zmax;k++)
        {
          int ymin=y;
          int ymax=y+resample[1];
          for (int j=ymin;j<ymax;j++)
          {
            int xmin=x;
            int xmax=x+resample[0];
            for (int i=xmin;i<xmax;i++)
            {
              int index = sliceSize * k + dims[0] * j + i;
              if (scalars[index] > 0)
              {
                count ++;
              }
            }
          }
        }
        
        if (count >= requiredCount) 
        {
          double point[3];
          point[0] = (static_cast<double> ( x ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[0]);
          
          point[0] = (static_cast<double> ( x + resample[0] ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[1]);
          
          point[0] = (static_cast<double> ( x + resample[0] ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y + resample[1] ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[2]);
          
          point[0] = (static_cast<double> ( x ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y + resample[1] ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[3]);
          
          point[0] = (static_cast<double> ( x ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z + resample[2 ]) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[4]);
          
          point[0] = (static_cast<double> ( x + resample[0] ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z + resample[2] ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[5]);
          
          point[0] = (static_cast<double> ( x + resample[0] ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y + resample[1] ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z + resample[2] ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[6]);
          
          point[0] = (static_cast<double> ( x ) - 0.5) * spacing[0] + origin[0];
          point[1] = (static_cast<double> ( y + resample[1] ) - 0.5) * spacing[1] + origin[1];
          point[2] = (static_cast<double> ( z + resample[2] ) - 0.5) * spacing[2] + origin[2];
          locator->InsertUniquePoint(point, ptIds[7]);
          
          newCells->InsertNextCell(8,ptIds); 
                          
        }                
      }
    }
    self->UpdateProgress( static_cast<double> (z)/numSteps );
  }
   
}

//-------------------------------------------------------------------------------------                                        
int vtkMimxCreateVoxelMesh::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkImageData *input = vtkImageData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int dims[3], extent[6];
  int estimatedSize;
  double spacing[3], origin[3];
  double bounds[6];
  
  vtkDebugMacro(<< "Executing voxel mesh");

  //
  // Initialize and check input
  //
  vtkPointData *pd=input->GetPointData();
  if (pd ==NULL)
    {
    vtkErrorMacro(<<"PointData is NULL");
    return 1;
    }
  vtkDataArray *inScalars=pd->GetScalars();
  if ( inScalars == NULL )
    {
    vtkErrorMacro(<<"Scalars must be defined for mesh generation");
    return 1;
    }

  if ( input->GetDataDimension() != 3 )
    {
    vtkErrorMacro(<<"Cannot generate mesh for data of dimension != 3");
    return 1;
    }
  input->GetDimensions(dims);
  input->GetOrigin(origin);
  input->GetSpacing(spacing);
  
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
  
  // estimate the number of points from the volume dimensions
  estimatedSize = static_cast<int>(
    pow(static_cast<double>(dims[0]*dims[1]*dims[2]),0.75));
  estimatedSize = estimatedSize / 1024 * 1024; //multiple of 1024
  if (estimatedSize < 1024)
    {
    estimatedSize = 1024;
    }
  vtkDebugMacro(<< "Estimated allocation size is " << estimatedSize);
  
  vtkPoints *newPts = vtkPoints::New(); 
  newPts->Allocate(estimatedSize,estimatedSize/2);
  
  // compute bounds for merging points
  for ( int i=0; i<3; i++)
    {
    bounds[2*i] = origin[i] + extent[2*i] * spacing[i];
    bounds[2*i+1] = origin[i] + extent[2*i+1] * spacing[i];
    }
  if ( this->Locator == NULL )
    {
    this->CreateDefaultLocator();
      std::cout << "Create Default Locator" << std::endl;
    }
  this->Locator->InitPointInsertion (newPts, bounds, estimatedSize);
  
  vtkCellArray *newCells = vtkCellArray::New();
  newCells->Allocate(newCells->EstimateSize(estimatedSize,8));
  
  if (inScalars->GetNumberOfComponents() == 1 )
    {
    void* scalars = inScalars->GetVoidPointer(0);
    switch (inScalars->GetDataType())
      {
      vtkTemplateMacro(
        vtkGenerateVoxelMesh(this, static_cast<VTK_TT*>(scalars),
                                        dims, origin, spacing, 
                                        this->ResampleSize,
                                        this->Locator,
                                        newCells, this->InclusionVoxelCount)
        );
      } //switch
    }
  
  output->SetPoints(newPts);
  newPts->Delete();

  output->SetCells(VTK_HEXAHEDRON, newCells);
  newCells->Delete();
  output->Squeeze();
  

  vtkStringArray *elementSetNames = vtkStringArray::New();
  elementSetNames->SetName("Element_Set_Names");
  elementSetNames->InsertNextValue(this->ElementSetName.c_str());

  unsigned int numberOfCells = output->GetNumberOfCells();
  vtkIntArray *elementSetArray = vtkIntArray::New();
  elementSetArray->SetNumberOfValues( numberOfCells );
  elementSetArray->SetName(this->ElementSetName.c_str());
  
  vtkIntArray *elementSetNumberArray = vtkIntArray::New();
  elementSetNumberArray->SetNumberOfValues( numberOfCells );
  elementSetNumberArray->SetName("Element_Numbers");
  for (unsigned int i=0;i<numberOfCells;i++)
  {
    elementSetArray->SetValue(i,1);
    elementSetNumberArray->SetValue(i,this->StartingElementNumber+i);
  }
  output->GetCellData()->AddArray(elementSetArray);
  output->GetCellData()->AddArray(elementSetNumberArray);
  output->GetFieldData()->AddArray(elementSetNames);
  
  vtkStringArray *nodeSetNames = vtkStringArray::New();
  nodeSetNames->SetName("Node_Set_Names");
  nodeSetNames->InsertNextValue(this->NodeSetName.c_str());

  unsigned int numberOfPoints = output->GetNumberOfPoints();
  vtkIntArray *nodeSetArray = vtkIntArray::New();
  nodeSetArray->SetNumberOfValues( numberOfPoints );
  nodeSetArray->SetName(this->NodeSetName.c_str());
  
  vtkIntArray *nodeSetNumberArray = vtkIntArray::New();
  nodeSetNumberArray->SetNumberOfValues( numberOfPoints );
  nodeSetNumberArray->SetName("Node_Numbers");
  
  for (unsigned int i=0;i<numberOfPoints;i++)
  {
    nodeSetArray->SetValue(i,1);
    nodeSetNumberArray->SetValue(i,this->StartingNodeNumber+i);
  }
  output->GetPointData()->AddArray(nodeSetArray);
  output->GetPointData()->AddArray(nodeSetNumberArray);
  output->GetFieldData()->AddArray(nodeSetNames);
  
  
  if (this->Locator)
  {
    this->Locator->Initialize(); //free storage
  }
  
  this->UpdateProgress(100.0);
  
  return 1;
}

//-------------------------------------------------------------------------------------
void vtkMimxCreateVoxelMesh::CreateDefaultLocator()
{
  if ( this->Locator == NULL)
  {
    this->Locator = vtkMergePoints::New();
  }
}

//-------------------------------------------------------------------------------------
// Description:
// Specify a spatial locator for merging points. By default, 
// an instance of vtkMergePoints is used.
void vtkMimxCreateVoxelMesh::SetLocator(vtkPointLocator *locator)
{
  if ( this->Locator == locator ) 
    {
    return;
    }
  
  if ( this->Locator )
    {
    this->Locator->UnRegister(this);
    this->Locator = NULL;
    }
  
  if (locator)
    {
    locator->Register(this);
    }
  
  this->Locator = locator;
  this->Modified();
}

