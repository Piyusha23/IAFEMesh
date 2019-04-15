/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxIntervertebralDiscAbaqusWriter.h,v $
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


#ifndef __vtkMimxIntervertebralDiscAbaqusWriter_h
#define __vtkMimxIntervertebralDiscAbaqusWriter_h

#include "vtkDataSet.h"
#include "vtkDataSetWriter.h"
#include "vtkPolyData.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"


class vtkMimxIntervertebralDiscAbaqusWriter : public vtkDataSetWriter
{
public:
  
  static vtkMimxIntervertebralDiscAbaqusWriter *New();
  vtkTypeRevisionMacro(vtkMimxIntervertebralDiscAbaqusWriter,vtkDataSetWriter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  vtkSetMacro( Modulus, double );
  vtkSetMacro( PoissonsRatio, double );
  vtkSetMacro( StartingNodeNumber, int );
  vtkSetMacro( StartingElementNumber, int);
  void SetDisc(vtkUnstructuredGrid* );
  void SetDiscBoundingBox(vtkUnstructuredGrid* );
protected:
  
  vtkMimxIntervertebralDiscAbaqusWriter();
  ~vtkMimxIntervertebralDiscAbaqusWriter();

  void UnstGridExecute( vtkUnstructuredGrid * Mesh, vtkUnstructuredGrid * BBox );
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int StartingNodeNumber;
  int StartingElementNumber;

private:
  
  double Modulus;
  double PoissonsRatio;

  vtkMimxIntervertebralDiscAbaqusWriter(const vtkMimxIntervertebralDiscAbaqusWriter&);  // Not implemented.
  void operator=(const vtkMimxIntervertebralDiscAbaqusWriter&);  // Not implemented.

};

#endif
