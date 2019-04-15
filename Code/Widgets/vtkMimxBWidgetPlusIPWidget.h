/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxBWidgetPlusIPWidget.h,v $
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

#ifndef _vtkBWidgetPlusIPWidget_h
#define _vtkBWidgetPlusIPWidget_h

#include "vtkUnstructuredGrid.h"
#include "vtkDataSetMapper.h"

//	class for storing polydata generated from Tracing
class vtkBWidgetPlusIPWidget 
{
public:
	vtkBWidgetPlusIPWidget();
	~vtkBWidgetPlusIPWidget();
	void SetInput();
private:
	vtkDataSetMapper* UGridMapper;
	vtkActor* Actor;
	char *FileName;
	double PrevColor[3];
protected:
};

#endif
