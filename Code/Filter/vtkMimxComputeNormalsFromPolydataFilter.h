/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxComputeNormalsFromPolydataFilter.h,v $
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


#ifndef __vtkMimxComputeNormalsFromPolydataFilter_h
#define __vtkMimxComputeNormalsFromPolydataFilter_h

#include "vtkPointSetAlgorithm.h"

//MIMX Headers
#include "vtkMimxFilterWin32Header.h"


class VTK_MIMXFILTER_EXPORT vtkMimxComputeNormalsFromPolydataFilter : public vtkPointSetAlgorithm
{
public:
  static vtkMimxComputeNormalsFromPolydataFilter *New();
  vtkTypeRevisionMacro(vtkMimxComputeNormalsFromPolydataFilter,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  
protected:
  vtkMimxComputeNormalsFromPolydataFilter();
  ~vtkMimxComputeNormalsFromPolydataFilter();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
private:
  vtkMimxComputeNormalsFromPolydataFilter(const vtkMimxComputeNormalsFromPolydataFilter&);  // Not implemented.
  void operator=(const vtkMimxComputeNormalsFromPolydataFilter&);  // Not implemented.

};

#endif
