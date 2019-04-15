/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkPatchSurfaceMeshUsingTPS.cxx,v $

  Copyright (c) University of Iowa
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPatchSurfaceMeshUsingTPS.h"

#include "vtkCellArray.h"
#include "vtkDelaunay2D.h"
#include "vtkGeometryFilter.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMergePoints.h"
#include "vtkObjectFactory.h"
#include "vtkPointLocator.h"
#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "vtkPolyData.h"
#include "vtkPolyDataCollection.h"
#include "vtkUnstructuredGrid.h"
#include "vtkSmartPointer.h"
#include "vtkCellArray.h"
#include "vtkThinPlateSplineTransform.h"
#include"vtkTransformFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkFeatureEdges.h"
#include "vtkPointLocator.h"
#include "vtkDelaunay2D.h"
#include "vtkSTLWriter.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkAppendFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkGridTransform.h"
#include "vtkPolyDataReader.h"
#include "vtkAppendPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSetWriter.h"


vtkCxxRevisionMacro(vtkPatchSurfaceMeshUsingTPS, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkPatchSurfaceMeshUsingTPS);

// Construct with all types of clipping turned off.
vtkPatchSurfaceMeshUsingTPS::vtkPatchSurfaceMeshUsingTPS()
{

}

vtkPatchSurfaceMeshUsingTPS::~vtkPatchSurfaceMeshUsingTPS()
{

}

int vtkPatchSurfaceMeshUsingTPS::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	// read the loop
	int	temp = input->GetNumberOfPoints();
	
	
	// read the mesh for std points

    vtkPolyDataReader *meshreader = vtkPolyDataReader::New();
//	meshreader->SetFileName("D:/srinivas/IaFeMesh_March09_2009/Build/bin/debug/Circle_AYS_InternalPts_900.vtk");
	meshreader->SetFileName("Circle_AYS_InternalPts_900.vtk");
	meshreader->Update();


	#define PI 3.14159265
	vtkPoints *pts = vtkPoints::New();
	for(int i = 0;  i< temp ; i++)
	{
		float param, sine, cosine ;
		float theta = 360.0/temp ;
		param = i;
		cosine = 5* cos(theta*i*PI/180);
		sine   = 5* sin(theta*i*PI/180);		
		pts->SetNumberOfPoints(temp);
		pts->SetPoint(i,cosine,sine, 0.0);

	}

	vtkPolyData *pointdata = vtkPolyData::New();
	pointdata->SetPoints(pts);

	vtkAppendFilter *append = vtkAppendFilter::New();
	append->AddInput(pointdata);
	append->AddInput(meshreader->GetOutput());
	append->Update();

	
	// create a cut surface mesh with given # of points on surface 
	vtkDelaunay2D *delaunay1 = vtkDelaunay2D::New();
	delaunay1->SetInput(append->GetOutput());
	delaunay1->Update();   


	///////////////////////////////////////////////////////////////
   vtkPolyData *delaunay = vtkPolyData::New();  
   delaunay->SetPoints(delaunay1->GetOutput()->GetPoints());

   delaunay1->GetOutput()->BuildLinks();
   //vtkCellArray *polyarray = delaunay1->GetOutput()->GetPolys();

   vtkCellArray *localArray = vtkCellArray::New();
   for (int i = 0; i<delaunay1->GetOutput()->GetNumberOfCells(); i++ )
	{
		vtkIdType npts;  //vtkIdType npts1;
		vtkIdType *pts ;  //vtkIdType *pts1 ; 
		delaunay1->GetOutput()->GetCellPoints(i, npts, pts);		
		int temp = pts[2];
		pts[2] = pts[0];
		pts[0] = temp;	
		localArray->InsertNextCell(npts, pts);		
	}

	delaunay->SetPolys(localArray);
	localArray->Delete();
	///////////////////////////////////////////////////////////////




//	Delaunay Mesh of the 2D circle with same # of points as on the loop
	//vtkPolyDataNormals *delaunay = vtkPolyDataNormals::New();
	//delaunay->SetInput(delaunay1->GetOutput());
	//delaunay->FlipNormalsOn();
	//delaunay->Update();

	vtkFeatureEdges *featureedge = vtkFeatureEdges::New();
	featureedge->SetInput(delaunay);
	featureedge->Update();

	vtkPointLocator *locator = vtkPointLocator::New();
	vtkPoints *points = vtkPoints::New();
	points->Allocate(delaunay->GetNumberOfPoints());
	locator->InitPointInsertion (points, featureedge->GetOutput()->GetPoints()->GetBounds());
	
	for(int i = 0; i< delaunay->GetNumberOfPoints(); i++)
	{
		locator->InsertNextPoint(delaunay->GetPoint(i));			
	
	}
	locator->BuildLocator();

	vtkPoints *newpoints = vtkPoints::New();
	newpoints->SetNumberOfPoints(input->GetNumberOfPoints());

	for(int i=0; i < input->GetNumberOfPoints(); i++)	
	{
		int location = locator->IsInsertedPoint(featureedge->GetOutput()->GetPoint(i));
		if(location!= -1)
		{		
			newpoints->SetPoint(location,input->GetPoint(i));		
		}
		
	}
	points->Squeeze();

	vtkPolyData *UndefPoly = delaunay;
	vtkPolyData *DefPoly = input ; 

	vtkPoints *UndefPoints = vtkPoints::New();
	vtkPoints *DefPoints = vtkPoints::New();

	// Consider only surface nodes
	for (int i=0; i <featureedge->GetOutput()->GetNumberOfPoints(); i++)
	{
		DefPoints->InsertNextPoint(DefPoly->GetPoint(i));
		UndefPoints->InsertNextPoint(UndefPoly->GetPoint(i));
		
	}

	vtkThinPlateSplineTransform *thinTransform = vtkThinPlateSplineTransform::New( );	
	thinTransform->SetBasisToR( );	
//	thinTransform->SetBasisToR2LogR();
//	thinTransform->SetSigma(2000000000);
	thinTransform->SetSourceLandmarks( UndefPoints);
	thinTransform->SetTargetLandmarks( DefPoints);
//	thinTransform->SetTargetLandmarks( newpoints);
	thinTransform->Update( );

	vtkTransformFilter *finalTransform = vtkTransformFilter::New( );
	finalTransform->SetInput(delaunay);
	finalTransform->SetTransform( thinTransform );
	finalTransform->Update( );

//	vtkPolyData *poly = vtkPolyData::SafeDownCast(finalTransform->GetOutput());
	
	output->DeepCopy(vtkPolyData::SafeDownCast(finalTransform->GetOutput()));

	
	pts->Delete();
	pointdata->Delete();
	append->Delete();
	delaunay1->Delete();
	delaunay->Delete();
	featureedge->Delete();
	locator->Delete();
	points->Delete();
	newpoints->Delete();


  return 1;
}


int vtkPatchSurfaceMeshUsingTPS::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

void vtkPatchSurfaceMeshUsingTPS::PrintSelf(ostream& os, vtkIndent indent)
{
 
}

