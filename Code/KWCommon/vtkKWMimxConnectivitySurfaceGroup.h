/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxConnectivitySurfaceGroup.h,v $
Language:  C++
Date:      $Date: 2013/03/17 00:39:12 $
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
// vtkPolyDataConnectivityFilter 
// NOTE: FILL In Information.

#ifndef __vtkKWMimxConnectivitySurfaceGroup_h
#define __vtkKWMimxConnectivitySurfaceGroup_h

#include "vtkKWMimxMenuOptionGroup.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"

#include "vtkMimxKWCommonWin32Header.h"


class vtkActor;
class vtkPolyDataMapper;
class vtkPolyDataConnectivityFilter;

class vtkKWComboBoxWithLabel;
class vtkKWLoadSaveDialog;
class vtkKWMenuButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWFrameWithLabel;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxConnectivitySurfaceGroup : public vtkKWMimxMenuOptionGroup
{
public:
  static vtkKWMimxConnectivitySurfaceGroup* New();
  vtkTypeRevisionMacro(vtkKWMimxConnectivitySurfaceGroup,vtkKWMimxMenuOptionGroup);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Update();
  virtual void UpdateEnableState();
  
  // Button Press callbacks
  //void ConnectivitySurfaceDoneCallback();
  void ConnectivitySurfaceCancelCallback();
  int  ConnectivitySurfaceApplyCallback();
  void ConnectivitySurfaceCallback();
  void AppendSurfaceCallback();

  // Update Status Methods
  void UpdateMergeFrame();
  void UpdateObjectLists();


protected:
	vtkKWMimxConnectivitySurfaceGroup();
	~vtkKWMimxConnectivitySurfaceGroup();
	
	// Internally Used VTK Objects
	vtkPolyDataMapper *SelectedSurfaceMapper;
	vtkActor *SelectedSurfaceActor;
	vtkPolyDataConnectivityFilter *Connectivity;
	
	// Internally used KWWidget Objects
	vtkKWMenuButtonWithLabel *Menubutton;
	vtkKWMultiColumnListWithScrollbars *MultiColumnList;
	vtkKWFrameWithLabel *ComponentFrame;
	vtkKWComboBoxWithLabel *ObjectListComboBox;
	vtkKWLoadSaveDialog *FileBrowserDialog;

	virtual void CreateWidget();
	

private:
  vtkKWMimxConnectivitySurfaceGroup(const vtkKWMimxConnectivitySurfaceGroup&); // Not implemented
  void operator=(const vtkKWMimxConnectivitySurfaceGroup&); // Not implemented
};

#endif

