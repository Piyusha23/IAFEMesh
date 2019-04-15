/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxUnstructuredGridActor.h,v $
Language:  C++
Date:      $Date: 2013/01/02 17:45:57 $
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

// .NAME vtkMimxUnstructuredGridActor
// .SECTION Description
// vtkMimxUnstructuredGridActor is the class to handle display and manipulation
// of Bounding Boxes.


#ifndef __vtkMimxUnstructuredGridActor_h
#define __vtkMimxUnstructuredGridActor_h


//MIMX Headers
#include "vtkMimxCommonWin32Header.h"
#include "vtkMimxActorBase.h"
#include "mimxMatrixTemplate.h"

class vtkActor;
class vtkDataSetMapper;
class vtkIdList;
class vtkUnstructuredGrid;

class VTK_MIMXCOMMON_EXPORT vtkMimxUnstructuredGridActor : public vtkMimxActorBase
{
public:

  static vtkMimxUnstructuredGridActor *New();
  vtkTypeRevisionMacro(vtkMimxUnstructuredGridActor,vtkMimxActorBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  enum { 
    DisplaySurface                = 1,
    DisplayOutline                = 2,
    DisplaySurfaceAndOutline      = 3
  };
  
  
 //vtkDataSet* GetDataSet();
  vtkUnstructuredGrid* GetDataSet();
 // functions related to mesh seeding
 void ComputeMeshSeed(double AvElLength);
 int MeshSeedFromAverageElementLength(double, double, double);
 void ChangeMeshSeed(int, int, int);	// to change mesh seed and propagate the change
 // through all the cells whose mesh seed is changed
 //void SetDataType(int );
 vtkSetMacro(ElementSetName, char*);
 vtkGetMacro(ElementSetName, char*);
 void ChangeMeshSeedAvElLength(int BoxNum, int Axis, double ElementLength);
 int GetMeshSeed(int BoxNum, int Axis);
 void DeleteNodeSet(const char *Name);
 void DeleteElementSet(const char *Name);
 void SetFillColor(double rgb[3]);
 void SetFillColor(double red, double green, double blue);
 void GetFillColor(double &red, double &green, double &blue);
 void GetFillColor(double rgb[3]);
 void SetOutlineColor(double rgb[3]);
 void SetOutlineColor(double red, double green, double blue);
 void GetOutlineColor(double &red, double &green, double &blue);
 void GetOutlineColor(double rgb[3]);
 void SetDisplayType( int mode );
 int  GetDisplayType();
  
protected:
  vtkMimxUnstructuredGridActor();
  ~vtkMimxUnstructuredGridActor();
  vtkUnstructuredGrid *UnstructuredGrid;
  vtkDataSetMapper *UnstructuredGridMapper;
private:
	int Links; // to check if links need to be recomputed
	void BuildMeshSeedLinks(); // generates a matrix of values signifying the connection
	// between different cells. this helps in calculation of
	// mesh seeds of cells that are not immediate neighbors.
	void GetCellNeighbors(vtkIdType, vtkIdList*);	// get cell neighbors that have not
	// been traversed
	int CheckIfCellsShareEdgeX(int , int , int, int);
	int CheckIfCellsShareEdgeY(int , int , int, int);
	int CheckIfCellsShareEdgeZ(int , int , int, int);
	void EstimateMeshSeedsBasedOnAverageElementLength(double, double, double);
	//	the common edge could be x, y, z on the cell being compared
	int WhichEdgeOfCellBeingCompared(vtkIdType, vtkIdType);
	// mesh seed links
	// size is determined by numberofcells in the unstructuredgrid*3
	// matrix to store cell connectivity information
	CMatrix<int> MeshSeedLinks;
	// matrix for book keeping on cell neighbors visited
	CMatrix<int> MeshSeedCheck;
	// to be used when mesh seeding on average element length is concidered
	void SetHigherNumberedBBoxMeshSeed(int , int );
	char *ElementSetName;
	double FillColor[3];
	double OutlineColor[3];
	int DisplayType;
  vtkMimxUnstructuredGridActor(const vtkMimxUnstructuredGridActor&);  // Not implemented.
  void operator=(const vtkMimxUnstructuredGridActor&);  // Not implemented.
};

#endif

