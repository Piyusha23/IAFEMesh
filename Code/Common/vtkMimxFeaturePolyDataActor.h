/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkMimxFeaturePolyDataActor.h,v $
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

// .NAME vtkMimxFeaturePolyDataActor - Class for representation of polydataactor pipeline.
// .SECTION Description
// vtkMimxFeaturePolyDataActor contains pipeline and display property specification for a
// Feature line.

#ifndef __vtkMimxFeaturePolyDataActor_h
#define __vtkMimxFeaturePolyDataActor_h

#include "vtkMimxActorBase.h"
#include "vtkMimxCommonWin32Header.h"

#include <string>
#include <list>

class vtkActor;
class vtkDoubleArray;
class vtkFeatureEdges;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTubeFilter;
class vtkFeatureEdges;
class vtkLookupTable;
class vtkExtractCells;
class vtkGeometryFilter;
class vtkShrinkFilter;
class vtkScalarBarActor;
class vtkIdList;
class vtkStdString;

class VTK_MIMXCOMMON_EXPORT vtkMimxFeaturePolyDataActor : public vtkMimxActorBase
{
public:
  
  enum {
    DisplaySurface                = 1,
    DisplayOutline                = 2,
    DisplaySurfaceAndOutline      = 3
  };
  
  static vtkMimxFeaturePolyDataActor *New();
  vtkTypeRevisionMacro(vtkMimxFeaturePolyDataActor,vtkMimxActorBase);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //Get the vtkPolyData being displayed in the actor
  vtkPolyData* GetDataSet();
  
  //Set the color of the actor being displayed
  void SetColor(double rgb[3]);
  void SetColor(double red, double green, double blue);
  
  //Get/Set the color of the actor being displayed
  void GetColor(double &red, double &green, double &blue);
  void GetColor(double rgb[3]);
  
  //Get/Set the line thickness of the feature
  double GetLineThickness();
  void SetLineThickness(double thickness);
  
  //Get/Set the dataset type being displayed
  void SetDisplayType( int mode );
  int  GetDisplayType();
  
protected:
  vtkMimxFeaturePolyDataActor();
  ~vtkMimxFeaturePolyDataActor();
  
  
  
private:
  vtkMimxFeaturePolyDataActor(const vtkMimxFeaturePolyDataActor&);  // Not implemented.
  void operator=(const vtkMimxFeaturePolyDataActor&);  // Not implemented.
  double m_Color[3];
	double m_LineThickness;
	int    m_DisplayType;
  vtkPolyData *PolyData;
  vtkPolyDataMapper *PolyDataMapper;
  
};

#endif


