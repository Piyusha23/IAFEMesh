/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxSolidStructuredGridSource.cxx,v $
Language:  C++
Date:      $Date: 2012/12/07 19:08:59 $
Version:   $Revision: 1.1.1.1 $

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

#include "vtkMimxSolidStructuredGridSource.h"


#include "vtkObjectFactory.h"
#include "vtkSphereSource.h"
#include "vtkCellArray.h"
#include "vtkHexahedron.h"
#include "vtkDataSetMapper.h"
#include "vtkProperty.h"


///////////////////////////////////////////////////////////////
vtkCxxRevisionMacro(vtkMimxSolidStructuredGridSource, "$Revision: 1.1.1.1 $");
vtkStandardNewMacro(vtkMimxSolidStructuredGridSource);

vtkMimxSolidStructuredGridSource::vtkMimxSolidStructuredGridSource()
{
	StructuredGrid = vtkStructuredGrid::New();
	Mapper = vtkDataSetMapper::New();
	Actor = vtkActor::New();
	this->Mapper->SetInput(this->StructuredGrid);
	this->Actor->SetMapper(this->Mapper);
	this->StructuredGrid->Modified();
}

vtkMimxSolidStructuredGridSource::~vtkMimxSolidStructuredGridSource()
{
	Mapper->Delete();
	Actor->Delete();
}

void vtkMimxSolidStructuredGridSource::SetStructuredGrid(vtkStructuredGrid *SGrid)
{
	this->Mapper->SetInput(this->StructuredGrid);
	this->Actor->SetMapper(this->Mapper);
	this->StructuredGrid->Modified();
}
