/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxFeaturePolyDataActor.cxx,v $
Language:  C++
Date:      $Date: 2013/02/24 20:39:58 $
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

#include "vtkMimxFeaturePolyDataActor.h"

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkExtractCells.h"
#include "vtkFeatureEdges.h"
#include "vtkFieldData.h"
#include "vtkGeometryFilter.h"
#include "vtkIdList.h"
#include "vtkLookupTable.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkScalarBarActor.h"
#include "vtkShrinkFilter.h"
#include "vtkStringArray.h"
#include "vtkTextProperty.h"
#include "vtkTransform.h"
#include "vtkTubeFilter.h"
#include "vtkUnstructuredGrid.h"

vtkCxxRevisionMacro(vtkMimxFeaturePolyDataActor, "$Revision: 1.1 $");

vtkStandardNewMacro(vtkMimxFeaturePolyDataActor);

vtkMimxFeaturePolyDataActor::vtkMimxFeaturePolyDataActor()
{
	vtkPoints *points = vtkPoints::New();
  this->PolyData = vtkPolyData::New();
  this->PolyData->SetPoints(points);
  points->Delete();
	this->PolyDataMapper = vtkPolyDataMapper::New();
	this->Actor = vtkActor::New();
  
	// Set up the pipe line
	this->PolyDataMapper->SetInput(this->PolyData);
	this->Actor->SetMapper(this->PolyDataMapper);
	this->m_Color[0] = this->m_Color[1] = this->m_Color[2] = 1.0;
	this->SetDisplayType( DisplayOutline );
  
	this->m_Color[0] = 1.0;
	this->m_Color[1] = 1.0;
	this->m_Color[2] = 1.0;
  this->m_LineThickness = 1.0;
}

//----------------------------------------------------------------------------------
vtkMimxFeaturePolyDataActor::~vtkMimxFeaturePolyDataActor()
{
  if(this->PolyData)
    this->PolyData->Delete();
	if (this->PolyDataMapper)
    this->PolyDataMapper->Delete();
	if (this->Actor)
    this->Actor->Delete();
}

//----------------------------------------------------------------------------------
vtkPolyData* vtkMimxFeaturePolyDataActor::GetDataSet()
{
	return this->PolyData;
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::SetColor(double rgb[3])
{
  this->SetColor(rgb[0], rgb[1], rgb[2]);
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::SetColor(double red, double green, double blue)
{
  this->m_Color[0] = red;
  this->m_Color[1] = green;
  this->m_Color[2] = blue;
  
  this->Actor->GetProperty()->SetColor(red, green, blue);
  this->Actor->Modified();
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::GetColor(double &red, double &green, double &blue)
{
  red   = this->m_Color[0];
  green = this->m_Color[1];
  blue  = this->m_Color[2];
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::GetColor(double rgb[3])
{
  rgb[0]   = this->m_Color[0];
  rgb[1]   = this->m_Color[1];
  rgb[2]   = this->m_Color[2];
}

//----------------------------------------------------------------------------------
double vtkMimxFeaturePolyDataActor::GetLineThickness()
{
  return this->m_LineThickness;
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::SetLineThickness(double thickness)
{
  this->m_LineThickness = thickness;
  this->Actor->GetProperty()->SetLineWidth(thickness);
  this->Actor->Modified();
}

//----------------------------------------------------------------------------------
int vtkMimxFeaturePolyDataActor::GetDisplayType( )
{
  return this->m_DisplayType;
}

//----------------------------------------------------------------------------------
void vtkMimxFeaturePolyDataActor::SetDisplayType(int mode)
{
  this->m_DisplayType = mode;
}


