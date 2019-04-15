/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxSurfaceDensityGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/16 22:16:55 $
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
// Decimation and Linear subdivision of surfaces can be performed . Uses 
// vtkDecimatePro and vtkLinearSubdivisionFilter filters. User can 
// intearctively changed the percentage decimation/ No of subdivisions 
// and the initial and the final no of triangles on the surface are also shown.

#ifndef __vtkKWMimxSurfaceDensityGroup_h
#define __vtkKWMimxSurfaceDensityGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"


class vtkActor;
class vtkLinearSubdivisionFilter;
class vtkPolyDataMapper;
class vtkDecimatePro ;
class vtkKWMenuButtonWithLabel;
class vtkKWComboBoxWithLabel;
class vtkKWEntryWithLabel;
class vtkKWLoadSaveDialog;




class vtkKWMimxSurfaceDensityGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxSurfaceDensityGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxSurfaceDensityGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual void Update();
  virtual void UpdateEnableState();
  
  void SaveVTKSurfaceDoneCallback();
  void Dec_SubSurfaceCancelCallback();
  int Dec_SubSurfaceApplyCallback();
  void UpdateObjectLists();
  void DecimateSurfaceCallback();
  void LoopSubdivisionSurfaceCallback();
  void Decimatevaluechangedcallback(const char *);
  void Subdivisionvaluechangedcallback (const char *);

protected:
	vtkKWMimxSurfaceDensityGroup();
	~vtkKWMimxSurfaceDensityGroup();
	
	vtkPolyDataMapper *SelectedSurfaceMapper;
	vtkActor *SelectedSurfaceActor;
	vtkKWMenuButtonWithLabel *Menubutton;	
	vtkDecimatePro *Decimate ;
	vtkLinearSubdivisionFilter *LoopSubdivide ;
	vtkKWEntryWithLabel *Entrybutton;
	vtkKWEntryWithLabel *Entrybutton1;
	vtkKWEntryWithLabel *Entrybutton_InitTri;
	vtkKWEntryWithLabel *Entrybutton_FinaTri;
	
	virtual void CreateWidget();
	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWLoadSaveDialog *FileBrowserDialog;

private:
  vtkKWMimxSurfaceDensityGroup(const vtkKWMimxSurfaceDensityGroup&); // Not implemented
  void operator=(const vtkKWMimxSurfaceDensityGroup&); // Not implemented
};

#endif

