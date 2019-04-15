/*=========================================================================

Program:   MIMX Meshing Toolkit
Module:    $RCSfile: vtkKWMimxMergeBBGroup.h,v $
Language:  C++
Date:      $Date: 2012/12/31 01:01:56 $
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
// .NAME vtkKWMimxMergeBBGroup - a taFEMeshed notebook of UI pages
// .SECTION Description
// The class is derived from vtkKWMimxGroupBase. It contains 5 pages 1) Image
// 2) Surface 3) Building Block 4) F E Mesh 5) Mesh quality. Each page inturn
// contains a list of options specific to a page.

#ifndef __vtkKWMimxMergeBBGroup_h
#define __vtkKWMimxMergeBBGroup_h

// KWWidget Headers
#include "vtkKWRadioButton.h"

// MIMX Headers
#include "vtkKWMimxGroupBase.h"
#include "vtkKWMimxFEMeshMenuGroup.h"
#include "vtkKWMimxMainWindow.h"
#include "vtkKWMimxSurfaceMenuGroup.h"
#include "vtkKWMimxEditBBGroup.h"
#include "vtkMimxKWCommonWin32Header.h"

class vtkKWMultiColumnListWithScrollbars;
class vtkKWEntryWithLabel;
class vtkKWRadioButton;

class VTK_MIMXKWCOMMON_EXPORT vtkKWMimxMergeBBGroup : public vtkKWMimxGroupBase
{
public:
  static vtkKWMimxMergeBBGroup* New();
	vtkTypeRevisionMacro(vtkKWMimxMergeBBGroup,vtkKWMimxGroupBase);
	void PrintSelf(ostream& os, vtkIndent indent);
	virtual void Update();
	virtual void UpdateEnableState();
	void MergeBBDoneCallback();
	void MergeBBCancelCallback();
	int MergeBBApplyCallback();
	void UpdateObjectLists();
	vtkSetObjectMacro(VTKRadioButton, vtkKWRadioButton);
	void CopyConstraintValues(vtkUnstructuredGrid *input, vtkUnstructuredGrid *output);
	
protected:
	vtkKWMimxMergeBBGroup();
	~vtkKWMimxMergeBBGroup();
	virtual void CreateWidget();
	vtkKWMultiColumnListWithScrollbars *MultiColumnList;
	vtkKWEntryWithLabel *MergeTolerence;
	//vtkKWMimxEditBBGroup *EditBBGroup;
	vtkKWRadioButton *VTKRadioButton;
	
private:
  vtkKWMimxMergeBBGroup(const vtkKWMimxMergeBBGroup&); // Not implemented
  void operator=(const vtkKWMimxMergeBBGroup&); // Not implemented
};

#endif

