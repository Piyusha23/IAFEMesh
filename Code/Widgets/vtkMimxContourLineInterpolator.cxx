/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMimxContourLineInterpolator.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMimxContourLineInterpolator.h"

#include "vtkMimxContourRepresentation.h"
#include "vtkIntArray.h"

vtkCxxRevisionMacro(vtkMimxContourLineInterpolator, "$Revision: 1.1 $");

//----------------------------------------------------------------------
vtkMimxContourLineInterpolator::vtkMimxContourLineInterpolator()
{
}

//----------------------------------------------------------------------
vtkMimxContourLineInterpolator::~vtkMimxContourLineInterpolator()
{
}

//----------------------------------------------------------------------
int vtkMimxContourLineInterpolator::UpdateNode( vtkRenderer *, 
                                            vtkMimxContourRepresentation *,
                 double * vtkNotUsed(node), int vtkNotUsed(idx) )
{
  return 0;
}

//----------------------------------------------------------------------
void vtkMimxContourLineInterpolator::GetSpan( int nodeIndex,
                                          vtkIntArray *nodeIndices,
                                          vtkMimxContourRepresentation *rep)
{
  int index[2] = { nodeIndex - 1, nodeIndex };

  // Clear the array
  nodeIndices->Reset();
  nodeIndices->Squeeze();
  nodeIndices->SetNumberOfComponents(2);

  if ( rep->GetClosedLoop() )
    {
    ++index[0];
    ++index[1];
    if ( index[0] < 0 )
      {
      index[0] += rep->GetNumberOfNodes();
      }
    if ( index[1] < 0 )
      {
      index[1] += rep->GetNumberOfNodes();
      }
    if ( index[0] >= rep->GetNumberOfNodes() )
      {
      index[0] -= rep->GetNumberOfNodes();
      }
    if ( index[1] >= rep->GetNumberOfNodes() )
      {
      index[1] -= rep->GetNumberOfNodes();
      }
    }

  if ( index[0] >= 0 && index[0] < rep->GetNumberOfNodes() &&
       index[1] >= 0 && index[1] < rep->GetNumberOfNodes() )
    {
    // Sanity check
    nodeIndices->InsertNextTupleValue(index);
    }
}

//----------------------------------------------------------------------
void vtkMimxContourLineInterpolator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);  
}
